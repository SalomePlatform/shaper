// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_ExtrusionBoolean.cpp
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_ExtrusionBoolean.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Prism.h>

//=================================================================================================
void FeaturesPlugin_ExtrusionBoolean::initMakeSolidsAttributes()
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
void FeaturesPlugin_ExtrusionBoolean::makeSolids(const ListOfShape& theFaces,
                                                 ListOfShape& theResults,
                                                 std::list<std::shared_ptr<GeomAPI_Interface>>& theAlgos)
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

  // Extrude faces.
  theResults.clear();
  for(ListOfShape::const_iterator aFacesIt = theFaces.begin(); aFacesIt != theFaces.end(); aFacesIt++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *aFacesIt;
    std::shared_ptr<GeomAlgoAPI_Prism> aPrismAlgo = std::shared_ptr<GeomAlgoAPI_Prism>(new GeomAlgoAPI_Prism(aBaseShape, aToShape, aToSize, aFromShape, aFromSize));

    // Checking that the algorithm worked properly.
    if(!aPrismAlgo->isDone() || !aPrismAlgo->shape().get() || aPrismAlgo->shape()->isNull() ||
       !aPrismAlgo->isValid()) {
      setError("Extrusion algorithm failed");
      theResults.clear();
      return;
    }
    theResults.push_back(aPrismAlgo->shape());
    theAlgos.push_back(aPrismAlgo);
  }
}
