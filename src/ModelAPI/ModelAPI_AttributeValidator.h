// File:        ModelAPI_AttributeValidator.h
// Created:     5 Aug 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModelAPI_AttributeValidator_H
#define ModelAPI_AttributeValidator_H

#include <ModelAPI_Feature.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Validator.h>

class ModelAPI_AttributeValidator : public ModelAPI_Validator
{
 public:
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const ObjectPtr& theObject) const = 0;
};

#endif
