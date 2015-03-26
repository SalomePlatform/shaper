// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Tools.cpp
// Created:     06 Aug 2014
// Author:      Vitaly Smetannikov

#include "GeomValidators_Tools.h"

#include "ModelAPI_AttributeRefAttr.h"
#include "ModelAPI_AttributeSelection.h"
#include "ModelAPI_AttributeReference.h"

namespace GeomValidators_Tools {

  ObjectPtr getObject(const AttributePtr& theAttribute)
  {
    ObjectPtr anObject;
    std::string anAttrType = theAttribute->attributeType();
    if (anAttrType == ModelAPI_AttributeRefAttr::typeId()) {
      AttributeRefAttrPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(theAttribute);
      if (anAttr != NULL && anAttr->isObject())
        anObject = anAttr->object();
    }
    if (anAttrType == ModelAPI_AttributeSelection::typeId()) {
      AttributeSelectionPtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
      if (anAttr != NULL && anAttr->isInitialized())
        anObject = anAttr->context();
    }
    if (anAttrType == ModelAPI_AttributeReference::typeId()) {
      AttributeReferencePtr anAttr = std::dynamic_pointer_cast<ModelAPI_AttributeReference>(theAttribute);
      if (anAttr.get() != NULL && anAttr->isInitialized())
        anObject = anAttr->value();
    }
    return anObject;
  }
}
