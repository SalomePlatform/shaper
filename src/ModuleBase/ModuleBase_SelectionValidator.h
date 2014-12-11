// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_SelectionValidator.h
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_SelectionValidator_H
#define ModuleBase_SelectionValidator_H

#include "ModuleBase.h"
#include "ModuleBase_ISelection.h"

#include <ModelAPI_Validator.h>

class ModuleBase_SelectionValidator : public ModelAPI_Validator
{
 public:
  virtual bool isValid(const ModuleBase_ISelection* theSelection) const = 0;
};

#endif
