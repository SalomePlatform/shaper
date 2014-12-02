// File:        FeaturesPlugin_Placement.cpp
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#include "FeaturesPlugin_Placement.h"

#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeSelection.h>

#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAlgoAPI_Placement.h>

FeaturesPlugin_Placement::FeaturesPlugin_Placement()
{
}

void FeaturesPlugin_Placement::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Placement::BASE_FACE_ID(), ModelAPI_AttributeSelection::type());
  data()->addAttribute(FeaturesPlugin_Placement::ATTRACT_FACE_ID(), ModelAPI_AttributeSelection::type());
}

void FeaturesPlugin_Placement::execute()
{
  // Verify the base face
  std::shared_ptr<ModelAPI_AttributeSelection> aFaceRef = std::dynamic_pointer_cast<
    ModelAPI_AttributeSelection>(data()->attribute(FeaturesPlugin_Placement::BASE_FACE_ID()));
  if (!aFaceRef)
    return;

  std::shared_ptr<GeomAPI_Shape> aBaseFace = 
    std::dynamic_pointer_cast<GeomAPI_Shape>(aFaceRef->value());
  if (!aBaseFace)
    return;

  std::shared_ptr<GeomAPI_Shape> aBaseFaceContext;
  ResultPtr aContextRes = aFaceRef->context();
  if (aContextRes) {
    if (aContextRes->groupName() == ModelAPI_ResultBody::group())
      aBaseFaceContext = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContextRes)->shape();
    else if (aContextRes->groupName() == ModelAPI_ResultConstruction::group())
      aBaseFaceContext = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContextRes)->shape();
  }
  if (!aBaseFaceContext) {
    static const std::string aContextError = "The selection context is bad";
    setError(aContextError);
    return;
  }

  // Verify the attractive face
  aFaceRef = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(
      data()->attribute(FeaturesPlugin_Placement::ATTRACT_FACE_ID()));

  std::shared_ptr<GeomAPI_Shape> anAttractiveFace = 
    std::dynamic_pointer_cast<GeomAPI_Shape>(aFaceRef->value());
  if (!anAttractiveFace)
    return;

  std::shared_ptr<GeomAPI_Shape> anAttractiveFaceContext;
  aContextRes = aFaceRef->context();
  if (aContextRes) {
    if (aContextRes->groupName() == ModelAPI_ResultBody::group())
      anAttractiveFaceContext = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContextRes)->shape();
    else if (aContextRes->groupName() == ModelAPI_ResultConstruction::group())
      anAttractiveFaceContext = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContextRes)->shape();
  }
  if (!anAttractiveFaceContext) {
    static const std::string aContextError = "The selection context is bad";
    setError(aContextError);
    return;
  }

  // Verify faces planarity
  std::shared_ptr<GeomAPI_Face> aBaseFace1(new GeomAPI_Face(aBaseFace));
  std::shared_ptr<GeomAPI_Face> anAttractFace1(new GeomAPI_Face(anAttractiveFace));
  if (!aBaseFace1->isPlanar() || !anAttractFace1->isPlanar()) {
    static const std::string aPlanarityError = "One of selected face is not planar";
    setError(aPlanarityError);
    return;
  }

  std::shared_ptr<GeomAPI_Pln> aBasePlane = aBaseFace1->getPlane();
  std::shared_ptr<GeomAPI_Pln> anAttractivePlane = anAttractFace1->getPlane();

  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
  GeomAlgoAPI_Placement aFeature(anAttractiveFaceContext, anAttractivePlane, aBasePlane);
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
  LoadNamingDS(aFeature, aResultBody, anAttractiveFace, anAttractiveFaceContext);

  setResult(aResultBody);
}

//============================================================================
void FeaturesPlugin_Placement::LoadNamingDS(
    GeomAlgoAPI_Placement& theFeature,
    std::shared_ptr<ModelAPI_ResultBody> theResultBody,
    std::shared_ptr<GeomAPI_Shape> theBasis,
    std::shared_ptr<GeomAPI_Shape> theContext)
{
  /// TODO: SZY
/*

  //load result
  if(theBasis->isEqual(theContext))
    theResultBody->store(theFeature.shape());
  else
    theResultBody->storeGenerated(theContext, theFeature.shape()); 

  GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();
  theFeature.mapOfShapes(*aSubShapes);

    //Insert lateral face : Face from Edge
  theResultBody->loadAndOrientGeneratedShapes(theFeature.makeShape(), theBasis, EDGE,_LATERAL_TAG, *aSubShapes);

  //Insert bottom face
  std::shared_ptr<GeomAPI_Shape> aBottomFace = theFeature.firstShape();  
  if (!aBottomFace->isNull()) {
	if (aSubShapes->isBound(aBottomFace)) {	 
		aBottomFace = aSubShapes->find(aBottomFace);		
    }    
    theResultBody->generated(aBottomFace, _FIRST_TAG);
  }



  //Insert top face
  std::shared_ptr<GeomAPI_Shape> aTopFace = theFeature.lastShape();
  if (!aTopFace->isNull()) {
    if (aSubShapes->isBound(aTopFace)) {	 
      aTopFace = aSubShapes->find(aTopFace);	
    }
    theResultBody->generated(aTopFace, _LAST_TAG);
  }

*/
}
