// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_FeatureValidator.h
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef Model_FeatureValidator_H
#define Model_FeatureValidator_H

#include <Model.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_FeatureValidator.h>

#include <memory>
#include <set>
#include <map>

class Model_FeatureValidator : public ModelAPI_FeatureValidator
{
  // not obligatory attributes, not checked for initialization
  std::map<std::string, std::set<std::string> > myNotObligatory;
 public:
  /// Returns true if feature and/or attributes are valid
  /// \param theFeature the validated feature
  MODEL_EXPORT virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
    const std::list<std::string>& theArguments) const;

  // sets not obligatory attributes, not checked for initialization
  virtual void registerNotObligatory(std::string theFeature, std::string theAttribute);

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
