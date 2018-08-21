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

#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>
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

  GeomAlgoAPI_MakeShapeList aMakeShapeList;
  GeomAPI_DataMapOfShapeShape aMapOfShapes;
  if (!aShapesToAdd.empty()) {
    // Cut objects with not used solids.
    std::shared_ptr<GeomAlgoAPI_Boolean> anObjectsCutAlgo(
      new GeomAlgoAPI_Boolean(aShapesToSmash,
                              aShapesToAdd,
                              GeomAlgoAPI_Boolean::BOOL_CUT));

    if (GeomAlgoAPI_ShapeTools::volume(anObjectsCutAlgo->shape()) > 1.e-27) {
      aShapesToSmash.clear();
      aShapesToSmash.push_back(anObjectsCutAlgo->shape());
      aMakeShapeList.appendAlgo(anObjectsCutAlgo);
      aMapOfShapes.merge(anObjectsCutAlgo->mapOfSubShapes());
    }

    // Cut tools with not used solids.
    std::shared_ptr<GeomAlgoAPI_Boolean> aToolsCutAlgo(
      new GeomAlgoAPI_Boolean(aTools,
                              aShapesToAdd,
                              GeomAlgoAPI_Boolean::BOOL_CUT));

    if (GeomAlgoAPI_ShapeTools::volume(aToolsCutAlgo->shape()) > 1.e-27) {
      aTools.clear();
      aTools.push_back(aToolsCutAlgo->shape());
      aMakeShapeList.appendAlgo(aToolsCutAlgo);
      aMapOfShapes.merge(aToolsCutAlgo->mapOfSubShapes());
    }
  }

  // Cut objects with tools.
  std::shared_ptr<GeomAlgoAPI_Boolean> aBoolAlgo(
    new GeomAlgoAPI_Boolean(aShapesToSmash,
                            aTools,
                            GeomAlgoAPI_Boolean::BOOL_CUT));

  // Checking that the algorithm worked properly.
  if (!aBoolAlgo->isDone()) {
    static const std::string aFeatureError = "Error: Boolean algorithm failed.";
    setError(aFeatureError);
    return;
  }
  if (aBoolAlgo->shape()->isNull()) {
    static const std::string aShapeError = "Error: Resulting shape is Null.";
    setError(aShapeError);
    return;
  }
  if (!aBoolAlgo->isValid()) {
    std::string aFeatureError = "Error: Resulting shape is not valid.";
    setError(aFeatureError);
    return;
  }
  aMakeShapeList.appendAlgo(aBoolAlgo);
  aMapOfShapes.merge(aBoolAlgo->mapOfSubShapes());

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
    if (!aFillerAlgo->isDone()) {
      std::string aFeatureError = "Error: PaveFiller algorithm failed.";
      setError(aFeatureError);
      return;
    }
    if (aFillerAlgo->shape()->isNull()) {
      static const std::string aShapeError = "Error: Resulting shape is Null.";
      setError(aShapeError);
      return;
    }
    if (!aFillerAlgo->isValid()) {
      std::string aFeatureError = "Error: Resulting shape is not valid.";
      setError(aFeatureError);
      return;
    }

    aShape = aFillerAlgo->shape();
    aMakeShapeList.appendAlgo(aFillerAlgo);
    aMapOfShapes.merge(aFillerAlgo->mapOfSubShapes());
  }

  std::shared_ptr<GeomAPI_Shape> aFrontShape = anOriginalShapes.front();
  anOriginalShapes.pop_front();
  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
  loadNamingDS(aResultBody,
               aFrontShape,
               anOriginalShapes,
               aShape,
               aMakeShapeList,
               aMapOfShapes);

  setResult(aResultBody, aResultIndex);
  aResultIndex++;

  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//==================================================================================================
void FeaturesPlugin_BooleanSmash::loadNamingDS(ResultBodyPtr theResultBody,
                                             const GeomShapePtr theBaseShape,
                                             const ListOfShape& theTools,
                                             const GeomShapePtr theResultShape,
                                             GeomAlgoAPI_MakeShape& theMakeShape,
                                             GeomAPI_DataMapOfShapeShape& theMapOfShapes)
{
  //load result
  if (theBaseShape->isEqual(theResultShape)) {
    theResultBody->store(theResultShape, false);
  } else if (theResultShape->isEqual(theTools.front())) {
    theResultBody->store(theResultShape, false);
  } else {
    const int aModifyVTag = 1;
    const int aModifyETag = 2;
    const int aModifyFTag = 3;
    const int aDeletedTag = 4;
    /// sub solids will be placed at labels 5, 6, etc. if result is compound of solids
    const int aSubsolidsTag = 5;

    theResultBody->storeModified(theBaseShape, theResultShape, aSubsolidsTag);

    const std::string aModVName = "Modified_Vertex";
    const std::string aModEName = "Modified_Edge";
    const std::string aModFName = "Modified_Face";

    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::VERTEX,
                                               aModifyVTag, aModVName, theMapOfShapes, false,
                                               true, true);
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::EDGE,
                                               aModifyETag, aModEName, theMapOfShapes, false,
                                               true, true);
    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE,
                                               aModifyFTag, aModFName, theMapOfShapes, false,
                                               true, true);

    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape,
                                     GeomAPI_Shape::VERTEX, aDeletedTag);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape,
                                     GeomAPI_Shape::EDGE, aDeletedTag);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape,
                                     GeomAPI_Shape::FACE, aDeletedTag);

    for (ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++)
    {
      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::VERTEX,
                                                 aModifyVTag, aModVName, theMapOfShapes, false,
                                                 true, true);

      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::EDGE,
                                                 aModifyETag, aModEName, theMapOfShapes, false,
                                                 true, true);

      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE,
                                                 aModifyFTag, aModFName, theMapOfShapes, false,
                                                 true, true);

      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::VERTEX, aDeletedTag);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::EDGE, aDeletedTag);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE, aDeletedTag);
    }
  }
}
