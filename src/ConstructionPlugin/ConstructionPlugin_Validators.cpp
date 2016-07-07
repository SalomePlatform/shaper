// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        ConstructionPlugin_Validators.cpp
// Created:     04 July 2016
// Author:      Dmitry Bobylev

#include "ConstructionPlugin_Validators.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Lin.h>

#include <ModelAPI_AttributeSelection.h>

#include <Events_InfoMessage.h>

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
