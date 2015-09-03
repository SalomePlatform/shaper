// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_CompositeBoolean.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_CompositeBoolean.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_Revolution.h>
#include <GeomAlgoAPI_ShapeTools.h>

//=================================================================================================
void FeaturesPlugin_CompositeBoolean::initAttributes()
{
  data()->addAttribute(SKETCH_OBJECT_ID(), ModelAPI_AttributeReference::typeId());

  // Boolean works with solids always.
  data()->addAttribute(BOOLEAN_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
  AttributeSelectionListPtr aSelection = data()->selectionList(BOOLEAN_OBJECTS_ID());
  aSelection->setSelectionType("SOLID");

  initMakeSolidsAttributes();
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
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeBoolean::subFeature(const int theIndex, bool forTree) const
{
  if (theIndex == 0)
    return std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_OBJECT_ID())->value());
  return std::shared_ptr<ModelAPI_Feature>();
}

//=================================================================================================
int FeaturesPlugin_CompositeBoolean::subFeatureId(const int theIndex) const
{
  std::shared_ptr<ModelAPI_Feature> aFeature = subFeature(theIndex);
  if (aFeature.get())
    return aFeature->data()->featureId();
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
  ListOfShape aBooleanTools;
  std::list<std::shared_ptr<GeomAPI_Interface>> aSolidsAlgos;
  makeSolids(aShells, aBooleanTools, aSolidsAlgos);
  if(aBooleanTools.empty()) {
    return;
  }

  // Getting objects for boolean operation.
  ListOfShape aBooleanObjects;
  AttributeSelectionListPtr anObjectsSelList = selectionList(BOOLEAN_OBJECTS_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    aBooleanObjects.push_back(anObject);
  }

  // Cut from each object solids.
  int aResultIndex = 0;

  switch(myBooleanOperationType) {
    case GeomAlgoAPI_Boolean::BOOL_CUT:
    case GeomAlgoAPI_Boolean::BOOL_COMMON:{
      // Cut each object with all tools
      for(ListOfShape::iterator anObjectsIt = aBooleanObjects.begin(); anObjectsIt != aBooleanObjects.end(); anObjectsIt++) {
        std::shared_ptr<GeomAPI_Shape> anObject = *anObjectsIt;
        ListOfShape aListWithObject;
        aListWithObject.push_back(anObject);
        GeomAlgoAPI_Boolean aBoolAlgo(aListWithObject, aBooleanTools, myBooleanOperationType);

        // Checking that the algorithm worked properly.
        if(!aBoolAlgo.isDone() || aBoolAlgo.shape()->isNull() || !aBoolAlgo.isValid()) {
          setError("Boolean algorithm failed");
          return;
        }

        if(GeomAlgoAPI_ShapeTools::volume(aBoolAlgo.shape()) > 1.e-7) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          loadNamingDS(aResultBody, anObject, aShells, aSolidsAlgos, aBooleanTools, aBoolAlgo);
          setResult(aResultBody, aResultIndex);
          aResultIndex++;
        }
      }
      break;
    }
    case GeomAlgoAPI_Boolean::BOOL_FUSE: {
      // Fuse all objects and all tools.
      GeomAlgoAPI_Boolean aBoolAlgo(aBooleanObjects, aBooleanTools, myBooleanOperationType);

      // Checking that the algorithm worked properly.
      if(!aBoolAlgo.isDone() || aBoolAlgo.shape()->isNull() || !aBoolAlgo.isValid()) {
        setError("Boolean algorithm failed");
        return;
      }

      std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
      loadNamingDS(aResultBody, aBooleanObjects.front(), aShells, aSolidsAlgos, aBooleanTools, aBoolAlgo);
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
                                                   const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                                   const ListOfShape& theShells,
                                                   const std::list<std::shared_ptr<GeomAPI_Interface>>& theSolidsAlgos,
                                                   const ListOfShape& theTools,
                                                   const GeomAlgoAPI_Boolean& theAlgo)
{
  //load result
  if(theBaseShape->isEqual(theAlgo.shape())) {
    theResultBody->store(theAlgo.shape());
  } else {
    const int aGenTag = 1;
    const int aModTag = 2;
    const int aDelTag = 3;
    const int aSubsolidsTag=4; /// sub solids will be placed at labels 6, 7, etc. if result is compound of solids
    int aToTag = 5; // may be many labels, starting from this index
    int aFromTag = 10000; // may be many labels, starting from this index or last aToTag index
    const std::string aGenName = "Generated";
    const std::string aModName = "Modified";
    const std::string aLatName = "LateralFace";
    const std::string aFromName = "FromFace";
    const std::string aToName = "ToFace";

    theResultBody->storeModified(theBaseShape, theAlgo.shape(), aSubsolidsTag);

    ListOfShape::const_iterator aShellsIter = theShells.begin();
    std::list<std::shared_ptr<GeomAPI_Interface>>::const_iterator aSolidsAlgosIter = theSolidsAlgos.begin();
    for(; aShellsIter != theShells.end() && aSolidsAlgosIter != theSolidsAlgos.end(); aShellsIter++, aSolidsAlgosIter++) {
      ListOfShape aFromFaces;
      ListOfShape aToFaces;
      std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes;

      //Insert lateral face : Face from Edge
      if(std::dynamic_pointer_cast<GeomAlgoAPI_Prism>(*aSolidsAlgosIter)) {
        std::shared_ptr<GeomAlgoAPI_Prism> aPrismAlgo = std::dynamic_pointer_cast<GeomAlgoAPI_Prism>(*aSolidsAlgosIter);
        aSubShapes = aPrismAlgo->mapOfShapes();
        theResultBody->loadAndOrientGeneratedShapes(aPrismAlgo->makeShape().get(), *aShellsIter, GeomAPI_Shape::EDGE, aGenTag,
                                                    aLatName, *aSubShapes.get());

        aFromFaces = aPrismAlgo->fromFaces();
        aToFaces = aPrismAlgo->toFaces();
      } else if(std::dynamic_pointer_cast<GeomAlgoAPI_Revolution>(*aSolidsAlgosIter)) {
        std::shared_ptr<GeomAlgoAPI_Revolution> aRevolAlgo = std::dynamic_pointer_cast<GeomAlgoAPI_Revolution>(*aSolidsAlgosIter);
        aSubShapes = aRevolAlgo->mapOfShapes();
        theResultBody->loadAndOrientGeneratedShapes(aRevolAlgo->makeShape().get(), *aShellsIter, GeomAPI_Shape::EDGE, aGenTag,
                                                    aLatName, *aSubShapes.get());
        aFromFaces = aRevolAlgo->fromFaces();
        aToFaces = aRevolAlgo->toFaces();
      }

      //Insert to faces
      for(ListOfShape::const_iterator anIt = aToFaces.cbegin(); anIt != aToFaces.cend(); anIt++) {
        std::shared_ptr<GeomAPI_Shape> aToFace = *anIt;
        if(aSubShapes->isBound(aToFace)) {
          aToFace = aSubShapes->find(aToFace);
        }
        theResultBody->generated(aToFace, aToName, aToTag++);
      }

      //Insert from faces
      if (aFromTag < aToTag) aFromTag = aToTag;
      for(ListOfShape::const_iterator anIt = aFromFaces.cbegin(); anIt != aFromFaces.cend(); anIt++) {
        std::shared_ptr<GeomAPI_Shape> aFromFace = *anIt;
        if(aSubShapes->isBound(aFromFace)) {
          aFromFace = aSubShapes->find(aFromFace);
        }
        theResultBody->generated(aFromFace, aFromName, aFromTag++);
      }
    }

    theResultBody->loadAndOrientModifiedShapes(theAlgo.makeShape().get(), theBaseShape, GeomAPI_Shape::FACE,
                                               aModTag, aModName, *theAlgo.mapOfShapes().get());
    theResultBody->loadDeletedShapes(theAlgo.makeShape().get(), theBaseShape, GeomAPI_Shape::FACE, aDelTag);

    for(ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++) {
      theResultBody->loadAndOrientModifiedShapes(theAlgo.makeShape().get(), *anIter, GeomAPI_Shape::FACE,
                                                 aModTag, aModName, *theAlgo.mapOfShapes().get());
      theResultBody->loadDeletedShapes(theAlgo.makeShape().get(), *anIter, GeomAPI_Shape::FACE, aDelTag);
    }
  }
}
