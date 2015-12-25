// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_CompositeBoolean.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_CompositeBoolean.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_MakeShapeList.h>
#include <GeomAlgoAPI_MakeSweep.h>
#include <GeomAlgoAPI_PaveFiller.h>
#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_Revolution.h>
#include <GeomAlgoAPI_ShapeTools.h>
#include <GeomAPI_ShapeExplorer.h>

//=================================================================================================
void FeaturesPlugin_CompositeBoolean::initAttributes()
{
  data()->addAttribute(SKETCH_OBJECT_ID(), ModelAPI_AttributeReference::typeId());

  // Boolean works with solids always.
  data()->addAttribute(BOOLEAN_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  AttributeSelectionListPtr aSelection = data()->selectionList(BOOLEAN_OBJECTS_ID());
  aSelection->setSelectionType("SOLID");

  initMakeSolidsAttributes();

  data()->addAttribute(SKETCH_SELECTION_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SKETCH_SELECTION_ID());
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeBoolean::addFeature(std::string theID)
{
  std::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID, false);
  if (aNew) {
    data()->reference(SKETCH_OBJECT_ID())->setValue(aNew);
  }
  // set as current also after it becomes sub to set correctly enabled for other sketch subs
  document()->setCurrentFeature(aNew, false);
  return aNew;
}

//=================================================================================================
int FeaturesPlugin_CompositeBoolean::numberOfSubs(bool forTree) const
{
  ObjectPtr aObj = data()->reference(SKETCH_OBJECT_ID())->value();
  return aObj.get()? 1 : 0;
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeBoolean::subFeature(const int theIndex, bool forTree)
{
  if (theIndex == 0)
    return std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_OBJECT_ID())->value());
  return std::shared_ptr<ModelAPI_Feature>();
}

//=================================================================================================
int FeaturesPlugin_CompositeBoolean::subFeatureId(const int theIndex) const
{
  if (theIndex == 0) {
    FeaturePtr aFeature = 
      std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_OBJECT_ID())->value());
    if (aFeature.get())
      return aFeature->data()->featureId();
  }
  return -1;
}

//=================================================================================================
bool FeaturesPlugin_CompositeBoolean::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature)
    return false;
 
  ObjectPtr aSub = data()->reference(SKETCH_OBJECT_ID())->value();
  return aSub == theObject;
}

//=================================================================================================
void FeaturesPlugin_CompositeBoolean::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
}

//=================================================================================================
void FeaturesPlugin_CompositeBoolean::erase()
{
  if (data().get() && data()->isValid()) { // on abort of sketch of this composite it may be invalid
    FeaturePtr aSketch =
      std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_OBJECT_ID())->value());
    if (aSketch.get() && aSketch->data()->isValid()) {
      document()->removeFeature(aSketch);
    }
  }
  ModelAPI_CompositeFeature::erase();
}


