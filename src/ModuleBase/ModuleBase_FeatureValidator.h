// File:        ModuleBase_FeatureValidator.h
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_FeatureValidator_H
#define ModuleBase_FeatureValidator_H

#include "ModuleBase.h"

#include <ModelAPI_Feature.h>
#include <ModelAPI_Validator.h>


class ModuleBase_FeatureValidator: public ModelAPI_Validator
{
public:
  virtual bool isValid(const FeaturePtr theFeature) const = 0;
};

#endif
