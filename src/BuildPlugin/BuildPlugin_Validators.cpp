// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildPlugin_Validators.cpp
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Validators.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultConstruction.h>

#include <GeomAPI_PlanarEdges.h>

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_WireBuilder.h>

#include <GeomValidators_ShapeType.h>

#include <Events_Error.h>

//=================================================================================================
bool BuildPlugin_ValidatorBaseForBuild::isValid(const AttributePtr& theAttribute,
                                                const std::list<std::string>& theArguments,
                                                std::string& theError) const
{
  // Get base objects list.
  if(theAttribute->attributeType() != ModelAPI_AttributeSelectionList::typeId()) {
    Events_Error::send("Error: BuildPlugin_ValidatorBaseForBuild does not support attribute type \"" + theAttribute->attributeType()
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

    // Check that shapes has acceptable type.
    GeomValidators_ShapeType aValidatorShapeType;
    if(!aValidatorShapeType.isValid(aSelection, theArguments, theError)) {
      return false;
    }

    // Check that it is shape on sketch.
    ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aConstruction.get()) {
      if(aConstruction->isInfinite()) {
        theError = "Inifinte objects not acceptable.";
        return false;
      }

      std::shared_ptr<GeomAPI_PlanarEdges> anEdges = std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(aContextShape);
      if(anEdges.get()) {
        if(aShape->isEqual(aContextShape)) {
          // It is whole sketch.
          return false;
        }
      } else if(!aShape->isEqual(aContextShape)) {
        // Local selection on body does not allowed.
        theError = "Selected shape is in the local selection. Only global selection is allowed.";
        return false;
      }
    }
  }

  return true;
}

//=================================================================================================
bool BuildPlugin_ValidatorBaseForWire::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                               const std::list<std::string>& theArguments,
                                               std::string& theError) const
{
  // Get attribute.
  if(theArguments.size() != 1) {
    Events_Error::send("Error: BuildPlugin_ValidatorBaseForWire should be used only with 1 parameter (ID of base objects list)");
    return false;
  }
  AttributePtr anAttribute = theFeature->attribute(theArguments.front());

  // Check base objects list.
  BuildPlugin_ValidatorBaseForBuild aValidatorBaseForBuild;
  std::list<std::string> anArguments;
  anArguments.push_back("edge");
  anArguments.push_back("wire");
  if(!aValidatorBaseForBuild.isValid(anAttribute, anArguments, theError)) {
    return false;
  }

  // Collect base shapes.
  AttributeSelectionListPtr aSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(anAttribute);
  ListOfShape aListOfShapes;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      aShape = aSelection->context()->shape();
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

//=================================================================================================
bool BuildPlugin_ValidatorBaseForWireisNotObligatory(std::string theFeature, std::string theAttribute)
{
  return false;
}

//=================================================================================================
bool BuildPlugin_ValidatorBaseForFace::isValid(const AttributePtr& theAttribute,
                                               const std::list<std::string>& theArguments,
                                               std::string& theError) const
{
  // Get base objects list.
  BuildPlugin_ValidatorBaseForBuild aValidatorBaseForBuild;
  std::list<std::string> anArguments;
  anArguments.push_back("edge");
  anArguments.push_back("wire");
  if(!aValidatorBaseForBuild.isValid(theAttribute, anArguments, theError)) {
    return false;
  }

  // Collect base shapes.
  AttributeSelectionListPtr aSelectionList =
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
  ListOfShape aListOfShapes;
  for(int anIndex = 0; anIndex < aSelectionList->size(); ++anIndex) {
    AttributeSelectionPtr aSelection = aSelectionList->value(anIndex);
    GeomShapePtr aShape = aSelection->value();
    if(!aShape.get()) {
      aShape = aSelection->context()->shape();
    }
    aListOfShapes.push_back(aShape);
  }

  // Check that they are planar.
  GeomShapePtr aCompound = GeomAlgoAPI_CompoundBuilder::compound(aListOfShapes);

  return aCompound->isPlanar();
}
