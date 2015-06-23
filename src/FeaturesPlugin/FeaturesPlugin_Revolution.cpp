// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Revolution.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_Revolution.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

#define FACE 4
#define EDGE 6
#define _LATERAL_TAG 1
#define _FROM_TAG 2
#define _TO_TAG 3

//=================================================================================================
FeaturesPlugin_Revolution::FeaturesPlugin_Revolution()
{
}

//=================================================================================================
void FeaturesPlugin_Revolution::initAttributes()
{
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // revolution works with faces always
  aSelection->setSelectionType("FACE");

  data()->addAttribute(AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TO_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FROM_OBJECT_ID());
}

//=================================================================================================
void FeaturesPlugin_Revolution::execute()
{
  AttributeSelectionListPtr aFaceRefs = selectionList(LIST_ID());

  //Getting axis.
  std::shared_ptr<GeomAPI_Ax1> anAxis;
  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(AXIS_OBJECT_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
    anEdge = std::make_shared<GeomAPI_Edge>(anObjRef->value());
  } else if(anObjRef->context() && anObjRef->context()->shape() && anObjRef->context()->shape()->isEdge()) {
    anEdge = std::make_shared<GeomAPI_Edge>(anObjRef->context()->shape());
  }
  if(anEdge) {
    anAxis = std::make_shared<GeomAPI_Ax1>(anEdge->line()->location(), anEdge->line()->direction());
  }

  // Getting angles.
  double aFromAngle = real(FROM_ANGLE_ID())->value();
  double aToAngle = real(TO_ANGLE_ID())->value();

  // Getting bounding planes.
  std::shared_ptr<GeomAPI_Shape> aFromShape;
  std::shared_ptr<GeomAPI_Shape> aToShape;

  if(string(CREATION_METHOD())->value() == "ByPlanesAndOffsets") {
    anObjRef = selection(FROM_OBJECT_ID());
    if(anObjRef.get() != NULL) {
      aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
      if(aFromShape.get() == NULL && anObjRef->context().get() != NULL) {
        aFromShape = anObjRef->context()->shape();
      }
    }
    anObjRef = selection(TO_OBJECT_ID());
    if(anObjRef.get() != NULL) {
      aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
      if(aToShape.get() == NULL && anObjRef->context().get() != NULL) {
        aToShape =  anObjRef->context()->shape();
      }
    }
  }

  // for each selected face generate a result
  int anIndex = 0, aResultIndex = 0;
  for(; anIndex < aFaceRefs->size(); anIndex++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aFaceRef = aFaceRefs->value(anIndex);
    ResultPtr aContextRes = aFaceRef->context();
    std::shared_ptr<GeomAPI_Shape> aContext = aContextRes->shape();
    if (!aContext.get()) {
      static const std::string aContextError = "The selection context is bad";
      setError(aContextError);
      break;
    }

    std::shared_ptr<GeomAPI_Shape> aValueFace = aFaceRef->value();
    int aFacesNum = -1; // this mean that "aFace" is used
    ResultConstructionPtr aConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContextRes);
    if (!aValueFace.get()) { // this may be the whole sketch result selected, check and get faces
      if (aConstruction.get()) {
        aFacesNum = aConstruction->facesNum();
      } else {
        static const std::string aFaceError = "Can not find basis for extrusion";
        setError(aFaceError);
        break;
      }
    }

    for(int aFaceIndex = 0; aFaceIndex < aFacesNum || aFacesNum == -1; aFaceIndex++) {
      ResultBodyPtr aResultBody = document()->createBody(data(), aResultIndex);
      std::shared_ptr<GeomAPI_Shape> aBaseShape;
      if (aFacesNum == -1) {
        aBaseShape = aValueFace;
      } else {
        aBaseShape = std::dynamic_pointer_cast<GeomAPI_Shape>(aConstruction->face(aFaceIndex));
      }

      GeomAlgoAPI_Revolution aFeature(aBaseShape, anAxis, aToShape, aToAngle, aFromShape, aFromAngle);
      if(!aFeature.isDone()) {
        static const std::string aFeatureError = "Revolution algorithm failed";
        setError(aFeatureError);
        break;
      }

      // Check if shape is valid
      if(aFeature.shape()->isNull()) {
        static const std::string aShapeError = "Resulting shape is Null";
        setError(aShapeError);
        break;
      }
      if(!aFeature.isValid()) {
        std::string aFeatureError = "Warning: resulting shape is not valid";
        setError(aFeatureError);
        break;
      }
      //LoadNamingDS
      LoadNamingDS(aFeature, aResultBody, aBaseShape, aContext);

      setResult(aResultBody, aResultIndex);
      aResultIndex++;

      if (aFacesNum == -1)
        break;
    }
  }
  // remove the rest results if there were produced in the previous pass
  removeResults(aResultIndex);
}

//=================================================================================================
void FeaturesPlugin_Revolution::LoadNamingDS(GeomAlgoAPI_Revolution& theFeature,
                                             std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                             std::shared_ptr<GeomAPI_Shape> theBasis,
                                             std::shared_ptr<GeomAPI_Shape> theContext)
{
  //load result
  if(theBasis->isEqual(theContext))
    theResultBody->store(theFeature.shape());
  else
    theResultBody->storeGenerated(theContext, theFeature.shape());

  std::shared_ptr<GeomAPI_DataMapOfShapeShape> aSubShapes = theFeature.mapOfShapes();

  std::string aGeneratedName = "LateralFace";
  theResultBody->loadAndOrientGeneratedShapes(theFeature.makeShape().get(), theBasis, EDGE,_LATERAL_TAG, aGeneratedName, *aSubShapes);

  //Insert from face
  std::string aBotName = "FromFace";
  std::shared_ptr<GeomAPI_Shape> aBottomFace = theFeature.firstShape();
  if(!aBottomFace->isNull()) {
    if(aSubShapes->isBound(aBottomFace)) {
      aBottomFace = aSubShapes->find(aBottomFace);
    }
    theResultBody->generated(aBottomFace, aBotName, _FROM_TAG);
  }

  //Insert to face
  std::string aTopName = "ToFace";
  std::shared_ptr<GeomAPI_Shape> aTopFace = theFeature.lastShape();
  if (!aTopFace->isNull()) {
    if (aSubShapes->isBound(aTopFace)) {
      aTopFace = aSubShapes->find(aTopFace);
    }
    theResultBody->generated(aTopFace, aTopName, _TO_TAG);
  }

}
