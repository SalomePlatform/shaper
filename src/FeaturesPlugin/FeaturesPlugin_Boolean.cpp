// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Boolean.cpp
// Created:     02 Sept 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Boolean.h"

#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Tools.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_ShapeExplorer.h>

#include <algorithm>

//=================================================================================================
FeaturesPlugin_Boolean::FeaturesPlugin_Boolean()
{
}

//=================================================================================================
void FeaturesPlugin_Boolean::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Boolean::TYPE_ID(), ModelAPI_AttributeInteger::typeId());

  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Boolean::OBJECT_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("SOLID");

  aSelection = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Boolean::TOOL_LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("SOLID");

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), OBJECT_LIST_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TOOL_LIST_ID());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> FeaturesPlugin_Boolean::getShape(const std::string& theAttrName)
{
  std::shared_ptr<ModelAPI_AttributeReference> aObjRef = std::dynamic_pointer_cast<
      ModelAPI_AttributeReference>(data()->attribute(theAttrName));
  if (aObjRef) {
    std::shared_ptr<ModelAPI_ResultBody> aConstr = std::dynamic_pointer_cast<
        ModelAPI_ResultBody>(aObjRef->value());
    if (aConstr)
      return aConstr->shape();
  }
  return std::shared_ptr<GeomAPI_Shape>();
}

