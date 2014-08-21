// File:        ModelAPI_ResultValidators.h
// Created:     23 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModelAPI_ResultValidators_H
#define ModelAPI_ResultValidators_H

#include "ModelAPI_Validator.h"
#include "ModelAPI_Object.h"

class ModelAPI_ResultValidator : public ModelAPI_Validator
{
 public:
  virtual bool isValid(const ObjectPtr theObject) const = 0;
};

#endif
