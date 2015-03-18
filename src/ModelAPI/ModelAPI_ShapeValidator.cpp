// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ShapeValidator.cpp
// Created:     2 Feb 2015
// Author:      Natalia ERMOLAEVA

#include "ModelAPI_ShapeValidator.h"

#include <ModelAPI_AttributeSelection.h>
#include "ModelAPI_Object.h"

bool ModelAPI_ShapeValidator::isValid(const AttributePtr& theAttribute,
                                       const std::list<std::string>& theArguments) const
{
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theAttribute->owner());
  AttributeSelectionPtr aSelectionAttribute = 
                     std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
  GeomShapePtr aShape = aSelectionAttribute->value();

  std::string aCurrentAttributeId = theAttribute->id();
  // get all feature attributes
  std::list<AttributePtr> anAttrs = 
                   aFeature->data()->attributes(ModelAPI_AttributeSelection::type());
  if (anAttrs.size() > 0 && aShape.get() != NULL) {
    std::list<AttributePtr>::iterator anAttr = anAttrs.begin();
    for(; anAttr != anAttrs.end(); anAttr++) {
      AttributePtr anAttribute = *anAttr;
      // take into concideration only other attributes
      if (anAttribute.get() != NULL && anAttribute->id() != aCurrentAttributeId) {
        AttributeSelectionPtr aSelectionAttribute = 
          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(anAttribute);
        // the shape of the attribute should be not the same
        if (aSelectionAttribute.get() != NULL && aShape->isEqual(aSelectionAttribute->value())) {
          return false;
        }
      }
    }
  }
  return true;
}
