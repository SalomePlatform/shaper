// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_FeatureValidator.h
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModelAPI_FeatureValidator_H
#define ModelAPI_FeatureValidator_H

#include "ModelAPI.h"
#include "ModelAPI_Validator.h"

class ModelAPI_FeatureValidator : public ModelAPI_Validator
{
 public:
  /// Returns true if feature and/or attributes are valid
  /// \param theFeature the validated feature
  /// \param theAttr the validated attribute ID, empty string of feature is validated
  /// \param theArguments list of string, feature attribute names: dependent attributes
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
    const std::list<std::string>& theArguments) const = 0;

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute) = 0;
};

#endif
