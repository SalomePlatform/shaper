// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_ConstructionComposite.h
// Created:     20 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_ConstructionComposite_H
#define GeomValidators_ConstructionComposite_H

#include "GeomValidators.h"
#include "ModelAPI_AttributeValidator.h"

/**
* \ingroup Validators
* A validator of selection
*/
class GeomValidators_ConstructionComposite : public ModelAPI_AttributeValidator
{
 public:
   GEOMVALIDATORS_EXPORT GeomValidators_ConstructionComposite() {}
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments) const;
};

#endif
