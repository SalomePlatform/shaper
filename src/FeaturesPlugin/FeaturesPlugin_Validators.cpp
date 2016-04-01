// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Validators.cpp
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_Validators.h"

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultConstruction.h>

//=================================================================================================
bool FeaturesPlugin_PipeLocationsValidator::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                    const std::list<std::string>& theArguments,
                                                    std::string& theError) const
{
  static const std::string aCreationMethodID = "creation_method";
  static const std::string aBaseObjectsID = "base_objects";
  static const std::string aLocationsID = "locations_objects";

  if(theFeature->getKind() != "Pipe") {
    theError = "Feature \"" + theFeature->getKind() + "\" does not supported by this validator.";
    return false;
  }

  AttributeStringPtr aCreationMethodAttr = theFeature->string(aCreationMethodID);
  if(!aCreationMethodAttr.get()) {
    theError = "Could not get \"" + aCreationMethodID + "\" attribute.";
    return false;
  }

  if(aCreationMethodAttr->value() != "locations") {
    return true;
  }

  AttributeSelectionListPtr aBaseObjectsSelectionList = theFeature->selectionList(aBaseObjectsID);
  if(!aBaseObjectsSelectionList.get()) {
    theError = "Could not get \"" + aBaseObjectsID + "\" attribute.";
    return false;
  }

  AttributeSelectionListPtr aLocationsSelectionList = theFeature->selectionList(aLocationsID);
  if(!aLocationsSelectionList.get()) {
    theError = "Could not get \"" + aBaseObjectsID + "\" attribute.";
    return false;
  }

  if(aLocationsSelectionList->size() > 0 && aLocationsSelectionList->size() != aBaseObjectsSelectionList->size()) {
    theError = "Number of locations should be the same as base objects.";
    return false;
  }

  return true;
}

//=================================================================================================
bool FeaturesPlugin_PipeLocationsValidator::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  return false;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorBaseForGeneration::isValid(const AttributePtr& theAttribute,
                                                        const std::list<std::string>& theArguments,
                                                        std::string& theError) const
{
  // Checking attribute.
  if(!isValidAttribute(theAttribute, theError)) {
    if(theError.empty()) {
      theError = "Attribute contains shape with unacceptable type.";
    }
    return false;
  }

  return true;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorBaseForGeneration::isValidAttribute(const AttributePtr& theAttribute,
                                                                 std::string& theError) const
{
  if(!theAttribute.get()) {
    theError = "Empty attribute.";
    return false;
  }

  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    for(int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
      // If at least one attribute is invalid, the result is false.
      if(!isValidAttribute(aListAttr->value(anIndex), theError)) {
        return false;
      }
    }
  } else if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    // Getting context.
    AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aContext = anAttr->context();
    if(!aContext.get()) {
      theError = "Attribute have empty context.";
      return false;
    }

    GeomShapePtr aShape = anAttr->value();
    GeomShapePtr aContextShape = aContext->shape();
    if(!aShape.get()) {
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      theError = "Empty shape selected";
      return false;
    }

    ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aConstruction.get()) {
      // Construciotn selected. Check that is is not infinite.
      if(aConstruction->isInfinite()) {
        theError = "Infinite constructions is not allowed as base.";
        return false;
      }

      if(aShape->isEqual(aContextShape)) {
        // Whole construction selected. Check that it have faces.
        if(aConstruction->facesNum() > 0) {
          return true;
        }
      } else {
        // Shape on construction selected. Check that it is a face or wire.
        if(aShape->shapeType() == GeomAPI_Shape::WIRE || aShape->shapeType() == GeomAPI_Shape::FACE) {
          return true;
        }
      }
    }

    if(!aShape->isEqual(aContextShape)) {
      // Local selection on body does not allowed.
      theError = "Selected shape is in the local selection. Only global selection is allowed.";
      return false;
    }

    // Check that object is a shape with allowed type.
    aShape = aContext->shape();
    GeomAPI_Shape::ShapeType aShapeType = aShape->shapeType();
    if(aShapeType != GeomAPI_Shape::VERTEX &&
       aShapeType != GeomAPI_Shape::EDGE &&
       aShapeType != GeomAPI_Shape::WIRE &&
       aShapeType != GeomAPI_Shape::FACE) {
      theError = "Selected shape has unacceptable type. Acceptable types are: faces or wires on sketch, \
                  whole sketch(if it has at least one face), and following objects: vertex, edge, wire, face.";
      return false;
    }

  } else {
    theError = "Following attribute does not supported: " + anAttributeType + ".";
    return false;
  }

  return true;
}