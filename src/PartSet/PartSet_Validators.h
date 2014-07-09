// File:        PartSet_Validators.h
// Created:     09 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_Validators_H
#define PartSet_Validators_H

#include "PartSet.h"

#include <ModuleBase_SelectionValidator.h>
#include <ModuleBase_ISelection.h>

class PartSet_DistanceValidator: public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

class PartSet_LengthValidator: public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

#endif
