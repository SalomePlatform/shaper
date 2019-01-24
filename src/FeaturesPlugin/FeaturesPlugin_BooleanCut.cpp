// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include "FeaturesPlugin_BooleanCut.h"

#include "FeaturesPlugin_Tools.h"

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAlgoAPI_Tools.h>

#include <GeomAPI_ShapeExplorer.h>
#include <GeomAPI_ShapeIterator.h>

//==================================================================================================
FeaturesPlugin_BooleanCut::FeaturesPlugin_BooleanCut()
: FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_CUT)
{
}

//==================================================================================================
void FeaturesPlugin_BooleanCut::execute()
{
  ListOfShape anObjects, aTools;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompSolidsObjects;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompoundObjects;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(OBJECT_LIST_ID());
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    ResultPtr aContext = anObjectAttr->context();
    ResultBodyPtr aResCompSolidPtr = ModelAPI_Tools::bodyOwner(aContext);
    if (aResCompSolidPtr.get())
    {
      std::shared_ptr<GeomAPI_Shape> aContextShape = aResCompSolidPtr->shape();
      GeomAPI_Shape::ShapeType aShapeType = aResCompSolidPtr->shape()->shapeType();
      std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>& aMap =
        aShapeType == GeomAPI_Shape::COMPSOLID ? aCompSolidsObjects : aCompoundObjects;

        std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
          anIt = aMap.begin();
        for (; anIt != aMap.end(); anIt++) {
          if (anIt->first->isEqual(aContextShape)) {
            aMap[anIt->first].push_back(anObject);
            break;
          }
        }
        if (anIt == aMap.end()) {
          aMap[aContextShape].push_back(anObject);
        }

    } else {
      anObjects.push_back(anObject);
    }
  }

  // Getting tools.
  AttributeSelectionListPtr aToolsSelList = selectionList(TOOL_LIST_ID());
  for(int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
    AttributeSelectionPtr aToolAttr = aToolsSelList->value(aToolsIndex);
    GeomShapePtr aTool = aToolAttr->value();
    if(!aTool.get()) {
      return;
    }
    aTools.push_back(aTool);
  }

  int aResultIndex = 0;

  if((anObjects.empty() && aCompSolidsObjects.empty() && aCompoundObjects.empty())
     || aTools.empty()) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  std::vector<FeaturesPlugin_Tools::ResultBaseAlgo> aResultBaseAlgoList;
  ListOfShape aResultShapesList;
  std::string anError;

  // For solids cut each object with all tools.
  for(ListOfShape::iterator anObjectsIt = anObjects.begin();
      anObjectsIt != anObjects.end();
      ++anObjectsIt) {
    std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
    std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
    std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(
      new GeomAlgoAPI_Boolean(anObject,
                              aTools,
                              GeomAlgoAPI_Boolean::BOOL_CUT));
    GeomShapePtr aResShape = aCutAlgo->shape();

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCutAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aMakeShapeList->appendAlgo(aCutAlgo);

    GeomAPI_ShapeIterator aShapeIt(aResShape);
    if (aShapeIt.more() || aResShape->shapeType() == GeomAPI_Shape::VERTEX)
    {
      std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

      ListOfShape anObjectList;
      anObjectList.push_back(anObject);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               anObjectList,
                                               aTools,
                                               aMakeShapeList,
                                               aResShape);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;

      FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
      aRBA.resultBody = aResultBody;
      aRBA.baseShape = anObject;
      aRBA.makeShape = aMakeShapeList;
      aResultBaseAlgoList.push_back(aRBA);
      aResultShapesList.push_back(aResShape);
    }
  }

  // Compsolids handling
  for (std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
       anIt = aCompSolidsObjects.begin();
       anIt != aCompSolidsObjects.end();
       ++anIt)
  {
    std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
    ListOfShape& aUsedInOperationSolids = anIt->second;

    // Collecting solids from compsolids which will not be modified in boolean operation.
    ListOfShape aNotUsedSolids;
    for(GeomAPI_ShapeExplorer anExp(aCompSolid, GeomAPI_Shape::SOLID);
        anExp.more();
        anExp.next())
    {
      std::shared_ptr<GeomAPI_Shape> aSolidInCompSolid = anExp.current();
      ListOfShape::iterator aUsedIt = aUsedInOperationSolids.begin();
      for (; aUsedIt != aUsedInOperationSolids.end(); aUsedIt++) {
        if (aSolidInCompSolid->isEqual(*aUsedIt)) {
          break;
        }
      }
      if (aUsedIt == aUsedInOperationSolids.end()) {
        aNotUsedSolids.push_back(aSolidInCompSolid);
      }
    }

    std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
    std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(
      new GeomAlgoAPI_Boolean(aUsedInOperationSolids,
                              aTools,
                              GeomAlgoAPI_Boolean::BOOL_CUT));

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCutAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aMakeShapeList->appendAlgo(aCutAlgo);
    GeomShapePtr aResultShape = aCutAlgo->shape();

    // Add result to not used solids from compsolid.
    if(!aNotUsedSolids.empty()) {
      ListOfShape aShapesToAdd = aNotUsedSolids;
      aShapesToAdd.push_back(aCutAlgo->shape());
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

    GeomAPI_ShapeIterator aShapeIt(aResultShape);
    if (aShapeIt.more() || aResultShape->shapeType() == GeomAPI_Shape::VERTEX)
    {
      std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

      ListOfShape anObjectList;
      anObjectList.push_back(aCompSolid);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               anObjectList,
                                               aTools,
                                               aMakeShapeList,
                                               aResultShape);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;

      FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
      aRBA.resultBody = aResultBody;
      aRBA.baseShape = aCompSolid;
      aRBA.makeShape = aMakeShapeList;
      aResultBaseAlgoList.push_back(aRBA);
      aResultShapesList.push_back(aResultShape);
    }
  }

  // Compounds handling
  for (std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
       anIt = aCompoundObjects.begin();
       anIt != aCompoundObjects.end();
       ++anIt)
  {
    std::shared_ptr<GeomAPI_Shape> aCompound = anIt->first;
    ListOfShape& aUsedInOperationShapes = anIt->second;

    // Collecting shapes from compound which will not be modified in boolean operation.
    ListOfShape aNotUsedShapes;
    for (GeomAPI_ShapeIterator aCompIt(aCompound);
         aCompIt.more();
         aCompIt.next())
    {
      std::shared_ptr<GeomAPI_Shape> aShapeInCompound = aCompIt.current();
      ListOfShape::iterator aUsedIt = aUsedInOperationShapes.begin();
      for (; aUsedIt != aUsedInOperationShapes.end(); aUsedIt++) {
        if (aShapeInCompound->isEqual(*aUsedIt)) {
          break;
        }
      }
      if (aUsedIt == aUsedInOperationShapes.end()) {
        aNotUsedShapes.push_back(aShapeInCompound);
      }
    }

    std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
    std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(
      new GeomAlgoAPI_Boolean(aUsedInOperationShapes,
                              aTools,
                              GeomAlgoAPI_Boolean::BOOL_CUT));

    // Checking that the algorithm worked properly.
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aCutAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aMakeShapeList->appendAlgo(aCutAlgo);
    GeomShapePtr aResultShape = aCutAlgo->shape();

    // Add result to not used shape from compound.
    if (!aNotUsedShapes.empty()) {
      ListOfShape aShapesForResult = aNotUsedShapes;
      if (aResultShape->shapeType() == GeomAPI_Shape::COMPOUND) {
        for (GeomAPI_ShapeIterator aResultIt(aResultShape); aResultIt.more(); aResultIt.next()) {
          aShapesForResult.push_back(aResultIt.current());
        }
      } else {
        aShapesForResult.push_back(aResultShape);
      }

      if (aShapesForResult.size() == 1) {
        aResultShape = aShapesForResult.front();
      } else {
        aResultShape = GeomAlgoAPI_CompoundBuilder::compound(aShapesForResult);
      }
    }

    GeomAPI_ShapeIterator aShapeIt(aResultShape);
    if (aShapeIt.more() || aResultShape->shapeType() == GeomAPI_Shape::VERTEX) {
      std::shared_ptr<ModelAPI_ResultBody> aResultBody =
        document()->createBody(data(), aResultIndex);

      ListOfShape anObjectList;
      anObjectList.push_back(aCompound);
      FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                               anObjectList,
                                               aTools,
                                               aMakeShapeList,
                                               aResultShape);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;

      FeaturesPlugin_Tools::ResultBaseAlgo aRBA;
      aRBA.resultBody = aResultBody;
      aRBA.baseShape = aCompound;
      aRBA.makeShape = aMakeShapeList;
      aResultBaseAlgoList.push_back(aRBA);
      aResultShapesList.push_back(aResultShape);
    }
  }

  // Store deleted shapes after all results has been proceeded. This is to avoid issue when in one
  // result shape has been deleted, but in another it was modified or stayed.
  GeomShapePtr aResultShapesCompound = GeomAlgoAPI_CompoundBuilder::compound(aResultShapesList);
  FeaturesPlugin_Tools::loadDeletedShapes(aResultBaseAlgoList, aTools, aResultShapesCompound);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
