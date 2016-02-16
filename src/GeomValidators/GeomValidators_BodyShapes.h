// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_BodyShapes.h
// Created:     21 December 2015
// Author:      Dmitry Bobylev

#ifndef GeomValidators_BodyShapes_H
#define GeomValidators_BodyShapes_H

#include <GeomValidators.h>

#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Attribute.h>

/**
 * Generic validator for any attribute of a feature.
 */
class GeomValidators_BodyShapes : public ModelAPI_AttributeValidator
{
public:
  /// \return True if the attribute is valid. It checks whether the shape is a
  /// body subshape. Does not allow select construction shapes.
  /// \param[in] theAttribute an attribute to check
  /// \param[in] theArguments a filter parameters
  /// \param[out] theError error message.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const;
};

#endif
