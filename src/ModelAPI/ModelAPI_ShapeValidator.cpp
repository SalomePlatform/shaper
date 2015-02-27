// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ShapeValidator.cpp
// Created:     2 Feb 2015
// Author:      Natalia ERMOLAEVA

#include "ModelAPI_ShapeValidator.h"

#include <ModelAPI_AttributeSelection.h>
#include "ModelAPI_Object.h"

bool ModelAPI_ShapeValidator::isValid(const FeaturePtr& theFeature,
                                      const std::list<std::string>& theArguments,
                                      const ObjectPtr& theObject,
                                      const AttributePtr& theAttribute,
                                      const GeomShapePtr& theShape) const
{
  std::string aCurrentAttributeId = theAttribute->id();
  // get all feature attributes
  std::list<AttributePtr> anAttrs = 
                   theFeature->data()->attributes(ModelAPI_AttributeSelection::type());
  if (anAttrs.size() > 0 && theShape.get() != NULL) {
    std::list<AttributePtr>::iterator anAttr = anAttrs.begin();
    for(; anAttr != anAttrs.end(); anAttr++) {
      AttributePtr anAttribute = *anAttr;
      // take into concideration only other attributes
      if (anAttribute.get() != NULL && anAttribute->id() != aCurrentAttributeId) {
        AttributeSelectionPtr aSelectionAttribute = 
          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(anAttribute);
        // the shape of the attribute should be not the same
        if (aSelectionAttribute.get() != NULL && theShape->isEqual(aSelectionAttribute->value())) {
          return false;
        }
      }
    }
  }
  return true;
}
