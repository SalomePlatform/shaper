// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Face.h
// Created:     27 Aug 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_PartSet_H
#define GeomValidators_PartSet_H

#include <GeomValidators.h>
#include <ModelAPI_AttributeValidator.h>

/**
* \ingroup Validators
* A validator for partset objects.
*/
class GeomValidators_PartSet : public ModelAPI_AttributeValidator
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
