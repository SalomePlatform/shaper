// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Finite.cpp
// Created:     27 Aug 2015
// Author:      Natalia ERMOLAEVA

#include <GeomValidators_Finite.h>

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_ResultConstruction.h>

bool GeomValidators_Finite::isValid(const AttributePtr& theAttribute,
                                   const std::list<std::string>& theArguments,
                                   Events_InfoMessage& theError) const
{
  bool aValid = true;

  const std::string anAttributeType = theAttribute->attributeType();

  if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr aSelectionAttr = std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aResult = aSelectionAttr->context();
    ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aResult);
    if (aConstruction.get() && aConstruction->isInfinite()) {
      aValid = false;
      theError = "Infinite result is selected.";
    }
  } else if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr aSelectionListAttr = 
                      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);
    for (int i = 0, aSize = aSelectionListAttr->size(); i < aSize; i++) {
      AttributeSelectionPtr aSelectAttr = aSelectionListAttr->value(i);
      ResultPtr aResult = aSelectAttr->context();
      if (aResult.get() && aResult->groupName() == ModelAPI_ResultConstruction::group()) {
        ResultConstructionPtr aConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aResult);
        if (aConstruction.get() && aConstruction->isInfinite()) {
          aValid = false;
          theError = "Infinite result is selected.";
        }
      }
    }
  }
  return aValid;
}

