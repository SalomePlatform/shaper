// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildPlugin_Validators.cpp
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Validators.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_PlanarEdges.h>

#include <GeomAlgoAPI_WireBuilder.h>

#include <Events_Error.h>

//=================================================================================================
bool BuildPlugin_ValidatorBaseForWire::isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               std::string& theError) const
{
  // Get base objects list.
  if(theAttribute->attributeType() != ModelAPI_AttributeSelectionList::typeId()) {
    Events_Error::send("Validator does not support attribute type \"" + theAttribute->attributeType()
      + "\"\n Only \"" + ModelAPI_AttributeSelectionList::typeId() + "\" supported.");
    return false;
  }
  AttributeSelectionListPtr aSelectionList = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  if(!aSelectionList.get()) {
    theError = "Could not get selection list.";
    return false;
  }
  if(aSelectionList->size() == 0) {
    theError = "Empty selection list.";
    return false;
  }

  // Collect base shapes.
  ListOfShape aListOfShapes;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    if(!aSelection.get()) {
      theError = "Could not get selection.";
      return false;
    }
    ResultPtr aContext = aSelection->context();
    if(!aContext.get()) {
      theError = "Attribute have empty context.";
      return false;
    }

    GeomShapePtr aShape = aSelection->value();
    GeomShapePtr aContextShape = aContext->shape();
    if(!aShape.get()) {
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      theError = "Empty shape selected.";
      return false;
    }

    // Check that shape has acceptable type.
    if(aShape->shapeType() != GeomAPI_Shape::EDGE && aShape->shapeType() != GeomAPI_Shape::WIRE) {
      theError = "Selected shape has wrong type. Only edges and wires acceptable.";
      return false;
    }

    // Check that it is edge on sketch.
    ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aConstruction.get()) {
      if(aConstruction->isInfinite()) {
        theError = "Inifinte objects not acceptable.";
        return false;
      }

      std::shared_ptr<GeomAPI_PlanarEdges> anEdges = std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aContextShape);
      if(!anEdges.get()) {
        // It is not an edge on the sketch.
        // Check that it is not local selection.
        if(!aShape->isEqual(aContextShape)) {
          // Local selection on body does not allowed.
          theError = "Selected shape is in the local selection. Only global selection is allowed.";
          return false;
        }
      }
    }

    aListOfShapes.push_back(aShape);
  }

  // Create wire.
  GeomShapePtr aWire = GeomAlgoAPI_WireBuilder::wire(aListOfShapes);
  if(!aWire.get()) {
    theError = "Result wire empty. Probably it has disconnected edges or non-manifold.";
    return false;
  }

  return true;
}