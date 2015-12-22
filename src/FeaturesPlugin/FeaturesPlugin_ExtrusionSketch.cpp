// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionSketch.cpp
// Created:     11 September 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_ExtrusionSketch.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Prism.h>

//=================================================================================================
FeaturesPlugin_ExtrusionSketch::FeaturesPlugin_ExtrusionSketch()
{
}

//=================================================================================================
void FeaturesPlugin_ExtrusionSketch::initMakeSolidsAttributes()
{
  data()->addAttribute(CREATION_METHOD(), ModelAPI_AttributeString::typeId());

  data()->addAttribute(TO_SIZE_ID(), ModelAPI_AttributeDouble::typeId());
  data()->addAttribute(FROM_SIZE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_OFFSET_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TO_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FROM_OBJECT_ID());
}

//=================================================================================================
void FeaturesPlugin_ExtrusionSketch::makeSolid(const std::shared_ptr<GeomAPI_Shape> theFace,
                                               std::shared_ptr<GeomAPI_Shape>& theResult,
                                               ListOfShape& theFromFaces,
                                               ListOfShape& theToFaces,
                                               std::shared_ptr<GeomAlgoAPI_MakeShape>& theMakeShape,
                                               std::shared_ptr<GeomAPI_DataMapOfShapeShape>& theDataMap)
{
  // Getting extrusion sizes.
  double aToSize = 0.0;
  double aFromSize = 0.0;

  if(string(CREATION_METHOD())->value() == "BySizes") {
    aToSize = real(TO_SIZE_ID())->value();
    aFromSize =  real(FROM_SIZE_ID())->value();
  } else {
    aToSize = real(TO_OFFSET_ID())->value();
    aFromSize =  real(FROM_OFFSET_ID())->value();
  }

  // Getting extrusion bounding planes.
  std::shared_ptr<GeomAPI_Shape> aToShape;
  std::shared_ptr<GeomAPI_Shape> aFromShape;

  if(string(CREATION_METHOD())->value() == "ByPlanesAndOffsets") {
    std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(TO_OBJECT_ID());
    if(anObjRef.get() != NULL) {
      aToShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
      if(aToShape.get() == NULL && anObjRef->context().get() != NULL) {
        aToShape =  anObjRef->context()->shape();
      }
    }
    anObjRef = selection(FROM_OBJECT_ID());
    if(anObjRef.get() != NULL) {
      aFromShape = std::dynamic_pointer_cast<GeomAPI_Shape>(anObjRef->value());
      if(aFromShape.get() == NULL && anObjRef->context().get() != NULL) {
        aFromShape = anObjRef->context()->shape();
      }
    }
  }

  // Extrude face
  std::shared_ptr<GeomAlgoAPI_Prism> aPrismAlgo(new GeomAlgoAPI_Prism(theFace, aToShape, aToSize, aFromShape, aFromSize));

  // Checking that the algorithm worked properly.
  if(!aPrismAlgo->isDone() || !aPrismAlgo->shape().get() || aPrismAlgo->shape()->isNull() ||
     !aPrismAlgo->isValid()) {
    return;
  }

  theResult = aPrismAlgo->shape();
  theFromFaces = aPrismAlgo->fromFaces();
  theToFaces = aPrismAlgo->toFaces();
  theMakeShape = aPrismAlgo;
  theDataMap = aPrismAlgo->mapOfSubShapes();
}
