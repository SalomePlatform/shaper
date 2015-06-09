// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Extrusion.cpp
// Created:     30 May 2014
// Author:      Vitaly SMETANNIKOV

#include "FeaturesPlugin_Extrusion.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeReference.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_XYZ.h>
#include <GeomAlgoAPI_Extrusion.h>
#include <GeomAlgoAPI_FaceBuilder.h>
#include <GeomAlgoAPI_Prism.h>

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
  AttributeSelectionListPtr aSelection = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(data()->addAttribute(
    FeaturesPlugin_Extrusion::LIST_ID(), ModelAPI_AttributeSelectionList::typeId()));
  // extrusion works with faces always
  aSelection->setSelectionType("FACE");

  data()->addAttribute(FeaturesPlugin_Extrusion::TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FeaturesPlugin_Extrusion::FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FeaturesPlugin_Extrusion::FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FeaturesPlugin_Extrusion::TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FeaturesPlugin_Extrusion::FROM_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FeaturesPlugin_Extrusion::TO_OBJECT_ID());
}

void FeaturesPlugin_Extrusion::execute()
{
  AttributeSelectionListPtr aFaceRefs = selectionList(FeaturesPlugin_Extrusion::LIST_ID());

  std::shared_ptr<GeomAPI_Shape> aFromShape;
  std::shared_ptr<GeomAPI_Shape> aToShape;

  // Getting bounding planes.
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(FeaturesPlugin_Extrusion::FROM_OBJECT_ID());
  if (anObjRef) {
    aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
  }
  anObjRef = selection(FeaturesPlugin_Extrusion::TO_OBJECT_ID());
  if (anObjRef) {
    aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
  }

  // Getting sizes.
  double aFromSize = real(FeaturesPlugin_Extrusion::FROM_SIZE_ID())->value();
  double aToSize = real(FeaturesPlugin_Extrusion::TO_SIZE_ID())->value();

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

      // If bounding faces was not set creating them.
      std::shared_ptr<GeomAPI_Face>  aBaseFace(new GeomAPI_Face(aBaseShape));
      std::shared_ptr<GeomAPI_Pln>   aBasePln = aBaseFace->getPlane();
      std::shared_ptr<GeomAPI_Dir>   aBaseDir = aBasePln->direction();
      std::shared_ptr<GeomAPI_Pnt>   aBaseLoc = aBasePln->location();
      std::shared_ptr<GeomAPI_Shape> aBasePlane = GeomAlgoAPI_FaceBuilder::planarFace(aBaseLoc, aBaseDir);

      std::shared_ptr<GeomAPI_Shape> aBoundingFromShape = aFromShape ? aFromShape : aBasePlane;
      std::shared_ptr<GeomAPI_Shape> aBoundingToShape   = aToShape   ? aToShape   : aBasePlane;

      // Moving bounding faces according to "from" and "to" sizes.
      std::shared_ptr<GeomAPI_Face> aFromFace(new GeomAPI_Face(aBoundingFromShape));
      std::shared_ptr<GeomAPI_Pln>  aFromPln = aFromFace->getPlane();
      std::shared_ptr<GeomAPI_Pnt>  aFromLoc = aFromPln->location();
      std::shared_ptr<GeomAPI_Dir>  aFromDir = aFromPln->direction();

      std::shared_ptr<GeomAPI_Face> aToFace(new GeomAPI_Face(aBoundingToShape));
      std::shared_ptr<GeomAPI_Pln>  aToPln = aToFace->getPlane();
      std::shared_ptr<GeomAPI_Pnt>  aToLoc = aToPln->location();
      std::shared_ptr<GeomAPI_Dir>  aToDir = aToPln->direction();

      bool aSign = aFromLoc->xyz()->dot(aBaseDir->xyz()) > aToLoc->xyz()->dot(aBaseDir->xyz());

      std::shared_ptr<GeomAPI_Pnt> aFromPnt(new GeomAPI_Pnt(aFromLoc->xyz()->added(aBaseDir->xyz()->multiplied(aSign ? aFromSize : -aFromSize))));
      aBoundingFromShape = GeomAlgoAPI_FaceBuilder::planarFace(aFromPnt, aFromDir);

      std::shared_ptr<GeomAPI_Pnt> aToPnt(new GeomAPI_Pnt(aToLoc->xyz()->added(aBaseDir->xyz()->multiplied(aSign ? -aToSize : aToSize))));
      aBoundingToShape = GeomAlgoAPI_FaceBuilder::planarFace(aToPnt, aToDir);

      //GeomAlgoAPI_Extrusion aFeature(aFace, aFromSize);
      GeomAlgoAPI_Prism aFeature(aBaseShape, aBoundingFromShape, aBoundingToShape);
      if(!aFeature.isDone()) {
        static const std::string aFeatureError = "Extrusion algorithm failed";
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

//============================================================================
void FeaturesPlugin_Extrusion::LoadNamingDS(GeomAlgoAPI_Prism& theFeature,
                                            std::shared_ptr<ModelAPI_ResultBody> theResultBody,
                                            std::shared_ptr<GeomAPI_Shape> theBasis,
                                            std::shared_ptr<GeomAPI_Shape> theContext)
{
  //load result
  if(theBasis->isEqual(theContext))
    theResultBody->store(theFeature.shape());
  else
    theResultBody->storeGenerated(theBasis, theFeature.shape());

  GeomAPI_DataMapOfShapeShape* aSubShapes = new GeomAPI_DataMapOfShapeShape();
  theFeature.mapOfShapes(*aSubShapes);

  //Insert lateral face : Face from Edge
  std::string aLatName = "LateralFace";
  theResultBody->loadAndOrientGeneratedShapes(theFeature.makeShape(), theBasis, EDGE,_LATERAL_TAG, aLatName, *aSubShapes);

  //Insert bottom face
  std::string aBotName = "BottomFace";
  std::shared_ptr<GeomAPI_Shape> aBottomFace = theFeature.firstShape();
  if(!aBottomFace->isNull()) {
    if(aSubShapes->isBound(aBottomFace)) {
      aBottomFace = aSubShapes->find(aBottomFace);
    }
    theResultBody->generated(aBottomFace, aBotName, _FIRST_TAG);
  }

  //Insert top face
  std::string aTopName = "TopFace";
  std::shared_ptr<GeomAPI_Shape> aTopFace = theFeature.lastShape();
  if (!aTopFace->isNull()) {
    if (aSubShapes->isBound(aTopFace)) {
      aTopFace = aSubShapes->find(aTopFace);
    }
    theResultBody->generated(aTopFace, aTopName, _LAST_TAG);
  }
}
