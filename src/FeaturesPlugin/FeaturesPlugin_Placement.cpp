// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Placement.cpp
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#include "FeaturesPlugin_Placement.h"

#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAlgoAPI_Placement.h>

#define _MODIFIEDF_TAG 1
#define _MODIFIEDE_TAG 2
#define _MODIFIEDV_TAG 3
#define _FACE 4
FeaturesPlugin_Placement::FeaturesPlugin_Placement()
{
}

void FeaturesPlugin_Placement::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Placement::BASE_OBJECT_ID(), ModelAPI_AttributeSelection::type());
  data()->addAttribute(FeaturesPlugin_Placement::ATTRACT_OBJECT_ID(), ModelAPI_AttributeSelection::type());
  data()->addAttribute(FeaturesPlugin_Placement::REVERSE_ID(), ModelAPI_AttributeBoolean::type());
  data()->addAttribute(FeaturesPlugin_Placement::CENTERING_ID(), ModelAPI_AttributeBoolean::type());
}

void FeaturesPlugin_Placement::execute()
{
  // Verify the base face
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = std::dynamic_pointer_cast<
    ModelAPI_AttributeSelection>(data()->attribute(FeaturesPlugin_Placement::BASE_OBJECT_ID()));
  if (!anObjRef)
    return;

  std::shared_ptr<GeomAPI_Shape> aBaseShape = 
    std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
  if (!aBaseShape)
    return;

  std::shared_ptr<GeomAPI_Shape> aBaseObject;
  ResultPtr aContextRes = anObjRef->context();
  if (aContextRes) {
    if (aContextRes->groupName() == ModelAPI_ResultBody::group())
      aBaseObject = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContextRes)->shape();
    else if (aContextRes->groupName() == ModelAPI_ResultConstruction::group())
      aBaseObject = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContextRes)->shape();
  }
  if (!aBaseObject) {
    static const std::string aContextError = "The selection context is bad";
    setError(aContextError);
    return;
  }

  // Verify the attractive face
  anObjRef = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(
      data()->attribute(FeaturesPlugin_Placement::ATTRACT_OBJECT_ID()));

  std::shared_ptr<GeomAPI_Shape> aSlaveShape = 
    std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
  if (!aSlaveShape)
    return;

  std::shared_ptr<GeomAPI_Shape> aSlaveObject;
  aContextRes = anObjRef->context();
  if (aContextRes) {
    if (aContextRes->groupName() == ModelAPI_ResultBody::group())
      aSlaveObject = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContextRes)->shape();
    else if (aContextRes->groupName() == ModelAPI_ResultConstruction::group())
      aSlaveObject = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContextRes)->shape();
  }
  if (!aSlaveObject) {
    static const std::string aContextError = "The selection context is bad";
    setError(aContextError);
    return;
  }

  // Verify planarity of faces and linearity of edges
  std::shared_ptr<GeomAPI_Shape> aShapes[2] = {aBaseShape, aSlaveShape};
  for (int i = 0; i < 2; i++) {
    if (aShapes[i]->isFace()) {
      std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(aShapes[i]));
      if (!aFace->isPlanar()) {
        static const std::string aPlanarityError = "One of selected faces is not planar";
        setError(aPlanarityError);
        return;
      }
    }
    else if (aShapes[i]->isEdge()) {
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aShapes[i]));
      if (!anEdge->isLine()) {
        static const std::string aLinearityError = "One of selected endges is not linear";
        setError(aLinearityError);
        return;
      }
    }
  }

  // Flags of the Placement
  AttributeBooleanPtr aBoolAttr = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
      data()->attribute(FeaturesPlugin_Placement::REVERSE_ID()));
  bool isReverse = aBoolAttr->value();
  aBoolAttr = std::dynamic_pointer_cast<ModelAPI_AttributeBoolean>(
      data()->attribute(FeaturesPlugin_Placement::CENTERING_ID()));
  bool isCentering = aBoolAttr->value();

  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
  GeomAlgoAPI_Placement aFeature(aSlaveObject, aBaseObject, aSlaveShape, aBaseShape, isReverse, isCentering);
  if(!aFeature.isDone()) {
    static const std::string aFeatureError = "Placement algorithm failed";
    setError(aFeatureError);
    return;
  }

  // Check if shape is valid
  if (aFeature.shape()->isNull()) {
    static const std::string aShapeError = "Resulting shape is Null";
    setError(aShapeError);
    return;
  }
  if(!aFeature.isValid()) {
    std::string aFeatureError = "Warning: resulting shape is not valid";
    setError(aFeatureError);
    return;
  }  
  //LoadNamingDS
  LoadNamingDS(aFeature, aResultBody, aSlaveObject);

  setResult(aResultBody);
}

//============================================================================
void FeaturesPlugin_Placement::LoadNamingDS(
    GeomAlgoAPI_Placement& theFeature,
    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
    std::shared_ptr<GeomAPI_Shape> theSlaveObject)
{
  //load result
  theResultBody->storeModified(theSlaveObject, theFeature.shape()); // the initial Slave, the resulting Slave

  GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();
  theFeature.mapOfShapes(*aSubShapes);
  
    // put modifed faces in DF
  std::string aModName = "Modified";
  theResultBody->loadAndOrientModifiedShapes(theFeature.makeShape(), theSlaveObject, _FACE, _MODIFIEDF_TAG, aModName, *aSubShapes); 

}
