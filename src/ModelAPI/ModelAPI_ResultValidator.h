// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultValidators.h
// Created:     23 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModelAPI_ResultValidators_H
#define ModelAPI_ResultValidators_H

#include "ModelAPI_Validator.h"
#include "ModelAPI_Object.h"

/**\class ModelAPI_ResultValidator
 * \ingroup DataModel
 * \brief The base class for selection filter for results of features.
 *
 * The referenced arguments of feature is normally results displayed in the viewer or by the 
 * selection attribute. This filter allows to filter out the results not usable as this argument.
 */
class ModelAPI_ResultValidator : public ModelAPI_Validator
{
public:
  /// The abstract method for implementation in the specific validator.
  /// \param theObject the checked object as an argument
  /// \returns true if object is OK for this filter
  virtual bool isValid(const ObjectPtr theObject) const = 0;
};

#endif
