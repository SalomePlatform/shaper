// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_BaseForGeneration.cpp
// Created:     18 March 2016
// Author:      Dmitry Bobylev

#include "GeomValidators_BaseForGeneration.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultConstruction.h>

#include <algorithm>

//=================================================================================================
bool GeomValidators_BaseForGeneration::isValid(const AttributePtr& theAttribute,
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
bool GeomValidators_BaseForGeneration::isValidAttribute(const AttributePtr& theAttribute,
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

    // Getting feature.
    FeaturePtr aFeature = ModelAPI_Feature::feature(aContext);
    if(!aFeature.get()) {
      theError = "Empty feature.";
      return false;
    }

    // Checking feature kind.
    std::string aFeatureKind = aFeature->getKind();
    if(aFeatureKind == "Axis" ||
       aFeatureKind == "Plane") {
      theError = "Shape from feature \"" + aFeatureKind +"\" is not allowed for selection.";
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

    if(aFeatureKind == "Sketch") {
      if(aShape->isEqual(aContextShape)) {
        // Whole sketch selected. Check that it have faces.
        ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
        if(aConstruction->facesNum() == 0) {
          theError = "Selected sketch does not have faces.";
          return false;
        }
      } else {
        // Shape on sketch selected. Check that it is a face.
        if(aShape->shapeType() != GeomAPI_Shape::FACE) {
          theError = "Selected shape has unacceptable type. Acceptable types are: faces on sketch, \
                      whole sketch(if it has at least one face), and following objects: vertex, edge, wire, face.";
          return false;
        }
      }
    } else {
      if(!aShape->isEqual(aContextShape)) {
        // Local selection does not allowed.
        theError = "Selected shape is in the local selection. Only global selection is allowed.";
        return false;
      } else {
        // Check that object is a shape with allowed type.
        aShape = aContext->shape();
        GeomAPI_Shape::ShapeType aShapeType = aShape->shapeType();
        if(aShapeType != GeomAPI_Shape::VERTEX &&
           aShapeType != GeomAPI_Shape::EDGE &&
           aShapeType != GeomAPI_Shape::WIRE &&
           aShapeType != GeomAPI_Shape::FACE) {
          theError = "Selected shape has unacceptable type. Acceptable types are: faces on sketch, \
                      whole sketch(if it has at least one face), and following objects: vertex, edge, wire, face.";
          return false;
        }
      }
    }
  } else {
    theError = "Following attribute does not supported: " + anAttributeType + ".";
    return false;
  }

  return true;
}