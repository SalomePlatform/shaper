// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_CompositeBoolean.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_CompositeBoolean.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAlgoAPI_ShapeProps.h>

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
  FeaturePtr aSketch =
    std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_OBJECT_ID())->value());
  if (aSketch.get() && aSketch->data()->isValid()) {
    document()->removeFeature(aSketch);
  }
  ModelAPI_CompositeFeature::erase();
}


//=================================================================================================
void FeaturesPlugin_CompositeBoolean::execute()
{
  // Getting faces to create solids.
  std::shared_ptr<ModelAPI_Feature> aSketchFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(
                                                     reference(SKETCH_OBJECT_ID())->value());
  if(!aSketchFeature) {
    return;
  }
  ResultPtr aSketchRes = aSketchFeature->results().front();
  ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aSketchRes);
  if(!aConstruction.get()) {
    return;
  }
  int aSketchFacesNum = aConstruction->facesNum();
  if(aSketchFacesNum == 0) {
    return; //TODO: set error message
  }
  ListOfShape aSketchFacesList;
  for(int aFaceIndex = 0; aFaceIndex < aSketchFacesNum; aFaceIndex++) {
    std::shared_ptr<GeomAPI_Shape> aFace = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
    aSketchFacesList.push_back(aFace);
  }

  // Pass faces to soldis creation function.
  ListOfShape aBooleanTools = MakeSolids(aSketchFacesList);
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

        if(GeomAlgoAPI_ShapeProps::volume(aBoolAlgo.shape()) > 1.e-7) {
          std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
          LoadNamingDS(aResultBody, anObject, aBooleanTools, aBoolAlgo);
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
      LoadNamingDS(aResultBody, aBooleanObjects.front(), aBooleanTools, aBoolAlgo);
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
void FeaturesPlugin_CompositeBoolean::LoadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                                   const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                                   const ListOfShape& theTools,
                                                   const GeomAlgoAPI_Boolean& theAlgo)
{
  //load result
  if(theBaseShape->isEqual(theAlgo.shape())) {
    theResultBody->store(theAlgo.shape());
  } else {
    theResultBody->storeModified(theBaseShape, theAlgo.shape());

    GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();

    const int aModTag = 1;
    const int aDeleteTag = 2;
    const std::string aModName = "Modified";
    theResultBody->loadAndOrientModifiedShapes(theAlgo.makeShape().get(), theBaseShape, GeomAPI_Shape::FACE,
                                               aModTag, aModName, *theAlgo.mapOfShapes().get());
    theResultBody->loadDeletedShapes(theAlgo.makeShape().get(), theBaseShape, GeomAPI_Shape::FACE, aDeleteTag);

    for(ListOfShape::const_iterator anIter = theTools.begin(); anIter != theTools.end(); anIter++) {
      theResultBody->loadAndOrientModifiedShapes(theAlgo.makeShape().get(), *anIter, GeomAPI_Shape::FACE,
                                                 aModTag, aModName, *theAlgo.mapOfShapes().get());
      theResultBody->loadDeletedShapes(theAlgo.makeShape().get(), *anIter, GeomAPI_Shape::FACE, aDeleteTag);
    }
  }
}
