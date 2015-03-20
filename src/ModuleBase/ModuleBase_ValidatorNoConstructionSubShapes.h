// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ValidatorNoConstructionSubShapes.h
// Created:     20 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ValidatorNoConstructionSubShapes_H
#define ModuleBase_ValidatorNoConstructionSubShapes_H

#include "ModuleBase.h"
#include "ModelAPI_AttributeValidator.h"

/**
* \ingroup Validators
* A validator of selection
*/
class ModuleBase_ValidatorNoConstructionSubShapes : public ModelAPI_AttributeValidator
{
 public:
   MODULEBASE_EXPORT ModuleBase_ValidatorNoConstructionSubShapes() {}
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  MODULEBASE_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                         const std::list<std::string>& theArguments) const;
};

#endif
