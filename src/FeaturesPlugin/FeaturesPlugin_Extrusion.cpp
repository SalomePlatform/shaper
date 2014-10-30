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
#include <Events_Error.h>
#include <GeomAlgoAPI_Extrusion.h>

using namespace std;
#define _LATERAL_TAG 1
#define _FIRST_TAG 2
#define _LAST_TAG 3
#define EDGE 6
#ifdef _DEBUG
#include <iostream>
#include <ostream>
#endif

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
  boost::shared_ptr<ModelAPI_AttributeSelection> aFaceRef = boost::dynamic_pointer_cast<
    ModelAPI_AttributeSelection>(data()->attribute(FeaturesPlugin_Extrusion::FACE_ID()));
  if (!aFaceRef)
    return;

  boost::shared_ptr<GeomAPI_Shape> aFace = 
    boost::dynamic_pointer_cast<GeomAPI_Shape>(aFaceRef->value());
  if (!aFace)
    return;

  boost::shared_ptr<GeomAPI_Shape> aContext;
  ResultPtr aContextRes = aFaceRef->context();
  if (aContextRes) {
    if (aContextRes->groupName() == ModelAPI_ResultBody::group())
      aContext = boost::dynamic_pointer_cast<ModelAPI_ResultBody>(aContextRes)->shape();
    else if (aContextRes->groupName() == ModelAPI_ResultConstruction::group())
      aContext = boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContextRes)->shape();
  }
  if (!aContext) {
    std::string aContextError = "The selection context is bad";
    Events_Error::send(aContextError, this);
    return;
  }

  double aSize = data()->real(FeaturesPlugin_Extrusion::SIZE_ID())->value();
  if (data()->boolean(FeaturesPlugin_Extrusion::REVERSE_ID())->value())
    aSize = -aSize;

  eraseResults(); // to erase the previously stored naming structures
  boost::shared_ptr<ModelAPI_ResultBody> aResultBody = document()->createBody(data());
  GeomAlgoAPI_Extrusion aFeature(aFace, aSize);
  if(!aFeature.isDone()) {
    std::string aFeatureError = "Extrusion algorithm failed";  
    Events_Error::send(aFeatureError, this);
    return;
  }

  // Check if shape is valid
  if (aFeature.shape()->isNull()) {
    std::string aShapeError = "Resulting shape is Null";     
    Events_Error::send(aShapeError, this);
#ifdef _DEBUG
    std::cerr << aShapeError << std::endl;
#endif
    return;
  }
  if(!aFeature.isValid()) {
    std::string aFeatureError = "Warning: resulting shape is not valid";  
    Events_Error::send(aFeatureError, this);
    return;
  }  
  //LoadNamingDS
  LoadNamingDS(aFeature, aResultBody, aFace, aContext);

  setResult(aResultBody);
}

//============================================================================
void FeaturesPlugin_Extrusion::LoadNamingDS(GeomAlgoAPI_Extrusion& theFeature, 
  boost::shared_ptr<ModelAPI_ResultBody> theResultBody, 
  boost::shared_ptr<GeomAPI_Shape> theBasis,
  boost::shared_ptr<GeomAPI_Shape> theContext)
{  


  //load result
  if(theBasis->isEqual(theContext))
    theResultBody->store(theFeature.shape());
  else
    theResultBody->storeGenerated(theContext, theFeature.shape()); 

  GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();
  theFeature.mapOfShapes(*aSubShapes);

    //Insert lateral face : Face from Edge
  theResultBody->loadAndOrientGeneratedShapes(theFeature.makeShape(), theFeature.shape(), EDGE,_LATERAL_TAG, *aSubShapes);

  //Insert bottom face
  boost::shared_ptr<GeomAPI_Shape> aBottomFace = theFeature.firstShape();  
  if (!aBottomFace->isNull()) {
	if (aSubShapes->isBound(aBottomFace)) {	 
		aBottomFace = aSubShapes->find(aBottomFace);		
    }    
    theResultBody->generated(aBottomFace, _FIRST_TAG);
  }



  //Insert top face
  boost::shared_ptr<GeomAPI_Shape> aTopFace = theFeature.lastShape();
  if (!aTopFace->isNull()) {
    if (aSubShapes->isBound(aTopFace)) {	 
      aTopFace = aSubShapes->find(aTopFace);	
    }
    theResultBody->generated(aTopFace, _LAST_TAG);
  }

  
}
