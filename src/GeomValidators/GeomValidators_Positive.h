// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_ValidatorPositive.h
// Created:     16 Sep 2014
// Author:      Mikhail PONIKAROV

#ifndef GeomValidators_ValidatorPositive_H
#define GeomValidators_ValidatorPositive_H

#include <GeomValidators.h>
#include <ModelAPI_AttributeValidator.h>

/**
 * Validates that the double attribute is greater than 1.e-5
 */
class GeomValidators_Positive : public ModelAPI_AttributeValidator
{
public:
  //! Constructor for only one instance per application: will register the validator
  GeomValidators_Positive();
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments) const;

};

#endif
