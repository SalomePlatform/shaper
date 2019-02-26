// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "FeaturesPlugin_BooleanFill.h"
#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeCustom.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_Partition.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_Face.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

#include <algorithm>
#include <map>

//=================================================================================================
FeaturesPlugin_BooleanFill::FeaturesPlugin_BooleanFill()
  : FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_FILL)
{
}

//=================================================================================================
void FeaturesPlugin_BooleanFill::execute()
{
  std::string anError;
  ListOfShape anObjects, aTools, anEdgesAndFaces, aPlanes;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompSolidsObjects;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList =
    selectionList(FeaturesPlugin_Boolean::OBJECT_LIST_ID());
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    ResultPtr aContext = anObjectAttr->context();
    ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(aContext);
    if(aResCompSolidPtr.get()
        && aResCompSolidPtr->shape()->shapeType() == GeomAPI_Shape::COMPSOLID) {
      std::shared_ptr<GeomAPI_Shape> aContextShape = aResCompSolidPtr->shape();
      std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
        anIt = aCompSolidsObjects.begin();
      for(; anIt != aCompSolidsObjects.end(); anIt++) {
        if(anIt->first->isEqual(aContextShape)) {
          aCompSolidsObjects[anIt->first].push_back(anObject);
          break;
        }
      }
      if(anIt == aCompSolidsObjects.end()) {
        aCompSolidsObjects[aContextShape].push_back(anObject);
      }
    } else {
      anObjects.push_back(anObject);
    }
  }

  // Getting tools.
  AttributeSelectionListPtr aToolsSelList = selectionList(FeaturesPlugin_Boolean::TOOL_LIST_ID());
  for(int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
    AttributeSelectionPtr aToolAttr = aToolsSelList->value(aToolsIndex);
    GeomShapePtr aTool = aToolAttr->value();
    if(!aTool.get()) {
      // It could be a construction plane.
      ResultPtr aContext = aToolAttr->context();
      aPlanes.push_back(aToolAttr->context()->shape());
    }
    else {
      aTools.push_back(aTool);
    }
  }

  int aResultIndex = 0;

  if ((anObjects.empty() && aCompSolidsObjects.empty())
      || (aTools.empty() && aPlanes.empty())) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  std::vector<FeaturesPlugin_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;

  // For solids cut each object with all tools.
  for(ListOfShape::iterator
      anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
    std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
    ListOfShape aListWithObject;
    aListWithObject.push_back(anObject);
    std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
    std::shared_ptr<GeomAlgoAPI_MakeShape> aBoolAlgo;
    GeomShapePtr aResShape;

    std::list<std::shared_ptr<GeomAPI_Pnt> > aBoundingPoints =
        GeomAlgoAPI_ShapeTools::getBoundingBox(aListWithObject, 1.0);

    // Resize planes.
    ListOfShape aToolsWithPlanes = aTools;
    for(ListOfShape::const_iterator anIt = aPlanes.cbegin();
                                    anIt != aPlanes.cend();
                                    ++anIt)
    {
      GeomShapePtr aPlane = *anIt;
      GeomShapePtr aTool = GeomAlgoAPI_ShapeTools::fitPlaneToBox(aPlane, aBoundingPoints);
      std::shared_ptr<GeomAlgoAPI_MakeShapeCustom> aMkShCustom(
        new GeomAlgoAPI_MakeShapeCustom);
      aMkShCustom->addModified(aPlane, aTool);
      aMakeShapeList->appendAlgo(aMkShCustom);
      aToolsWithPlanes.push_back(aTool);
    }

    aBoolAlgo.reset(new GeomAlgoAPI_Partition(aListWithObject, aToolsWithPlanes));
    aResShape = aBoolAlgo->shape();
    if (aResShape.get() && aResShape->shapeType() == GeomAPI_Shape::COMPOUND) {
      int aSubResultsNb = 0;
      GeomAPI_ShapeIterator anIt(aResShape);
      for(; anIt.more(); anIt.next()) {
        ++aSubResultsNb;
      }
      if(aSubResultsNb == 1) {
        anIt.init(aResShape);
        if(anIt.more()) {
          aResShape = anIt.current();
        }
      }
    }

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aMakeShapeList->appendAlgo(aBoolAlgo);

    std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

    // tools should be added to the list to fulfill the correct history of modification
    aListWithObject.insert(aListWithObject.end(), aTools.begin(), aTools.end());

    ListOfShape aUsedTools = aTools;
    aUsedTools.insert(aUsedTools.end(), aPlanes.begin(), aPlanes.end());

    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, aListWithObject, aUsedTools,
                                             aMakeShapeList, aResShape);
    setResult(aResultBody, aResultIndex);
    aResultIndex++;

    FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
    aRBA.resultBody = aResultBody;
    aRBA.baseShape = anObject;
    aRBA.makeShape = aMakeShapeList;
    aResultBaseAlgoList.push_back(aRBA);
    aResultShapesList.push_back(aResShape);
  }

  // Compsolids handling
  for(std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
      anIt = aCompSolidsObjects.begin();
      anIt != aCompSolidsObjects.end(); anIt++) {
    std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
    ListOfShape& aUsedInOperationSolids = anIt->second;

    // Collecting solids from compsolids which will not be modified in boolean operation.
    ListOfShape aNotUsedSolids;
    for(GeomAPI_ShapeExplorer
        anExp(aCompSolid, GeomAPI_Shape::SOLID); anExp.more(); anExp.next()) {
      std::shared_ptr<GeomAPI_Shape> aSolidInCompSolid = anExp.current();
      ListOfShape::iterator anIt = aUsedInOperationSolids.begin();
      for(; anIt != aUsedInOperationSolids.end(); anIt++) {
        if(aSolidInCompSolid->isEqual(*anIt)) {
          break;
        }
      }
      if(anIt == aUsedInOperationSolids.end()) {
        aNotUsedSolids.push_back(aSolidInCompSolid);
      }
    }

    std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
    std::shared_ptr<GeomAlgoAPI_MakeShape> aBoolAlgo;

    std::list<std::shared_ptr<GeomAPI_Pnt> > aBoundingPoints =
      GeomAlgoAPI_ShapeTools::getBoundingBox(aUsedInOperationSolids, 1.0);

    // Resize planes.
    ListOfShape aToolsWithPlanes = aTools;
    for(ListOfShape::const_iterator anIt = aPlanes.cbegin();
                                    anIt != aPlanes.cend();
                                    ++anIt)
    {
      GeomShapePtr aPlane = *anIt;
      GeomShapePtr aTool = GeomAlgoAPI_ShapeTools::fitPlaneToBox(aPlane, aBoundingPoints);
      std::shared_ptr<GeomAlgoAPI_MakeShapeCustom> aMkShCustom(
        new GeomAlgoAPI_MakeShapeCustom);
      aMkShCustom->addModified(aPlane, aTool);
      aMakeShapeList->appendAlgo(aMkShCustom);
      aToolsWithPlanes.push_back(aTool);
    }

    aBoolAlgo.reset(new GeomAlgoAPI_Partition(aUsedInOperationSolids, aToolsWithPlanes));

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aMakeShapeList->appendAlgo(aBoolAlgo);
    GeomShapePtr aResultShape = aBoolAlgo->shape();

    // Add result to not used solids from compsolid.
    if(!aNotUsedSolids.empty()) {
      ListOfShape aShapesToAdd = aNotUsedSolids;
      aShapesToAdd.push_back(aBoolAlgo->shape());
      std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
        new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
      if(!aFillerAlgo->isDone()) {
        std::string aFeatureError = "Error: PaveFiller algorithm failed.";
        setError(aFeatureError);
        return;
      }

      aMakeShapeList->appendAlgo(aFillerAlgo);
      aResultShape = aFillerAlgo->shape();
    }

    std::shared_ptr<ModelAPI_ResultBody> aResultBody =
      document()->createBody(data(), aResultIndex);

    ListOfShape aUsedTools = aTools;
    aUsedTools.insert(aUsedTools.end(), aPlanes.begin(), aPlanes.end());

    ListOfShape aBaseShapes;
    aBaseShapes.push_back(aCompSolid);
    // tools should be added to the list to fulfill the correct history of modification
    aBaseShapes.insert(aBaseShapes.end(), aTools.begin(), aTools.end());

    FeaturesPlugin_Tools::loadModifiedShapes(aResultBody, aBaseShapes, aUsedTools,
                                             aMakeShapeList, aResultShape);
    setResult(aResultBody, aResultIndex);
    aResultIndex++;

    FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
    aRBA.resultBody = aResultBody;
    aRBA.baseShape = aCompSolid;
    aRBA.makeShape = aMakeShapeList;
    aResultBaseAlgoList.push_back(aRBA);
    aResultShapesList.push_back(aResultShape);
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  GeomShapePtr aResultShapesCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  FeaturesPlugin_Tools::loadDeletedShapes(aResultBaseAlgoList, aTools, aResultShapesCompound);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
