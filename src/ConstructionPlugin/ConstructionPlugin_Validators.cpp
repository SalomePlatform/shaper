// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ConstructionPlugin_Validators.cpp
// Created:     04 July 2016
// Author:      Dmitry Bobylev

#include "ConstructionPlugin_Validators.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>

#include <ModelAPI_AttributeSelection.h>

#include <Events_InfoMessage.h>

static std::shared_ptr<GeomAPI_Lin> getLin(const GeomShapePtr theShape);
static std::shared_ptr<GeomAPI_Pln> getPln(const GeomShapePtr theShape);

//==================================================================================================
bool ConstructionPlugin_ValidatorPointLines::isValid(const AttributePtr& theAttribute,
                                                     const std::list<std::string>& theArguments,
                                                     Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr aLineAttribute1 = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr aLineAttribute2 = aFeature->selection(theArguments.front());

  GeomShapePtr aLineShape1 = aLineAttribute1->value();
  ResultPtr aContext1 = aLineAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute not initialized.";
    return false;
  }
  if(!aLineShape1.get()) {
    aLineShape1 = aContext1->shape();
  }
  if(!aLineShape1->isEdge()) {
    theError = "One of the selected shapes not an edge.";
    return false;
  }

  GeomShapePtr aLineShape2 = aLineAttribute2->value();
  ResultPtr aContext2 = aLineAttribute2->context();
  if(!aContext2.get()) {
    return true;
  }
  if(!aLineShape2.get()) {
    aLineShape2 = aContext2->shape();
  }
  if(!aLineShape2->isEdge()) {
    theError = "One of the selected shapes not an edge.";
    return false;
  }

  std::shared_ptr<GeomAPI_Edge> aLineEdge1(new GeomAPI_Edge(aLineShape1));
  std::shared_ptr<GeomAPI_Edge> aLineEdge2(new GeomAPI_Edge(aLineShape2));

  std::shared_ptr<GeomAPI_Lin> aLine1 = aLineEdge1->line();
  std::shared_ptr<GeomAPI_Lin> aLine2 = aLineEdge2->line();

  if(!aLine1->isCoplanar(aLine2)) {
    theError = "Selected lines not coplanar.";
    return false;
  }

  if(aLine1->isParallel(aLine2)) {
    theError = "Selected lines are parallel.";
    return false;
  }

  return true;
}

//==================================================================================================
bool ConstructionPlugin_ValidatorPointLineAndPlaneNotParallel::isValid(
    const AttributePtr& theAttribute,
    const std::list<std::string>& theArguments,
    Events_InfoMessage& theError) const
{
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());

  AttributeSelectionPtr anAttribute1 = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  AttributeSelectionPtr anAttribute2 = aFeature->selection(theArguments.front());

  std::shared_ptr<GeomAPI_Lin> aLin;
  std::shared_ptr<GeomAPI_Pln> aPln;

  GeomShapePtr aShape1 = anAttribute1->value();
  ResultPtr aContext1 = anAttribute1->context();
  if(!aContext1.get()) {
    theError = "One of the attribute not initialized.";
    return false;
  }
  if(!aShape1.get()) {
    aShape1 = aContext1->shape();
  }

  GeomShapePtr aShape2 = anAttribute2->value();
  ResultPtr aContext2 = anAttribute2->context();
  if(!aContext2.get()) {
    return true;
  }
  if(!aShape2.get()) {
    aShape2 = aContext2->shape();
  }

  aLin = getLin(aShape1);
  aPln = getPln(aShape2);
  if(!aLin.get() || !aPln.get()) {
    aLin = getLin(aShape2);
    aPln = getPln(aShape1);
  }

  if(!aLin.get() || !aPln.get()) {
    theError = "Wrong shape types selected.";
    return false;
  }

  if(aPln->isParallel(aLin)) {
    theError = "Plane and line are parallel.";
    return false;
  }

  return true;
}

static std::shared_ptr<GeomAPI_Lin> getLin(const GeomShapePtr theShape)
{
  std::shared_ptr<GeomAPI_Lin> aLin;

  if(!theShape->isEdge()) {
    return aLin;
  }

  std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(theShape));

  if(!anEdge->isLine()) {
    return aLin;
  }

  aLin = anEdge->line();

  return aLin;
}

static std::shared_ptr<GeomAPI_Pln> getPln(const GeomShapePtr theShape)
{
  std::shared_ptr<GeomAPI_Pln> aPln;

  if(!theShape->isFace()) {
    return aPln;
  }

  std::shared_ptr<GeomAPI_Face> aFace(new GeomAPI_Face(theShape));

  if(!aFace->isPlanar()) {
    return aPln;
  }

  aPln = aFace->getPlane();

  return aPln;
}
