// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Validators.cpp
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#include "FeaturesPlugin_Validators.h"

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_AttributeReference.h>

#include <Events_Error.h>

#include <GeomValidators_FeatureKind.h>
#include <GeomValidators_ShapeType.h>

#include <GeomAPI_PlanarEdges.h>
#include <GeomAlgoAPI_WireBuilder.h>

//=================================================================================================
bool FeaturesPlugin_ValidatorPipeLocations::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
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
bool FeaturesPlugin_ValidatorPipeLocations::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  return false;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorBaseForGeneration::isValid(const AttributePtr& theAttribute,
                                                        const std::list<std::string>& theArguments,
                                                        std::string& theError) const
{
  if(theArguments.empty()) {
    theError = "Validator parameters is empty.";
    return false;
  }

  // Checking attribute.
  if(!isValidAttribute(theAttribute, theArguments, theError)) {
    if(theError.empty()) {
      theError = "Attribute contains unacceptable shape.";
    }
    return false;
  }

  return true;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorBaseForGeneration::isValidAttribute(const AttributePtr& theAttribute,
                                                                 const std::list<std::string>& theArguments,
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
      if(!isValidAttribute(aListAttr->value(anIndex), theArguments, theError)) {
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
    GeomValidators_ShapeType aShapeTypeValidator;
    if(!aShapeTypeValidator.isValid(anAttr, theArguments, theError)) {
      theError = "Selected shape has unacceptable type. Acceptable types are: faces or wires on sketch, \
whole sketch(if it has at least one face), and whole objects with shape types: ";
      std::list<std::string>::const_iterator anIt = theArguments.cbegin();
      theError += *anIt;
      for(++anIt; anIt != theArguments.cend(); ++anIt) {
        theError += ", " + *anIt;
      }
      return false;
    }

  } else {
    theError = "Following attribute does not supported: " + anAttributeType + ".";
    return false;
  }

  return true;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorCompositeLauncher::isValid(const AttributePtr& theAttribute,
                                                        const std::list<std::string>& theArguments,
                                                        std::string& theError) const
{
  if (theAttribute->attributeType() != ModelAPI_AttributeReference::typeId()) {
    theError = "The attribute with the " + theAttribute->attributeType() + " type is not processed";
    return false;
  }
  if (theArguments.size() != 2) {
    theError = "Wrong parameters in XML definition for " + theAttribute->attributeType() + " type";
    return false;
  }
  // first argument is for the base attribute, second - for skipping feature kind
  std::list<std::string>::const_iterator anIt = theArguments.begin();
  std::string aBaseAttributeId = *anIt;
  FeaturePtr aFeature = ModelAPI_Feature::feature(theAttribute->owner());
  AttributePtr aBaseAttribute = aFeature->attribute(aBaseAttributeId);
  if (!aBaseAttribute.get()) {
    theError = "Wrong parameters in XML definition for " + theAttribute->attributeType() + " type";
    return false;
  }
  if (aBaseAttribute->isInitialized()) // when base list of composite feature is already filled,
    // this validator is not necessary anymore
    return true;

  anIt++;
  std::string aFeatureAttributeKind = *anIt;
  GeomValidators_FeatureKind* aValidator = new GeomValidators_FeatureKind();
  // check whether the selection is on the sketch
  std::list<std::string> anArguments;
  anArguments.push_back(aFeatureAttributeKind);

  bool aFeatureKind = aValidator->isValid(theAttribute, theArguments, theError);
  bool aPlanarFace = false;
  // check if selection has Face selected
  GeomValidators_ShapeType* aShapeType = new GeomValidators_ShapeType();
  anArguments.clear();
  anArguments.push_back("face");
  aPlanarFace = aShapeType->isValid(theAttribute, anArguments, theError);

  bool aValid = !aFeatureKind && aPlanarFace;
  return aValid;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorCanBeEmpty::isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                                 const std::list<std::string>& theArguments,
                                                 std::string& theError) const
{
  if(theArguments.size() != 2) {
    theError = "Validator should be used with 2 parameters for extrusion.";
    return false;
  }

  std::list<std::string>::const_iterator anArgsIt = theArguments.begin(), aLast = theArguments.end();

  AttributePtr aCheckAttribute = theFeature->attribute(*anArgsIt);
  ++anArgsIt;

  if(isShapesCanBeEmpty(aCheckAttribute, theError)) {
    return true;
  }

  AttributeSelectionPtr aSelAttr = theFeature->selection(*anArgsIt);
  if(!aSelAttr.get()) {
    theError = "Could not get selection attribute \"" + *anArgsIt + "\".";
    return false;
  }

  GeomShapePtr aShape = aSelAttr->value();
  if(!aShape.get()) {
    ResultPtr aContext = aSelAttr->context();
    if(!aContext.get()) {
      theError = "Selection attribute \"" + *anArgsIt + "\" can not be empty.";
      return false;
    }

    aShape = aContext->shape();
  }

  if(!aShape.get()) {
    theError = "Selection attribute \"" + *anArgsIt + "\" can not be empty.";
    return false;
  }

  return true;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorCanBeEmpty::isNotObligatory(std::string theFeature, std::string theAttribute)
{
  return false;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorCanBeEmpty::isShapesCanBeEmpty(const AttributePtr& theAttribute,
                                                            std::string& theError) const
{
  if(!theAttribute.get()) {
    return true;
  }

  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aListAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    for(int anIndex = 0; anIndex < aListAttr->size(); ++anIndex) {
      // If at least one attribute is invalid, the result is false.
      if(!isShapesCanBeEmpty(aListAttr->value(anIndex), theError)) {
        return false;
      }
    }
  } else if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    // Getting context.
    AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aContext = anAttr->context();
    if(!aContext.get()) {
      return false;
    }

    GeomShapePtr aShape = anAttr->value();
    GeomShapePtr aContextShape = aContext->shape();
    if(!aShape.get()) {
      aShape = aContextShape;
    }
    if(!aShape.get()) {
      return false;
    }

    if(aShape->shapeType() == GeomAPI_Shape::VERTEX ||
       aShape->shapeType() == GeomAPI_Shape::EDGE ||
       !aShape->isPlanar()) {
      return false;
    }
  } else {
    return false;
  }

  return true;
}

//=================================================================================================
bool FeaturesPlugin_ValidatorBaseForWire::isValid(const AttributePtr& theAttribute,
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