//=================================================================================================
void FeaturesPlugin_CompositeBoolean::execute()
{
  // Getting faces to create solids.
  std::shared_ptr<ModelAPI_Feature> aSketchFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(
                                                     reference(SKETCH_OBJECT_ID())->value());
  if(!aSketchFeature || aSketchFeature->results().empty()) {
    return;
  }
  ResultPtr aSketchRes = aSketchFeature->results().front();
  ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aSketchRes);
  if(!aConstruction.get()) {
    return;
  }
  selection(SKETCH_SELECTION_ID())->setValue(aSketchRes, std::shared_ptr<GeomAPI_Shape>());
  int aSketchFacesNum = aConstruction->facesNum();
  if(aSketchFacesNum == 0) {
    return;
  }
  ListOfShape aFacesList;
  for(int aFaceIndex = 0; aFaceIndex < aSketchFacesNum; aFaceIndex++) {
    std::shared_ptr<GeomAPI_Shape> aFace = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
    aFacesList.push_back(aFace);
  }

  // Searching faces with common edges.
  ListOfShape aShells;
  ListOfShape aFreeFaces;
  std::shared_ptr<GeomAPI_Shape> aFacesCompound = GeomAlgoAPI_CompoundBuilder::compound(aFacesList);
  GeomAlgoAPI_ShapeTools::combineShapes(aFacesCompound, GeomAPI_Shape::SHELL, aShells, aFreeFaces);
  aShells.insert(aShells.end(), aFreeFaces.begin(), aFreeFaces.end());

  // Pass shells/faces to solids creation function.
  ListOfShape aTools;
  ListOfMakeShape aSolidsAlgos;
  makeSolids(aShells, aTools, aSolidsAlgos);
  if(aTools.empty()) {
    return;
  }

  // Getting objects for boolean operation.
  ListOfShape anObjects;
  std::map<std::shared_ptr<GeomAPI_Shape>, ListOfShape> aCompSolidsObjects;
  AttributeSelectionListPtr anObjectsSelList = selectionList(BOOLEAN_OBJECTS_ID());
  if(anObjectsSelList->size() == 0) {
    return;
  }
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
      anObjects.push_back(anObject);
    }
  }

  // Cut from each object solids.
  int aResultIndex = 0;

  switch(myBooleanOperationType) {
    case GeomAlgoAPI_Boolean::BOOL_CUT:
    case GeomAlgoAPI_Boolean::BOOL_COMMON:{
      // Cut each object with all tools
      for(ListOfShape::iterator anObjectsIt = anObjects.begin(); anObjectsIt != anObjects.end(); anObjectsIt++) {
        std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
        ListOfShape aListWithObject;
        aListWithObject.push_back(anObject);
        GeomAlgoAPI_Boolean aBoolAlgo(aListWithObject, aTools, myBooleanOperationType);

        // Checking that the algorithm worked properly.
        if(!aBoolAlgo.isDone() || aBoolAlgo.shape()->isNull() || !aBoolAlgo.isValid()) {
          setError("Boolean algorithm failed");
          return;
        }

        if(GeomAlgoAPI_ShapeTools::volume(aBoolAlgo.shape()) > 1.e-7) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          loadNamingDS(aResultBody, aShells, aSolidsAlgos, anObject, aTools, aBoolAlgo.shape(),
                       aBoolAlgo, *aBoolAlgo.mapOfSubShapes().get());
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

        std::shared_ptr<GeomAlgoAPI_Boolean> aBoolAlgo(new GeomAlgoAPI_Boolean(aUsedInOperationSolids, aTools, myBooleanOperationType));

        // Checking that the algorithm worked properly.
        if(!aBoolAlgo->isDone() || aBoolAlgo->shape()->isNull() || !aBoolAlgo->isValid()) {
          setError("Boolean algorithm failed");
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
          std::string aFeatureError = "PaveFiller algorithm failed";
          setError(aFeatureError);
          return;
        }

        aMakeShapeList.appendAlgo(aFillerAlgo);
        aMapOfShapes.merge(aFillerAlgo->mapOfSubShapes());

        if(GeomAlgoAPI_ShapeTools::volume(aFillerAlgo->shape()) > 1.e-7) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          loadNamingDS(aResultBody, aShells, aSolidsAlgos, aCompSolid, aTools, aFillerAlgo->shape(), aMakeShapeList, aMapOfShapes);
          setResult(aResultBody, aResultIndex);
          aResultIndex++;
        }
      }
      break;
    }
    case GeomAlgoAPI_Boolean::BOOL_FUSE: {
      // Collecting all solids which will be fused.
      ListOfShape aSolidsToFuse;
      aSolidsToFuse.insert(aSolidsToFuse.end(), anObjects.begin(), anObjects.end());
      aSolidsToFuse.insert(aSolidsToFuse.end(), aTools.begin(), aTools.end());

      // Collecting solids from compsolids which will not be modified in boolean operation.
      ListOfShape aNotUsedSolids;
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
            aNotUsedSolids.push_back(aSolidInCompSolid);
          }
        }
      }

      ListOfShape anOriginalSolids = aSolidsToFuse;
      anOriginalSolids.insert(anOriginalSolids.end(), aNotUsedSolids.begin(), aNotUsedSolids.end());
      GeomAlgoAPI_MakeShapeList aMakeShapeList;
      GeomAPI_DataMapOfShapeShape aMapOfShapes;

      // If we have compsolids then cut with not used solids all others.
      if(!aNotUsedSolids.empty()) {
        aSolidsToFuse.clear();
        for(ListOfShape::iterator anIt = anOriginalSolids.begin(); anIt != anOriginalSolids.end(); anIt++) {
          ListOfShape aOneObjectList;
          aOneObjectList.push_back(*anIt);
          std::shared_ptr<GeomAlgoAPI_Boolean> aCutAlgo(new GeomAlgoAPI_Boolean(aOneObjectList, aNotUsedSolids, GeomAlgoAPI_Boolean::BOOL_CUT));

          if(GeomAlgoAPI_ShapeTools::volume(aCutAlgo->shape()) > 1.e-7) {
            aSolidsToFuse.push_back(aCutAlgo->shape());
            aMakeShapeList.appendAlgo(aCutAlgo);
            aMapOfShapes.merge(aCutAlgo->mapOfSubShapes());
          }
        }
      }

      anObjects.clear();
      anObjects.push_back(aSolidsToFuse.back());
      aSolidsToFuse.pop_back();
      aTools = aSolidsToFuse;

      // Fuse all objects and all tools.
      std::shared_ptr<GeomAlgoAPI_Boolean> aFuseAlgo(new GeomAlgoAPI_Boolean(anObjects, aTools, myBooleanOperationType));

      // Checking that the algorithm worked properly.
      if(!aFuseAlgo->isDone() || aFuseAlgo->shape()->isNull() || !aFuseAlgo->isValid()) {
        static const std::string aFeatureError = "Boolean algorithm failed";
        setError(aFeatureError);
        return;
      }

      std::shared_ptr<GeomAPI_Shape> aShape = aFuseAlgo->shape();
      aMakeShapeList.appendAlgo(aFuseAlgo);
      aMapOfShapes.merge(aFuseAlgo->mapOfSubShapes());

      // Add result to not used solids from compsolid (if we have any).
      if(!aNotUsedSolids.empty()) {
        aNotUsedSolids.push_back(aShape);
        std::shared_ptr<GeomAlgoAPI_PaveFiller> aFillerAlgo(new GeomAlgoAPI_PaveFiller(aNotUsedSolids, true));
        if(!aFillerAlgo->isDone()) {
          std::string aFeatureError = "PaveFiller algorithm failed";
          setError(aFeatureError);
          return;
        }
        if(aFillerAlgo->shape()->isNull()) {
          static const std::string aShapeError = "Resulting shape is Null";
          setError(aShapeError);
          return;
        }
        if(!aFillerAlgo->isValid()) {
          std::string aFeatureError = "Warning: resulting shape is not valid";
          setError(aFeatureError);
          return;
        }

        aShape = aFillerAlgo->shape();
        aMakeShapeList.appendAlgo(aFillerAlgo);
        aMapOfShapes.merge(aFillerAlgo->mapOfSubShapes());
      }

      std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aResultBody, aShells, aSolidsAlgos, anOriginalSolids.front(), anOriginalSolids, aShape, aMakeShapeList, aMapOfShapes);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;
      break;
    }
    default: {
      setError("Error: wrong type of boolean operation");
      return;
    }
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_CompositeBoolean::loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                                   const ListOfShape& theShells,
                                                   ListOfMakeShape& theSolidsAlgos,
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
    const int aGenTag = 1;
    const int aModTag = 2;
    const int aDelTag = 3;
    const int aSubsolidsTag=4; /// sub solids will be placed at labels 6, 7, etc. if result is compound of solids
    int aToTag = 5000; // may be many labels, starting from this index
    int aFromTag = 10000; // may be many labels, starting from this index or last aToTag index
    const std::string aGenName = "Generated";
    const std::string aModName = "Modified";
    const std::string aLatName = "LateralFace";
    const std::string aFromName = "FromFace";
    const std::string aToName = "ToFace";

    theResultBody->storeModified(theBaseShape, theResultShape, aSubsolidsTag);

    ListOfShape::const_iterator aShellsIter = theShells.begin();
    ListOfMakeShape::const_iterator aSolidsAlgosIter = theSolidsAlgos.begin();
    for(; aShellsIter != theShells.end() && aSolidsAlgosIter != theSolidsAlgos.end(); aShellsIter++, aSolidsAlgosIter++) {
      //Insert lateral face : Face from Edge
      std::shared_ptr<GeomAlgoAPI_MakeShape> aSolidAlgo = std::dynamic_pointer_cast<GeomAlgoAPI_MakeShape>(*aSolidsAlgosIter);
      if(aSolidAlgo.get()) {
        std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = aSolidAlgo->mapOfSubShapes();
        theResultBody->loadAndOrientGeneratedShapes(aSolidAlgo.get(), *aShellsIter, GeomAPI_Shape::EDGE, aGenTag,
                                                    aLatName, *aSubShapes.get());


        std::shared_ptr<GeomAlgoAPI_MakeSweep> aSweepAlgo = std::dynamic_pointer_cast<GeomAlgoAPI_MakeSweep>(aSolidAlgo);
        if(aSweepAlgo.get()) {
          //Insert to faces
          const ListOfShape& aToFaces = aSweepAlgo->toFaces();
          for(ListOfShape::const_iterator anIt = aToFaces.cbegin(); anIt != aToFaces.cend(); anIt++) {
            std::shared_ptr<GeomAPI_Shape> aToFace = *anIt;
            if(aSubShapes->isBound(aToFace)) {
              aToFace = aSubShapes->find(aToFace);
            }
            theResultBody->generated(aToFace, aToName, aToTag++);
          }

          //Insert from faces
          const ListOfShape& aFromFaces = aSweepAlgo->fromFaces();
          if (aFromTag < aToTag) aFromTag = aToTag;
          for(ListOfShape::const_iterator anIt = aFromFaces.cbegin(); anIt != aFromFaces.cend(); anIt++) {
            std::shared_ptr<GeomAPI_Shape> aFromFace = *anIt;
            if(aSubShapes->isBound(aFromFace)) {
              aFromFace = aSubShapes->find(aFromFace);
            }
            theResultBody->generated(aFromFace, aFromName, aFromTag++);
          }
        }
      }
    }

    theResultBody->loadAndOrientModifiedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE,
                                               aModTag, aModName, theMapOfShapes);
    theResultBody->loadDeletedShapes(&theMakeShape, theBaseShape, GeomAPI_Shape::FACE, aDelTag);

    for(ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++) {
      theResultBody->loadAndOrientModifiedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE,
                                                 aModTag, aModName, theMapOfShapes);
      theResultBody->loadDeletedShapes(&theMakeShape, *anIter, GeomAPI_Shape::FACE, aDelTag);
    }
  }
}
