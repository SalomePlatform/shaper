// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Different.h
// Created:     08 July 2015
// Author:      Sergey POKHODENKO

#ifndef GeomValidators_Different_H
#define GeomValidators_Different_H

#include <GeomValidators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_FeatureValidator.h>

/** \class GeomValidators_Different
 *  \ingroup Validators
 *  \brief Validates that attributes are not the same.
 */
class GeomValidators_Different : public ModelAPI_FeatureValidator
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

  GEOMVALIDATORS_EXPORT virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
