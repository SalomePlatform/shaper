// File:        ModuleBase_ViewSelectionValidator.h
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_ViewSelectionValidator_H
#define ModuleBase_ViewSelectionValidator_H

#include "ModuleBase.h"

#include <ModelAPI_Validator.h>

class MODULEBASE_EXPORT ModuleBase_ViewSelectionValidator: public ModelAPI_Validator
{
public:
  virtual bool isValid(const Handle(AIS_InteractiveContext)& theContext) const = 0;
};

#endif
