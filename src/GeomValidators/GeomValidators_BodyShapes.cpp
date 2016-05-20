// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_BodyShapes.cpp
// Created:     21 December 2015
// Author:      Dmitry Bobylev

#include "GeomValidators_BodyShapes.h"

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultConstruction.h>

bool GeomValidators_BodyShapes::isValid(const AttributePtr& theAttribute,
                                        const std::list<std::string>& theArguments,
                                        std::string& theError) const
{
  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = 
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    // all context objects should not be sketch entities
    for(int anIndex = 0, aSize = aSelectionListAttr->size(); anIndex < aSize; ++anIndex) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(anIndex);
      ResultPtr aContext = aSelectAttr->context();
      if(!aContext.get()) {
        theError = "Error: Context is empty.";
        return false;
      }

      ResultConstructionPtr aResultConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
      if(aResultConstruction.get()) {
        theError = "Error: Result construction selected.";
        return false;
      }
    }
  }

  return true;
}
