// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Validators.h
// Created:     22 March 2016
// Author:      Dmitry Bobylev

#ifndef FeaturesPlugin_Validators_H_
#define FeaturesPlugin_Validators_H_

#include <ModelAPI_FeatureValidator.h>

/// \class FeaturesPlugin_PipeLocationsValidator
/// \ingroup Validators
/// \brief Validator for the pipe locations.
class FeaturesPlugin_PipeLocationsValidator : public ModelAPI_FeatureValidator
{
 public:
  //! \return true if number of selected locations the same as number of selected bases, or empty.
  //! \param theFeature the checked feature
  //! \param theArguments arguments of the feature (not used)
  //! \param theError error message
  virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;

  /// Returns true if the attribute in feature is not obligatory for the feature execution
  virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
