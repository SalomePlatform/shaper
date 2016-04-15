// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        FeaturesPlugin_Wire.cpp
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_Wire.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultBody.h>

#include <GeomAlgoAPI_WireBuilder.h>

//=================================================================================================
FeaturesPlugin_Wire::FeaturesPlugin_Wire()
{
}

//=================================================================================================
void FeaturesPlugin_Wire::initAttributes()
{
  data()->addAttribute(BASE_OBJECTS_ID(), ModelAPI_AttributeSelectionList::typeId());
}

//=================================================================================================
void FeaturesPlugin_Wire::execute()
{
  // Get base objects list.
  AttributeSelectionListPtr aSelectionList = selectionList(BASE_OBJECTS_ID());
  if(!aSelectionList.get()) {
    setError("Error: Could not get selection list.");
    return;
  }
  if(aSelectionList->size() == 0) {
    setError("Error: Empty selection list.");
    return;
  }

  // Collect base shapes.
  ListOfShape aListOfShapes;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      setError("Error: Empty shape selected.");
      return;
    }

    if(aShape->shapeType() != GeomAPI_Shape::EDGE && aShape->shapeType() != GeomAPI_Shape::WIRE) {
      setError("Error: Selected shape has wrong type. Only edges and wires acceptable.");
      return;
    }

    aListOfShapes.push_back(aShape);
  }

  // Create wire.
  GeomShapePtr aWire = GeomAlgoAPI_WireBuilder::wire(aListOfShapes);
  if(!aWire.get()) {
    setError("Error: Result wire empty. Probably it has disconnected edges or non-manifold.");
    return;
  }

  // Store result.
  ResultBodyPtr aResultBody = document()->createBody(data());
  aResultBody->store(aWire);
  setResult(aResultBody);
}

