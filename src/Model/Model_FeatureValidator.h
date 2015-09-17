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

/**\class Model_FeatureValidator
 * \ingroup DataModel
 * \brief The geneneric validator for the whole feature.
 *
 * Can be redefined for some specific feature, but by default for each feature this validator is
 * used: it checks each argument of the feature and if one of it is not valid (and obligatory),
 * the hole feature is invalid.
 */
class Model_FeatureValidator : public ModelAPI_FeatureValidator
{
  // not obligatory attributes, not checked for initialization
  std::map<std::string, std::set<std::string> > myNotObligatory;
public:
  /// Returns true if feature and/or attributes are valid
  /// \param theFeature the validated feature
  /// \param theArguments the arguments in the configuration file for this validator
  /// \param theError erros message produced by validator to the user if it fails
  /// \returns true if feature is valid
  MODEL_EXPORT virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                    const std::list<std::string>& theArguments,
                                    std::string& theError) const;

  /// sets not obligatory attributes, not checked for initialization
  virtual void registerNotObligatory(std::string theFeature, std::string theAttribute);

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
