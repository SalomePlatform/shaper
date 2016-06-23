// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_MinObjectsSelected.h
// Created:     30 June 2015
// Author:      Dmitry Bobylev

#ifndef GeomValidators_MinObjectsSelected_H
#define GeomValidators_MinObjectsSelected_H

#include <GeomValidators.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_FeatureValidator.h>

/// \class GeomValidators_MinObjectsSelected
/// \ingroup Validators
/// \brief Validates number of objects in selection list.
class GeomValidators_MinObjectsSelected : public ModelAPI_FeatureValidator
{
public:
  /// \return true if selection list has enough objects.
  /// \param[in] theFeature the validated feature.
  /// \param[in] theArguments the arguments in the configuration file for this validator.
  /// \param[out] theError error message.
  /// \returns true if feature is valid.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const std::shared_ptr<ModelAPI_Feature>& theFeature,
                                             const std::list<std::string>& theArguments,
                                             Events_InfoMessage& theError) const;

  /// \return true if the attribute in feature is not obligatory for the feature execution.
  GEOMVALIDATORS_EXPORT virtual bool isNotObligatory(std::string theFeature, std::string theAttribute);
};

#endif
