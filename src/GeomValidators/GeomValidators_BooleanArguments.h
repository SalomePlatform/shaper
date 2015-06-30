// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_BooleanArguments.h
// Created:     30 June 2015
// Author:      Dmitry Bobylev

#ifndef GeomValidators_BooleanArguments_H
#define GeomValidators_BooleanArguments_H

#include <GeomValidators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_FeatureValidator.h>

/** \class GeomValidators_BooleanArguments
 *  \ingroup Validators
 *  \brief Validates that boolean operation have enough arguments.
 */
class GeomValidators_BooleanArguments : public ModelAPI_FeatureValidator
{
public:
  /** \brief Returns true if feature and/or attributes are valid.
   *  \param[in] theFeature the validated feature.
   *  \param[in] theArguments the arguments in the configuration file for this validator.
   *  \returns true if feature is valid.
   */
  GEOMVALIDATORS_EXPORT virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<std::string>& theArguments) const;

  /// \return true if the attribute in feature is not obligatory for the feature execution.
  GEOMVALIDATORS_EXPORT virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
