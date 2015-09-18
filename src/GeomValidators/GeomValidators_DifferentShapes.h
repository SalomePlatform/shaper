// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_DifferentShapes.h
// Created:     2 Feb 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_DifferentShapes_H
#define GeomValidators_DifferentShapes_H

#include <ModelAPI.h>

#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Attribute.h>

/**
 * Generic validator for any attribute of a feature.
 */
class GeomValidators_DifferentShapes : public ModelAPI_AttributeValidator
{
public:
  /// returns True if the attribute is valid. It checks whether the feature of the attribute
  /// does not contain a selection attribute filled with the same shape
  /// \param[in] theAttribute an attribute to check
  /// \param[in] theArguments a filter parameters
  /// \param[out] theError error message.
  MODELAPI_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                       const std::list<std::string>& theArguments,
                                       std::string& theError) const;
};

#endif
