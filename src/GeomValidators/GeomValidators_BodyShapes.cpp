// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_BodyShapes.cpp
// Created:     21 December 2015
// Author:      Dmitry Bobylev

#include "GeomValidators_BodyShapes.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Object.h>

bool GeomValidators_BodyShapes::isValid(const AttributePtr& theAttribute,
                                      const std::list<std::string>& theArguments,
                                      std::string& theError) const
{
  std::string anAttributeType = theAttribute->attributeType();
  if (anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = 
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    // all context objects should not be sketch entities
    for(int i = 0, aSize = aSelectionListAttr->size(); i < aSize; i++) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
      ObjectPtr anObject = aSelectAttr->context();
      if (!anObject.get())
        return false;
      else {
        FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
        std::string aFeatureKind = aFeature->getKind();
        if(aFeatureKind == "Sketch") {
          return false;
        }
      }
    }
  }

  return true;
}
