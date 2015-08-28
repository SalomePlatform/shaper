// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_PartSet.h
// Created:     27 Aug 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_PartSet_H
#define GeomValidators_PartSet_H

#include <GeomValidators.h>
#include <ModelAPI_AttributeValidator.h>

/**
* \ingroup Validators
* A validator for partset document. When PartSet is active, the attribute can accept only
* shape, which coincide to the shape of result.
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
