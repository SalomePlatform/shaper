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

bool GeomValidators_Positive::isValid(
    const AttributePtr& theAttribute, const std::list<std::string>& theArguments) const
{
  AttributeDoublePtr aDouble = 
    std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(theAttribute);
  if (aDouble.get())
    return aDouble->isInitialized() && aDouble->value() > 1.e-5;
  AttributeIntegerPtr aInteger = 
    std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(theAttribute);
  return aInteger->isInitialized() && aInteger->value() > 0;
}
