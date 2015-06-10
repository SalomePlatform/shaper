// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionCut.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_ExtrusionCut.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAlgoAPI_Boolean.h>
#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_ShapeProps.h>

//=================================================================================================
FeaturesPlugin_ExtrusionCut::FeaturesPlugin_ExtrusionCut()
{
}

//=================================================================================================
void FeaturesPlugin_ExtrusionCut::initAttributes()
{

  data()->addAttribute(SKETCH_OBJECT_ID(), ModelAPI_AttributeReference::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FROM_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TO_OBJECT_ID());

  data()->addAttribute(CUTLIST_ID(), ModelAPI_AttributeSelectionList::typeId());

  // extrusion works with faces always
  AttributeSelectionListPtr aSelection = data()->selectionList(CUTLIST_ID());
  aSelection->setSelectionType("SOLID");
}


std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_ExtrusionCut::addFeature(std::string theID)
{
  std::shared_ptr<ModelAPI_Feature> aNew = document()->addFeature(theID, false);
  if (aNew) {
    data()->reference(SKETCH_OBJECT_ID())->setValue(aNew);
  }
   // set as current also after it becomes sub to set correctly enabled for other sketch subs
  document()->setCurrentFeature(aNew, false);
  return aNew;
}

  
int FeaturesPlugin_ExtrusionCut::numberOfSubs() const
{
  ObjectPtr aObj = data()->reference(SKETCH_OBJECT_ID())->value();
  return aObj.get()? 1 : 0;
}

std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_ExtrusionCut::subFeature(const int theIndex) const
{
  if (theIndex == 0)
    return std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_OBJECT_ID())->value());
  return std::shared_ptr<ModelAPI_Feature>();
}

int FeaturesPlugin_ExtrusionCut::subFeatureId(const int theIndex) const
{
  std::shared_ptr<ModelAPI_Feature> aFeature = subFeature(theIndex);
  if (aFeature.get())
    return aFeature->data()->featureId();
  return -1;
}

bool FeaturesPlugin_ExtrusionCut::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature)
    return false;
 
  ObjectPtr aSub = data()->reference(SKETCH_OBJECT_ID())->value();
  return aSub == theObject;
}

void FeaturesPlugin_ExtrusionCut::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
}

//=================================================================================================
void FeaturesPlugin_ExtrusionCut::execute()
{
  // Getting extrusion bounding planes.
  std::shared_ptr<GeomAPI_Shape> aFromShape;
  std::shared_ptr<GeomAPI_Shape> aToShape;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(FROM_OBJECT_ID());
  if (anObjRef) {
    aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
  }
  anObjRef = selection(TO_OBJECT_ID());
  if (anObjRef) {
    aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
  }

  // Getting extrusion sizes.
  double aFromSize = real(FROM_SIZE_ID())->value();
  double aToSize = real(TO_SIZE_ID())->value();

  // Getting faces to extrude.
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

  // Extrude faces.
  ListOfShape anExtrusionList;
  for(int aFaceIndex = 0; aFaceIndex < aSketchFacesNum; aFaceIndex++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
    GeomAlgoAPI_Prism aPrismAlgo(aBaseShape, aFromShape, aFromSize, aToShape, aToSize);

    // Checking that the algorithm worked properly.
    if(!aPrismAlgo.isDone() || aPrismAlgo.shape()->isNull() || !aPrismAlgo.isValid()) {
      return;
    }
    anExtrusionList.push_back(aPrismAlgo.shape());
  }

  // Getting objects to cut from.
  ListOfShape aCutList;
  AttributeSelectionListPtr anObjectsSelList = selectionList(CUTLIST_ID());
  if (anObjectsSelList->size() == 0) {
    return;
  }
  for(int anObjectsIndex = 0; anObjectsIndex < anObjectsSelList->size(); anObjectsIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjectAttr = anObjectsSelList->value(anObjectsIndex);
    std::shared_ptr<GeomAPI_Shape> anObject = anObjectAttr->value();
    if(!anObject.get()) {
      return;
    }
    aCutList.push_back(anObject);
  }

  // Cut from each objec result of extrusion.
  int aResultIndex = 0;
  for(ListOfShape::iterator aCutListIt = aCutList.begin(); aCutListIt != aCutList.end(); aCutListIt++) {
    std::shared_ptr<GeomAPI_Shape> anObject = *aCutListIt;
    ListOfShape aListWithObject;
    aListWithObject.push_back(anObject);
    GeomAlgoAPI_Boolean aBoolAlgo(aListWithObject, anExtrusionList, GeomAlgoAPI_Boolean::BOOL_CUT);

    // Checking that the algorithm worked properly.
    if(!aBoolAlgo.isDone() || aBoolAlgo.shape()->isNull() || !aBoolAlgo.isValid()) {
      return;
    }

    if(GeomAlgoAPI_ShapeProps::volume(aBoolAlgo.shape()) > 1.e-7) {
      std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data(), aResultIndex);
      LoadNamingDS(aResultBody, anObject, anExtrusionList, aBoolAlgo);
      setResult(aResultBody, aResultIndex);
      aResultIndex++;
    }
  }
}

//=================================================================================================
void FeaturesPlugin_ExtrusionCut::LoadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
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
