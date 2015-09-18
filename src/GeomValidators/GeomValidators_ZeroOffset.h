// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_ZeroOffset.h
// Created:     13 May 2015
// Author:      Dmitry Bobylev

#ifndef GeomValidators_ZeroOffset_H
#define GeomValidators_ZeroOffset_H

#include <GeomValidators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_FeatureValidator.h>

/** \class GeomValidators_ZeroOffset
 *  \ingroup Validators
 *  \brief Validates that bounding planes not the same or both offsets are not 0
 */
class GeomValidators_ZeroOffset : public ModelAPI_FeatureValidator
{
public:
  /** \brief Returns true if feature and/or attributes are valid.
   *  \param[in] theFeature the validated feature.
   *  \param[in] theArguments the arguments in the configuration file for this validator.
   *  \return true if feature is valid.
   */
  GEOMVALIDATORS_EXPORT virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const;

  /// \return true if the attribute in feature is not obligatory for the feature execution.
  GEOMVALIDATORS_EXPORT virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
