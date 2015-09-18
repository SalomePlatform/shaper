// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_PartitionArguments.h
// Created:     17 September 2015
// Author:      Dmitry Bobylev

#ifndef GeomValidators_PartitionArguments_H
#define GeomValidators_PartitionArguments_H

#include <GeomValidators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_FeatureValidator.h>

/** \class GeomValidators_PartitionArguments
 *  \ingroup Validators
 *  \brief Validates that partition operation have enough arguments.
 */
class GeomValidators_PartitionArguments : public ModelAPI_FeatureValidator
{
public:
  /** \brief Returns true if feature and/or attributes are valid.
   *  \param[in] theFeature the validated feature.
   *  \param[in] theArguments the arguments in the configuration file for this validator.
   *  \param[out] theError error message.
   *  \returns true if feature is valid.
   */
  GEOMVALIDATORS_EXPORT virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const;

  /// \return true if the attribute in feature is not obligatory for the feature execution.
  GEOMVALIDATORS_EXPORT virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
