// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_FeatureValidator.h
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModelAPI_FeatureValidator_H
#define ModelAPI_FeatureValidator_H

#include "ModelAPI.h"
#include "ModelAPI_Validator.h"

/**\class ModelAPI_FeatureValidator
 * \ingroup DataModel
 * \brief The geneneric validator for the whole feature.
 *
 * Can be redefined for some specific feature, but by default for each feature this validator is
 * used: it checks each argument of the feature and if one of it is not valid (and obligatory),
 * the hole feature is invalid.
 */
class MODELAPI_EXPORT ModelAPI_FeatureValidator : public ModelAPI_Validator
{
 public:
  /// Default constructor
  ModelAPI_FeatureValidator();
  /// Virtual destructor
  virtual ~ModelAPI_FeatureValidator();

  /// Returns true if feature and/or attributes are valid
  /// \param theFeature the validated feature
  /// \param theArguments list of string, feature attribute names: dependent attributes
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
    const std::list<std::string>& theArguments) const = 0;

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute) = 0;
};

#endif
