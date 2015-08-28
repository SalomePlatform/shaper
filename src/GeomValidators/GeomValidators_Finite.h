// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Finite.h
// Created:     27 Aug 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_Finite_H
#define GeomValidators_Finite_H

#include <GeomValidators.h>
#include <ModelAPI_AttributeValidator.h>

/**
* \ingroup Validators
* A validator for finite objects. It returns false for construction infinite results.
*/
class GeomValidators_Finite : public ModelAPI_AttributeValidator
{
public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const;
};

#endif
