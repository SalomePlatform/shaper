// File:        ModuleBase_ResultValidators.h
// Created:     23 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_ResultValidators_H
#define ModuleBase_ResultValidators_H

#include "ModuleBase.h"
#include <ModelAPI_Validator.h>
#include <ModelAPI_Object.h>

class ModuleBase_ResultValidator: public ModelAPI_Validator
{
public:
  virtual bool isValid(const ObjectPtr theObject) const = 0;
};

class ModuleBase_ResulPointValidator: public ModuleBase_ResultValidator
{
public:
  MODULEBASE_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

class ModuleBase_ResulLineValidator: public ModuleBase_ResultValidator
{
public:
  MODULEBASE_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

class ModuleBase_ResulArcValidator: public ModuleBase_ResultValidator
{
public:
  MODULEBASE_EXPORT virtual bool isValid(const ObjectPtr theObject) const;
};

#endif