// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_IntersectionSelection.h
// Created:     16 Feb 2016
// Author:      Dmitry Bobylev

#ifndef GeomValidators_IntersectionSelection_H
#define GeomValidators_IntersectionSelection_H

#include <GeomValidators.h>

#include <ModelAPI_AttributeValidator.h>
#include <ModelAPI_Attribute.h>

/// \class GeomValidators_ZeroOffset
/// \ingroup Validators
/// \brief Validates selection for boolean operation.
class GeomValidators_IntersectionSelection: public ModelAPI_AttributeValidator
{
public:
  /// \return True if the attribute is valid. It checks whether the selection
  /// is acceptable for boolean operation.
  /// \param[in] theAttribute an attribute to check.
  /// \param[in] theArguments a filter parameters.
  /// \param[out] theError error message.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const;
};

#endif
