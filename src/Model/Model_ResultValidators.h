// File:        Model_ResultValidators.h
// Created:     23 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef Model_ResultValidators_H
#define Model_ResultValidators_H

#include "Model.h"
#include <ModelAPI_ResultValidator.h>
#include <ModelAPI_Object.h>

class Model_ResultPointValidator: public ModelAPI_ResultValidator
{
public:
  MODEL_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

class Model_ResultLineValidator: public ModelAPI_ResultValidator
{
public:
  MODEL_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

class Model_ResultArcValidator: public ModelAPI_ResultValidator
{
public:
  MODEL_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

#endif