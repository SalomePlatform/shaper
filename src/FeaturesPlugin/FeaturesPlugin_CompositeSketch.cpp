// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_CompositeSketch.cpp
// Created:     11 September 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_CompositeSketch.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_BodyBuilder.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_Prism.h>
#include <GeomAlgoAPI_Revolution.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <sstream>

//=================================================================================================
void FeaturesPlugin_CompositeSketch::initAttributes()
{
  data()->addAttribute(SKETCH_OBJECT_ID(), ModelAPI_AttributeReference::typeId());
  data()->addAttribute(SKETCH_SELECTION_ID(), ModelAPI_AttributeSelection::typeId());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), SKETCH_SELECTION_ID());

  //initMakeSolidsAttributes();
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeSketch::addFeature(std::string theID)
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
int FeaturesPlugin_CompositeSketch::numberOfSubs(bool forTree) const
{
  ObjectPtr aObj = data()->reference(SKETCH_OBJECT_ID())->value();
  return aObj.get()? 1 : 0;
}

//=================================================================================================
std::shared_ptr<ModelAPI_Feature> FeaturesPlugin_CompositeSketch::subFeature(const int theIndex, bool forTree)
{
  if (theIndex == 0)
    return std::dynamic_pointer_cast<ModelAPI_Feature>(data()->reference(SKETCH_OBJECT_ID())->value());
  return std::shared_ptr<ModelAPI_Feature>();
}

//=================================================================================================
int FeaturesPlugin_CompositeSketch::subFeatureId(const int theIndex) const
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
bool FeaturesPlugin_CompositeSketch::isSub(ObjectPtr theObject) const
{
  // check is this feature of result
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
  if (!aFeature)
    return false;
 
  ObjectPtr aSub = data()->reference(SKETCH_OBJECT_ID())->value();
  return aSub == theObject;
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature)
{
  AttributeSelectionListPtr aFacesSelectionList = selectionList(LIST_ID());
  if (aFacesSelectionList.get() && aFacesSelectionList->size() > 0)
    aFacesSelectionList->clear();
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::erase()
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
void FeaturesPlugin_CompositeSketch::execute()
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

  /// feature extrusion does not have the next attribute
  if (data()->attribute(SKETCH_SELECTION_ID()).get()) {
    if (!selection(SKETCH_SELECTION_ID())->isInitialized() || selection(SKETCH_SELECTION_ID())->context() != aSketchRes) {
      selection(SKETCH_SELECTION_ID())->setValue(aSketchRes, std::shared_ptr<GeomAPI_Shape>());
    }
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

  // Generating result for each shell and face.
  int aErrorsNum = 0;
  int aResultIndex = 0;
  for(ListOfShape::const_iterator anIter = aShells.cbegin(); anIter != aShells.cend(); anIter++) {
    std::shared_ptr<GeomAlgoAPI_MakeShape> aMakeShape;

    std::shared_ptr<GeomAPI_Shape> aBaseFace = *anIter;
    makeSolid(aBaseFace, aMakeShape);
    if(!aMakeShape.get()) {
      aErrorsNum++;
      continue;
    }

    ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
    loadNamingDS(aResultBody, aBaseFace, aMakeShape);
    setResult(aResultBody, aResultIndex);
    aResultIndex++;
  }

  if(aErrorsNum > 0) {
    std::ostringstream aStringStream;
    aStringStream << "Error: Could not create solid(s) from " << aErrorsNum << " face(s).";
    setError(aStringStream.str());
  }

  // Remove the rest results if there were produced in the previous pass.
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_CompositeSketch::loadNamingDS(std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                                  const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                                  const std::shared_ptr<GeomAlgoAPI_MakeShape>& theMakeShape)
{
  //load result
  theResultBody->storeGenerated(theBaseShape, theMakeShape->shape());

  //Insert lateral face : Face from Edge
  const std::string aLatName = "LateralFace";
  const int aLatTag = 1;
  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aDataMap = theMakeShape->mapOfSubShapes();
  theResultBody->loadAndOrientGeneratedShapes(theMakeShape.get(), theBaseShape, GeomAPI_Shape::EDGE, aLatTag, aLatName, *aDataMap.get());

  std::shared_ptr<GeomAlgoAPI_MakeSweep> aSweepAlgo = std::dynamic_pointer_cast<GeomAlgoAPI_MakeSweep>(theMakeShape);
  if(aSweepAlgo.get()) {
    //Insert to faces
    int aToFaceIndex = 1;
    const std::string aToName = "ToFace";
    int aToTag = 2;
    const ListOfShape& aToFaces = aSweepAlgo->toShapes();
    for(ListOfShape::const_iterator anIt = aToFaces.cbegin(); anIt != aToFaces.cend(); anIt++) {
      std::shared_ptr<GeomAPI_Shape> aToFace = *anIt;
      if(aDataMap->isBound(aToFace)) {
        aToFace = aDataMap->find(aToFace);
      }
      std::ostringstream aStr;
      aStr << aToName << "_" << aToFaceIndex++;
      theResultBody->generated(aToFace, aStr.str(), aToTag++);
    }

    //Insert from faces
    int aFromFaceIndex = 1;
    const std::string aFromName = "FromFace";
    int aFromTag = aToTag > 10000 ? aToTag : 10000;
    const ListOfShape& aFromFaces = aSweepAlgo->fromShapes();
    for(ListOfShape::const_iterator anIt = aFromFaces.cbegin(); anIt != aFromFaces.cend(); anIt++) {
      std::shared_ptr<GeomAPI_Shape> aFromFace = *anIt;
      if(aDataMap->isBound(aFromFace)) {
        aFromFace = aDataMap->find(aFromFace);
      }
      std::ostringstream aStr;
      aStr << aFromName << "_" << aFromFaceIndex++;
      theResultBody->generated(aFromFace, aStr.str(), aFromTag++);
    }
  }
}
//=================================================================================================
void FeaturesPlugin_CompositeSketch::setSketchObjectToList()
{
  std::shared_ptr<ModelAPI_Feature> aSketchFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(
                                                       reference(SKETCH_OBJECT_ID())->value());

  if(aSketchFeature.get() && !aSketchFeature->results().empty()) {
    ResultPtr aSketchRes = aSketchFeature->results().front();
    ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aSketchRes);
    if(aConstruction.get()) {
      AttributeSelectionListPtr aFacesSelectionList = selectionList(LIST_ID());
      if (aFacesSelectionList.get() && aFacesSelectionList->size() == 0)
        aFacesSelectionList->append(aSketchRes, std::shared_ptr<GeomAPI_Shape>());
    }
  }
}

