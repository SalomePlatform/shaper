// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_BodyShapes.cpp
// Created:     21 December 2015
// Author:      Dmitry Bobylev

#include "GeomValidators_BodyShapes.h"

#include <Events_InfoMessage.h>

#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_ResultConstruction.h>

bool GeomValidators_BodyShapes::isValid(const AttributePtr& theAttribute,
                                        const std::list<std::string>& theArguments,
                                        Events_InfoMessage& theError) const
{
  std::string anAttributeType = theAttribute->attributeType();
  if(anAttributeType == ModelAPI_AttributeSelection::typeId()) {
    AttributeSelectionPtr anAttrSelection =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(theAttribute);
    ResultPtr aContext = anAttrSelection->context();
    if(!aContext.get()) {
      theError = "Error: Context is empty.";
      return false;
    }

    ResultConstructionPtr aResultConstruction = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if(aResultConstruction.get()) {
      theError = "Error: Result construction selected.";
      return false;
    }
  } else if(anAttributeType == ModelAPI_AttributeSelectionList::typeId()) {
    AttributeSelectionListPtr anAttrSelectionList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(theAttribute);

    // All objects should not be result constructions.
    for(int anIndex = 0, aSize = anAttrSelectionList->size(); anIndex < aSize; ++anIndex) {
      AttributeSelectionPtr anAttrSelection = anAttrSelectionList->value(anIndex);
      if(!isValid(anAttrSelection, theArguments, theError)) {
        return false;
      }
    }
  } else {
    theError = "Error: Attribute \"%1\" does not supported by this validator.";
    theError.arg(anAttributeType);
    return false;
  }

  return true;
}
