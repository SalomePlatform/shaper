// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_ValidatorPositive.cpp
// Created:     16 Sep 2014
// Author:      Mikhail PONIKAROV

#include "GeomValidators_Positive.h"
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

/// Global instance for validators factory
GeomValidators_Positive MY_POSITIVE_INSTANCE;

GeomValidators_Positive::GeomValidators_Positive()
{
  // this validator is registered in the factory on this library loading
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("GeomValidators_Positive", this);
}

bool GeomValidators_Positive::isValid(const AttributePtr& theAttribute, 
                                      const std::list<std::string>& theArguments,
                                      std::string& theError) const
{
  double aMinValue = 1.e-5;
  if(theArguments.size() == 1) {
    std::list<std::string>::const_iterator anIt = theArguments.begin();
    char *aErr;
    double aValue = strtod((*anIt).c_str(), &aErr);
    if(*aErr == 0) {
      // very probably ok
      aMinValue = aValue;
    }
  }

  if (theAttribute->attributeType() == ModelAPI_AttributeDouble::typeId()) {
    AttributeDoublePtr aDouble =
        std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
    if (!aDouble->isInitialized()) {
      theError = "Double is not initialized.";
      return false;
    }
    if (aDouble->value() <= aMinValue) {
      theError = "Double is not positive.";
      return false;
    }
  }

  if (theAttribute->attributeType() == ModelAPI_AttributeInteger::typeId()) {
    AttributeIntegerPtr aInteger =
        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttribute);
    if (!aInteger->isInitialized()) {
      theError = "Integer is not initialized.";
      return false;
    }
    if (aInteger->value() <= floor(aMinValue)) {
      theError = "Integer is not positive.";
      return false;
    }
  }

  return true;
}
