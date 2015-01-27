// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_SelectionValidator.h
// Created:     8 Jul 2014
// Author:      Vitaly SMETANNIKOV

#ifndef ModuleBase_SelectionValidator_H
#define ModuleBase_SelectionValidator_H

#include "ModuleBase.h"
#include "ModuleBase_ISelection.h"

#include <ModelAPI_Validator.h>

#include <list>
#include <string>

/**
* A validator of selection
*/
class ModuleBase_SelectionValidator : public ModelAPI_Validator
{
 public:
   /// Returns True if selection is valid
   /// \param theSelection selection instance
  virtual bool isValid(const ModuleBase_ISelection* theSelection) const = 0;

   /// Returns True if selection is valid
   /// \param theSelection selection instance
   /// \param theArguments list of arguments
  virtual bool isValid(const ModuleBase_ISelection* theSelection,
                       const std::list<std::string>& theArguments) const
  {
    return isValid(theSelection);
  }
};

#endif