//=================================================================================================
void FeaturesPlugin_Boolean::execute()
{
  // Getting operation type.
  std::shared_ptr<ModelAPI_AttributeInteger> aTypeAttr = std::dynamic_pointer_cast<
      ModelAPI_AttributeInteger>(data()->attribute(FeaturesPlugin_Boolean::TYPE_ID()));
  if (!aTypeAttr)
    return;
  OperationType aType = (FeaturesPlugin_Boolean::OperationType)aTypeAttr->value();

  ListOfShape anObjects, aTools, anEdgesAndFaces;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompSolidsObjects;

  // Getting objects.
  AttributeSelectionListPtr anObjectsSelList = selectionList(FeaturesPlugin_Boolean::OBJECT_LIST_ID());
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    AttributeSelectionPtr anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    ResultPtr aContext = anObjectAttr->context();
    ResultCompSolidPtr aResCompSolidPtr = ModelAPI_Tools::compSolidOwner(aContext);
    if(aResCompSolidPtr.get()) {
      std::shared_ptr<GeomAPI_Shape> aContextShape = aResCompSolidPtr->shape();
      std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator anIt = aCompSolidsObjects.begin();
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
      if(anObject->shapeType() == GeomAPI_Shape::EDGE ||
         anObject->shapeType() == GeomAPI_Shape::FACE) {
        anEdgesAndFaces.push_back(anObject);
      } else {
        anObjects.push_back(anObject);
      }
    }
  }

  // Getting tools.
  AttributeSelectionListPtr aToolsSelList = selectionList(FeaturesPlugin_Boolean::TOOL_LIST_ID());
  for(int aToolsIndex = 0; aToolsIndex < aToolsSelList->size(); aToolsIndex++) {
    AttributeSelectionPtr aToolAttr = aToolsSelList->value(aToolsIndex);
    std::shared_ptr<GeomAPI_Shape> aTool = aToolAttr->value();
    if(!aTool.get()) {
      return;
    }
    if(aTool->shapeType() == GeomAPI_Shape::EDGE ||
       aTool->shapeType() == GeomAPI_Shape::FACE) {
      anEdgesAndFaces.push_back(aTool);
    } else {
      aTools.push_back(aTool);
    }
  }

  int aResultIndex = 0;

  switch(aType) {
    case BOOL_CUT:
    case BOOL_COMMON:{
      if((anObjects.empty() && aCompSolidsObjects.empty()) || aTools.empty()) {
        std::string aFeatureError = "Error: Not enough objects for boolean operation.";
        setError(aFeatureError);
        return;
      }

      // For solids cut each object with all tools.
      for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
        std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
        ListOfShape aListWithObject;
        aListWithObject.push_back(anObject);
        GeomAlgoAPI_Boolean aBoolAlgo(aListWithObject, aTools, (GeomAlgoAPI_Boolean::OperationType)aType);

        // Checking that the algorithm worked properly.
        if(!aBoolAlgo.isDone()) {
          static const std::string aFeatureError = "Error: Boolean algorithm failed.";
          setError(aFeatureError);
          return;
        }
        if(aBoolAlgo.shape()->isNull()) {
          static const std::string aShapeError = "Error: Resulting shape is Null.";
          setError(aShapeError);
          return;
        }
        if(!aBoolAlgo.isValid()) {
          std::string aFeatureError = "Error: Resulting shape is not valid.";
          setError(aFeatureError);
          return;
        }

        if(GeomAlgoAPI_ShapeTools::volume(aBoolAlgo.shape()) > 1.e-7) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          loadNamingDS(aResultBody, anObject, aTools, aBoolAlgo.shape(), aBoolAlgo, *aBoolAlgo.mapOfSubShapes().get());
          setResult(aResultBody, aResultIndex);
          aResultIndex++;
        }
      }

      // Compsolids handling
      for(std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator anIt = aCompSolidsObjects.begin();
        anIt != aCompSolidsObjects.end(); anIt++) {
        std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
        ListOfShape& aUsedInOperationSolids = anIt->second;

        // Collecting solids from compsolids which will not be modified in boolean operation.
        ListOfShape aNotUsedSolids;
        for(GeomAPI_ShapeExplorer anExp(aCompSolid, GeomAPI_Shape::SOLID); anExp.more(); anExp.next()) {
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

        std::shared_ptr<GeomAlgoAPI_Boolean> aBoolAlgo(new GeomAlgoAPI_Boolean(aUsedInOperationSolids,
                                                                               aTools,
                                                                               (GeomAlgoAPI_Boolean::OperationType)aType));

        // Checking that the algorithm worked properly.
        if(!aBoolAlgo->isDone()) {
          static const std::string aFeatureError = "Error: Boolean algorithm failed.";
          setError(aFeatureError);
          return;
        }
        if(aBoolAlgo->shape()->isNull()) {
          static const std::string aShapeError = "Error: Resulting shape is Null.";
          setError(aShapeError);
          return;
        }
        if(!aBoolAlgo->isValid()) {
          std::string aFeatureError = "Error: Resulting shape is not valid.";
          setError(aFeatureError);
          return;
        }

        GeomAlgoAPI_MakeShapeList aMakeShapeList;
        aMakeShapeList.appendAlgo(aBoolAlgo);
        GeomAPI_DataMapOfShapeShape aMapOfShapes;
        aMapOfShapes.merge(aBoolAlgo->mapOfSubShapes());

        // Add result to not used solids from compsolid.
        ListOfShape aShapesToAdd = aNotUsedSolids;
        aShapesToAdd.push_back(aBoolAlgo->shape());
        std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
        if(!aFillerAlgo->isDone()) {
          std::string aFeatureError = "Error: PaveFiller algorithm failed.";
          setError(aFeatureError);
          return;
        }

        aMakeShapeList.appendAlgo(aFillerAlgo);
        aMapOfShapes.merge(aFillerAlgo->mapOfSubShapes());

        if(GeomAlgoAPI_ShapeTools::volume(aFillerAlgo->shape()) > 1.e-7) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          loadNamingDS(aResultBody, aCompSolid, aTools, aFillerAlgo->shape(), aMakeShapeList, aMapOfShapes);
          setResult(aResultBody, aResultIndex);
          aResultIndex++;
        }
      }
      break;
    }
    case BOOL_FUSE: {
      if((anObjects.size() + aTools.size() + aCompSolidsObjects.size() + anEdgesAndFaces.size()) < 2) {
        std::string aFeatureError = "Error: Not enough objects for boolean operation.";
        setError(aFeatureError);
        return;
      }

      // Collecting all solids which will be fused.
      ListOfShape aSolidsToFuse;
      aSolidsToFuse.insert(aSolidsToFuse.end(), anObjects.begin(), anObjects.end());
      aSolidsToFuse.insert(aSolidsToFuse.end(), aTools.begin(), aTools.end());

      // Collecting solids from compsolids which will not be modified in boolean operation and will be added to result.
      ListOfShape aShapesToAdd;
      for(std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator anIt = aCompSolidsObjects.begin();
        anIt != aCompSolidsObjects.end(); anIt++) {
        std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
        ListOfShape& aUsedInOperationSolids = anIt->second;
        aSolidsToFuse.insert(aSolidsToFuse.end(), aUsedInOperationSolids.begin(), aUsedInOperationSolids.end());

        // Collect solids from compsolid which will not be modified in boolean operation.
        for(GeomAPI_ShapeExplorer anExp(aCompSolid, GeomAPI_Shape::SOLID); anExp.more(); anExp.next()) {
          std::shared_ptr<GeomAPI_Shape> aSolidInCompSolid = anExp.current();
          ListOfShape::iterator anIt = aUsedInOperationSolids.begin();
          for(; anIt != aUsedInOperationSolids.end(); anIt++) {
            if(aSolidInCompSolid->isEqual(*anIt)) {
              break;
            }
          }
          if(anIt == aUsedInOperationSolids.end()) {
            aShapesToAdd.push_back(aSolidInCompSolid);
          }
        }
      }

      ListOfShape anOriginalShapes = aSolidsToFuse;
      anOriginalShapes.insert(anOriginalShapes.end(), aShapesToAdd.begin(), aShapesToAdd.end());

      // Cut edges and faces(if we have any) with solids.
      GeomAlgoAPI_MakeShapeList aMakeShapeList;
      GeomAPI_DataMapOfShapeShape aMapOfShapes;
      std::shared_ptr<GeomAPI_Shape> aCuttedEdgesAndFaces;
      if(!anEdgesAndFaces.empty()) {
        std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(new GeomAlgoAPI_Boolean(anEdgesAndFaces, anOriginalShapes, GeomAlgoAPI_Boolean::BOOL_CUT));
        if(aCutAlgo->isDone()) {
          aCuttedEdgesAndFaces = aCutAlgo->shape();
          aMakeShapeList.appendAlgo(aCutAlgo);
          aMapOfShapes.merge(aCutAlgo->mapOfSubShapes());
        }
      }
      anOriginalShapes.insert(anOriginalShapes.end(), anEdgesAndFaces.begin(), anEdgesAndFaces.end());

      // If we have compsolids then cut with not used solids all others.
      if(!aShapesToAdd.empty()) {
        aSolidsToFuse.clear();
        for(ListOfShape::iterator anIt = anOriginalShapes.begin(); anIt != anOriginalShapes.end(); anIt++) {
          ListOfShape aOneObjectList;
          aOneObjectList.push_back(*anIt);
          std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(new GeomAlgoAPI_Boolean(aOneObjectList, aShapesToAdd, GeomAlgoAPI_Boolean::BOOL_CUT));

          if(GeomAlgoAPI_ShapeTools::volume(aCutAlgo->shape()) > 1.e-7) {
            aSolidsToFuse.push_back(aCutAlgo->shape());
            aMakeShapeList.appendAlgo(aCutAlgo);
            aMapOfShapes.merge(aCutAlgo->mapOfSubShapes());
          }
        }
      }

      if(!aSolidsToFuse.empty()) {
        anObjects.clear();
        anObjects.push_back(aSolidsToFuse.back());
        aSolidsToFuse.pop_back();
        aTools = aSolidsToFuse;
      }

      // Fuse all objects and all tools.
      std::shared_ptr<GeomAPI_Shape> aShape;
      if(anObjects.size() == 1 && aTools.empty()) {
        aShape = anObjects.front();
      } else if(anObjects.empty() && aTools.size() == 1) {
        aShape = aTools.front();
      } else if((anObjects.size() + aTools.size()) > 1){
        std::shared_ptr<GeomAlgoAPI_Boolean> aFuseAlgo(new GeomAlgoAPI_Boolean(anObjects,
                                                                               aTools,
                                                                               (GeomAlgoAPI_Boolean::OperationType)aType));

        // Checking that the algorithm worked properly.
        if(!aFuseAlgo->isDone()) {
          static const std::string aFeatureError = "Error: Boolean algorithm failed.";
          setError(aFeatureError);
          return;
        }
        if(aFuseAlgo->shape()->isNull()) {
          static const std::string aShapeError = "Error: Resulting shape is Null.";
          setError(aShapeError);
          return;
        }
        if(!aFuseAlgo->isValid()) {
          std::string aFeatureError = "Error: Resulting shape is not valid.";
          setError(aFeatureError);
          return;
        }

        aShape = aFuseAlgo->shape();
        aMakeShapeList.appendAlgo(aFuseAlgo);
        aMapOfShapes.merge(aFuseAlgo->mapOfSubShapes());
      }

      // Combine result with not used solids from compsolid and edges and faces (if we have any).
      if(aCuttedEdgesAndFaces.get() && !aCuttedEdgesAndFaces->isNull()) {
        aShapesToAdd.push_back(aCuttedEdgesAndFaces);
      } else {
        aShapesToAdd.insert(aShapesToAdd.end(), anEdgesAndFaces.begin(), anEdgesAndFaces.end());
      }
      if(!aShapesToAdd.empty()) {
        if(aShape.get()) {
          aShapesToAdd.push_back(aShape);
        }
        std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
        if(!aFillerAlgo->isDone()) {
          std::string aFeatureError = "Error: PaveFiller algorithm failed.";
          setError(aFeatureError);
          return;
        }
        if(aFillerAlgo->shape()->isNull()) {
          static const std::string aShapeError = "Error: Resulting shape is Null.";
          setError(aShapeError);
          return;
        }
        if(!aFillerAlgo->isValid()) {
          std::string aFeatureError = "Error: Resulting shape is not valid.";
          setError(aFeatureError);
          return;
        }

        aShape = aFillerAlgo->shape();
        aMakeShapeList.appendAlgo(aFillerAlgo);
        aMapOfShapes.merge(aFillerAlgo->mapOfSubShapes());
      }

      std::shared_ptr<GeomAPI_Shape> aBackShape = anOriginalShapes.back();
      anOriginalShapes.pop_back();
      std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aResultBody, aBackShape, anOriginalShapes, aShape, aMakeShapeList, aMapOfShapes);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;
      break;
    }
    case BOOL_SMASH: {
      if((anObjects.empty() && aCompSolidsObjects.empty()) || aTools.empty()) {
        std::string aFeatureError = "Error: Not enough objects for boolean operation.";
        setError(aFeatureError);
        return;
      }

      // List of original solids for naming.
      ListOfShape anOriginalShapes;
      anOriginalShapes.insert(anOriginalShapes.end(), anObjects.begin(), anObjects.end());
      anOriginalShapes.insert(anOriginalShapes.end(), aTools.begin(), aTools.end());

      // Collecting all solids which will be smashed.
      ListOfShape aShapesToSmash;
      aShapesToSmash.insert(aShapesToSmash.end(), anObjects.begin(), anObjects.end());

      // Collecting solids from compsolids which will not be modified in boolean operation and will be added to result.
      ListOfShape aShapesToAdd;
      for(std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape>::iterator anIt = aCompSolidsObjects.begin();
        anIt != aCompSolidsObjects.end(); anIt++) {
        std::shared_ptr<GeomAPI_Shape> aCompSolid = anIt->first;
        ListOfShape& aUsedInOperationSolids = anIt->second;
        anOriginalShapes.push_back(aCompSolid);
        aShapesToSmash.insert(aShapesToSmash.end(), aUsedInOperationSolids.begin(), aUsedInOperationSolids.end());

        // Collect solids from compsolid which will not be modified in boolean operation.
        for(GeomAPI_ShapeExplorer anExp(aCompSolid, GeomAPI_Shape::SOLID); anExp.more(); anExp.next()) {
          std::shared_ptr<GeomAPI_Shape> aSolidInCompSolid = anExp.current();
          ListOfShape::iterator anIt = aUsedInOperationSolids.begin();
          for(; anIt != aUsedInOperationSolids.end(); anIt++) {
            if(aSolidInCompSolid->isEqual(*anIt)) {
              break;
            }
          }
          if(anIt == aUsedInOperationSolids.end()) {
            aShapesToAdd.push_back(aSolidInCompSolid);
          }
        }
      }

      GeomAlgoAPI_MakeShapeList aMakeShapeList;
      GeomAPI_DataMapOfShapeShape aMapOfShapes;
      if(!aShapesToAdd.empty()) {
        // Cut objects with not used solids.
        std::shared_ptr<GeomAlgoAPI_Boolean> anObjectsCutAlgo(new GeomAlgoAPI_Boolean(aShapesToSmash,
                                                                                      aShapesToAdd,
                                                                                      GeomAlgoAPI_Boolean::BOOL_CUT));

        if(GeomAlgoAPI_ShapeTools::volume(anObjectsCutAlgo->shape()) > 1.e-7) {
          aShapesToSmash.clear();
          aShapesToSmash.push_back(anObjectsCutAlgo->shape());
          aMakeShapeList.appendAlgo(anObjectsCutAlgo);
          aMapOfShapes.merge(anObjectsCutAlgo->mapOfSubShapes());
        }

        // Cut tools with not used solids.
        std::shared_ptr<GeomAlgoAPI_Boolean> aToolsCutAlgo(new GeomAlgoAPI_Boolean(aTools,
                                                                                   aShapesToAdd,
                                                                                   GeomAlgoAPI_Boolean::BOOL_CUT));

        if(GeomAlgoAPI_ShapeTools::volume(aToolsCutAlgo->shape()) > 1.e-7) {
          aTools.clear();
          aTools.push_back(aToolsCutAlgo->shape());
          aMakeShapeList.appendAlgo(aToolsCutAlgo);
          aMapOfShapes.merge(aToolsCutAlgo->mapOfSubShapes());
        }
      }

      // Cut objects with tools.
      std::shared_ptr<GeomAlgoAPI_Boolean> aBoolAlgo(new GeomAlgoAPI_Boolean(aShapesToSmash,
                                                                             aTools,
                                                                             GeomAlgoAPI_Boolean::BOOL_CUT));

      // Checking that the algorithm worked properly.
      if(!aBoolAlgo->isDone()) {
        static const std::string aFeatureError = "Error: Boolean algorithm failed.";
        setError(aFeatureError);
        return;
      }
      if(aBoolAlgo->shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        return;
      }
      if(!aBoolAlgo->isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        return;
      }
      aMakeShapeList.appendAlgo(aBoolAlgo);
      aMapOfShapes.merge(aBoolAlgo->mapOfSubShapes());

      // Put all (cut result, tools and not used solids) to PaveFiller.
      aShapesToAdd.push_back(aBoolAlgo->shape());
      aShapesToAdd.insert(aShapesToAdd.end(), aTools.begin(), aTools.end());

      std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(new GeomAlgoAPI_PaveFiller(aShapesToAdd, true));
      if(!aFillerAlgo->isDone()) {
        std::string aFeatureError = "Error: PaveFiller algorithm failed.";
        setError(aFeatureError);
        return;
      }
      if(aFillerAlgo->shape()->isNull()) {
        static const std::string aShapeError = "Error: Resulting shape is Null.";
        setError(aShapeError);
        return;
      }
      if(!aFillerAlgo->isValid()) {
        std::string aFeatureError = "Error: Resulting shape is not valid.";
        setError(aFeatureError);
        return;
      }

      std::shared_ptr<GeomAPI_Shape> aShape = aFillerAlgo->shape();
      aMakeShapeList.appendAlgo(aFillerAlgo);
      aMapOfShapes.merge(aFillerAlgo->mapOfSubShapes());

      std::shared_ptr<GeomAPI_Shape> aFrontShape = anOriginalShapes.front();
      anOriginalShapes.pop_front();
      std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aResultBody, aFrontShape, anOriginalShapes, aShape, aMakeShapeList, aMapOfShapes);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;

      break;
    }
    default: {
      std::string anOperationError = "Error: Wrong type of operation";
      setError(anOperationError);
      return;
    }
  }
  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Boolean::loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                          const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                          const ListOfShape& theTools,
                                          const std::shared_ptr<GeomAPI_Shape> theResultShape,
                                          GeomAlgoAPI_MakeShape& theMakeShape,
                                          GeomAPI_DataMapOfShapeShape& theMapOfShapes)
{
  //load result
  if(theBaseShape->isEqual(theResultShape)) {
    theResultBody->store(theResultShape);
  } else {
    const int aModifyTag = 1;
    const int aDeletedTag = 2;
    const int aSubsolidsTag = 3; /// sub solids will be placed at labels 3, 4, etc. if result is compound of solids
    const int anEdgesAndFacesTag = 10000;

    theResultBody->storeModified(theBaseShape, theResultShape, aSubsolidsTag);

    const std::string aModName = "Modified";
    const std::string aModEName = "Modified_Edge";
    const std::string aModFName = "Modified_Face";

    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE,
                                               aModifyTag, aModName, theMapOfShapes);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE, aDeletedTag);

    int aTag;
    std::string aName;
    for(ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++) {
      if((*anIter)->shapeType() == GeomAPI_Shape::EDGE) {
        aTag = anEdgesAndFacesTag;
        aName = aModEName;
      }
      else if((*anIter)->shapeType() == GeomAPI_Shape::FACE) {
        aTag = anEdgesAndFacesTag;
        aName = aModFName;
      } else {
        aTag = aModifyTag;
        aName = aModName;
      }
      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, aName == aModEName ? GeomAPI_Shape::EDGE : GeomAPI_Shape::FACE,
                                                 aTag, aName, theMapOfShapes);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE, aDeletedTag);
    }
  }
}
