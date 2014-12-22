// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Extrusion.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Extrusion.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeBoolean.h>
#include <GeomAlgoAPI_Extrusion.h>

using namespace std;
#define _LATERAL_TAG 1
#define _FIRST_TAG 2
#define _LAST_TAG 3
#define EDGE 6

FeaturesPlugin_Extrusion::FeaturesPlugin_Extrusion()
{
}

void FeaturesPlugin_Extrusion::initAttributes()
{
  data()->addAttribute(FeaturesPlugin_Extrusion::FACE_ID(), ModelAPI_AttributeSelection::type());
  data()->addAttribute(FeaturesPlugin_Extrusion::SIZE_ID(), ModelAPI_AttributeDouble::type());
  data()->addAttribute(FeaturesPlugin_Extrusion::REVERSE_ID(), ModelAPI_AttributeBoolean::type());
}

void FeaturesPlugin_Extrusion::execute()
{
  std::shared_ptr<ModelAPI_AttributeSelection> aFaceRef = std::dynamic_pointer_cast<
    ModelAPI_AttributeSelection>(data()->attribute(FeaturesPlugin_Extrusion::FACE_ID()));
  if (!aFaceRef)
    return;

  std::shared_ptr<GeomAPI_Shape> aFace = 
    std::dynamic_pointer_cast<GeomAPI_Shape>(aFaceRef->value());
  if (!aFace)
    return;

  std::shared_ptr<GeomAPI_Shape> aContext;
  ResultPtr aContextRes = aFaceRef->context();
  if (aContextRes) {
    if (aContextRes->groupName() == ModelAPI_ResultBody::group())
      aContext = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aContextRes)->shape();
    else if (aContextRes->groupName() == ModelAPI_ResultConstruction::group())
      aContext = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContextRes)->shape();
  }
  if (!aContext) {
    static const std::string aContextError = "The selection context is bad";
    setError(aContextError);
    return;
  }

  double aSize = data()->real(FeaturesPlugin_Extrusion::SIZE_ID())->value();
  if (data()->boolean(FeaturesPlugin_Extrusion::REVERSE_ID())->value())
    aSize = -aSize;

  std::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
  GeomAlgoAPI_Extrusion aFeature(aFace, aSize);
  if(!aFeature.isDone()) {
    static const std::string aFeatureError = "Extrusion algorithm failed";  
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
  LoadNamingDS(aFeature, aResultBody, aFace, aContext);

  setResult(aResultBody);
}

//============================================================================
void FeaturesPlugin_Extrusion::LoadNamingDS(GeomAlgoAPI_Extrusion& theFeature, 
  std::shared_ptr<ModelAPI_ResultBody> theResultBody, 
  std::shared_ptr<GeomAPI_Shape> theBasis,
  std::shared_ptr<GeomAPI_Shape> theContext)
{  


  //load result
  if(theBasis->isEqual(theContext))
    theResultBody->store(theFeature.shape());
  else
    theResultBody->storeGenerated(theContext, theFeature.shape()); 

  GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();
  theFeature.mapOfShapes(*aSubShapes);
  std::string aPrefix =  data()->name() + "/";
    //Insert lateral face : Face from Edge
  theResultBody->loadAndOrientGeneratedShapes(theFeature.makeShape(), theBasis, EDGE,_LATERAL_TAG, aPrefix, *aSubShapes);

  //Insert bottom face
  std::shared_ptr<GeomAPI_Shape> aBottomFace = theFeature.firstShape();  
  if (!aBottomFace->isNull()) {
	if (aSubShapes->isBound(aBottomFace)) {	 
		aBottomFace = aSubShapes->find(aBottomFace);		
    }     
    theResultBody->generated(aBottomFace, aPrefix, _FIRST_TAG);
  }



  //Insert top face
  std::shared_ptr<GeomAPI_Shape> aTopFace = theFeature.lastShape();
  if (!aTopFace->isNull()) {
    if (aSubShapes->isBound(aTopFace)) {	 
      aTopFace = aSubShapes->find(aTopFace);	
    }
    theResultBody->generated(aTopFace, aPrefix, _LAST_TAG);
  }

  
}
