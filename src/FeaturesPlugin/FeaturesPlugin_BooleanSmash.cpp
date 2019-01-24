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

#include "FeaturesPlugin_BooleanSmash.h"

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
FeaturesPlugin_BooleanSmash::FeaturesPlugin_BooleanSmash()
: FeaturesPlugin_Boolean(FeaturesPlugin_Boolean::BOOL_SMASH)
{
}

//==================================================================================================
void FeaturesPlugin_BooleanSmash::initAttributes()
{
  data()->addAttribute(OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
  data()->addAttribute(TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//==================================================================================================
void FeaturesPlugin_BooleanSmash::execute()
{
  std::string anError;
  ListOfShape anObjects, aTools;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompSolidsObjects;

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

      std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
        anIt = aCompSolidsObjects.begin();
      for (; anIt != aCompSolidsObjects.end(); anIt++) {
        if (anIt->first->isEqual(aContextShape)) {
          aCompSolidsObjects[anIt->first].push_back(anObject);
          break;
        }
      }
      if (anIt == aCompSolidsObjects.end()) {
        aCompSolidsObjects[aContextShape].push_back(anObject);
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

  if((anObjects.empty() && aCompSolidsObjects.empty())
     || aTools.empty()) {
    std::string aFeatureError = "Error: Not enough objects for boolean operation.";
    setError(aFeatureError);
    return;
  }

  // List of original shapes for naming.
  ListOfShape anOriginalShapes;
  anOriginalShapes.insert(anOriginalShapes.end(), anObjects.begin(), anObjects.end());
  anOriginalShapes.insert(anOriginalShapes.end(), aTools.begin(), aTools.end());

  // Collecting all shapes which will be smashed.
  ListOfShape aShapesToSmash;
  aShapesToSmash.insert(aShapesToSmash.end(), anObjects.begin(), anObjects.end());

  // Collecting solids from compsolids which will not be modified in
  // boolean operation and will be added to result.
  ListOfShape aShapesToAdd;
  for (std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator
       anIt = aCompSolidsObjects.begin();
       anIt != aCompSolidsObjects.end();
       ++anIt)
  {
    std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
    ListOfShape& aUsedInOperationSolids = anIt->second;
    anOriginalShapes.push_back(aCompSolid);
    aShapesToSmash.insert(aShapesToSmash.end(),
                          aUsedInOperationSolids.begin(),
                          aUsedInOperationSolids.end());

    // Collect solids from compsolid which will not be modified in boolean operation.
    for (GeomAPI_ShapeExplorer anExp(aCompSolid, GeomAPI_Shape::SOLID);
         anExp.more();
         anExp.next())
    {
      std::shared_ptr<GeomAPI_Shape> aSolidInCompSolid = anExp.current();
      ListOfShape::iterator anIt = aUsedInOperationSolids.begin();
      for (; anIt != aUsedInOperationSolids.end(); anIt++) {
        if (aSolidInCompSolid->isEqual(*anIt)) {
          break;
        }
      }
      if (anIt == aUsedInOperationSolids.end()) {
        aShapesToAdd.push_back(aSolidInCompSolid);
      }
    }
  }

  std::shared_ptr<GeomAlgoAPI_MakeShapeList> aMakeShapeList(new GeomAlgoAPI_MakeShapeList());
  if (!aShapesToAdd.empty()) {
    // Cut objects with not used solids.
    std::shared_ptr<GeomAlgoAPI_Boolean> anObjectsCutAlgo(
      new GeomAlgoAPI_Boolean(aShapesToSmash,
                              aShapesToAdd,
                              GeomAlgoAPI_Boolean::BOOL_CUT));

    if (GeomAlgoAPI_ShapeTools::volume(anObjectsCutAlgo->shape()) > 1.e-27) {
      aShapesToSmash.clear();
      aShapesToSmash.push_back(anObjectsCutAlgo->shape());
      aMakeShapeList->appendAlgo(anObjectsCutAlgo);
    }

    // Cut tools with not used solids.
    std::shared_ptr<GeomAlgoAPI_Boolean> aToolsCutAlgo(
      new GeomAlgoAPI_Boolean(aTools,
                              aShapesToAdd,
                              GeomAlgoAPI_Boolean::BOOL_CUT));

    if (GeomAlgoAPI_ShapeTools::volume(aToolsCutAlgo->shape()) > 1.e-27) {
      aTools.clear();
      aTools.push_back(aToolsCutAlgo->shape());
      aMakeShapeList->appendAlgo(aToolsCutAlgo);
    }
  }

  // Cut objects with tools.
  std::shared_ptr<GeomAlgoAPI_Boolean> aBoolAlgo(
    new GeomAlgoAPI_Boolean(aShapesToSmash,
                            aTools,
                            GeomAlgoAPI_Boolean::BOOL_CUT));

  // Checking that the algorithm worked properly.
  if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aBoolAlgo, getKind(), anError)) {
    setError(anError);
    return;
  }

  aMakeShapeList->appendAlgo(aBoolAlgo);

  // Put all (cut result, tools and not used solids) to PaveFiller.
  GeomShapePtr aShape = aBoolAlgo->shape();
  GeomAPI_ShapeIterator anIt(aShape);
  if (anIt.more() || aShape->shapeType() == GeomAPI_Shape::VERTEX) {
    aShapesToAdd.push_back(aShape);
  }
  aShapesToAdd.insert(aShapesToAdd.end(), aTools.begin(), aTools.end());

  if (aShapesToAdd.size() == 1) {
    aShape = aShapesToAdd.front();
  }
  else {
    std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(
      new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
    if (GeomAlgoAPI_Tools::AlgoError::isAlgorithmFailed(aFillerAlgo, getKind(), anError)) {
      setError(anError);
      return;
    }

    aShape = aFillerAlgo->shape();
    aMakeShapeList->appendAlgo(aFillerAlgo);
  }

  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);

  FeaturesPlugin_Tools::loadModifiedShapes(aResultBody,
                                           anOriginalShapes,
                                           anOriginalShapes,
                                           aMakeShapeList,
                                           aShape);

  setResult(aResultBody, aResultIndex);
  aResultIndex++;

  FeaturesPlugin_Tools::loadDeletedShapes(aResultBody,
                                          GeomShapePtr(),
                                          anOriginalShapes,
                                          aMakeShapeList,
                                          aShape);

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}
