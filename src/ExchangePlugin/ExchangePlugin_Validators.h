// File:        SketchPlugin_Validators.h
// Created:     01 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef EXCHANGEPLUGIN_VALIDATORS_H
#define EXCHANGEPLUGIN_VALIDATORS_H

#include "ExchangePlugin.h"
#include <ModelAPI_AttributeValidator.h>

class ExchangePlugin_ImportFormatValidator : public ModelAPI_AttributeValidator
{
 public:
  virtual bool isValid(const FeaturePtr& theFeature,
                       const std::list<std::string>& theArguments,
                       const ObjectPtr& theObject) const;

};

#endif
