// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_RevolutionBoolean.h
// Created:     11 June 2015
// Author:      Dmitry Bobylev

#include <FeaturesPlugin_RevolutionBoolean.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <GeomAlgoAPI_Revolution.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

//=================================================================================================
void FeaturesPlugin_RevolutionBoolean::initMakeSolidsAttributes()
{
  data()->addAttribute(FeaturesPlugin_RevolutionBoolean::AXIS_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());

  data()->addAttribute(FROM_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(FROM_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  data()->addAttribute(TO_OBJECT_ID(), ModelAPI_AttributeSelection::typeId());
  data()->addAttribute(TO_ANGLE_ID(), ModelAPI_AttributeDouble::typeId());

  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), FROM_OBJECT_ID());
  ModelAPI_Session::get()->validators()->registerNotObligatory(getKind(), TO_OBJECT_ID());
}

//=================================================================================================
ListOfShape FeaturesPlugin_RevolutionBoolean::MakeSolids(const ListOfShape& theFaces)
{
  //Getting axis.
  std::shared_ptr<GeomAPI_Ax1> anAxis;
  std::shared_ptr<GeomAPI_Edge> anEdge;
  std::shared_ptr<ModelAPI_AttributeSelection> anObjRef = selection(FeaturesPlugin_RevolutionBoolean::AXIS_OBJECT_ID());
  if(anObjRef && anObjRef->value() && anObjRef->value()->isEdge()) {
    anEdge = std::shared_ptr<GeomAPI_Edge>(new GeomAPI_Edge(anObjRef->value()));
  }
  if(anEdge) {
    anAxis = std::shared_ptr<GeomAPI_Ax1>(new GeomAPI_Ax1(anEdge->line()->location(), anEdge->line()->direction()));
  }

  // Getting revolution bounding planes.
  std::shared_ptr<GeomAPI_Shape> aFromShape;
  std::shared_ptr<GeomAPI_Shape> aToShape;
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

  // Getting revolution angles.
  double aFromAngle = real(FROM_ANGLE_ID())->value();
  double aToAngle = real(TO_ANGLE_ID())->value();

  // Revol faces.
  ListOfShape aRevolutionList;
  for(ListOfShape::const_iterator aFacesIt = theFaces.begin(); aFacesIt != theFaces.end(); aFacesIt++) {
    std::shared_ptr<GeomAPI_Shape> aBaseShape = *aFacesIt;
    GeomAlgoAPI_Revolution aRevolAlgo(aBaseShape, anAxis, aFromShape, aFromAngle, aToShape, aToAngle);

    // Checking that the algorithm worked properly.
    if(!aRevolAlgo.isDone() || aRevolAlgo.shape()->isNull() || !aRevolAlgo.isValid()) {
      setError("Revolution algorithm failed");
      return ListOfShape();
    }
    aRevolutionList.push_back(aRevolAlgo.shape());
  }

  return aRevolutionList;
}