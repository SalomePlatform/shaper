// File:        PartSet_Validators.h
// Created:     09 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_Validators_H
#define PartSet_Validators_H

#include "PartSet.h"

#include <ModuleBase_SelectionValidator.h>
#include <ModuleBase_ISelection.h>

/*
* Selector validators
*/

//! A class to validate a selection for Distance constraint operation
class PartSet_DistanceValidator: public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! A class to validate a selection for Length constraint operation
class PartSet_LengthValidator: public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! A class to validate a selection for Perpendicular constraint operation
class PartSet_PerpendicularValidator: public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! A class to validate a selection for Perpendicular constraint operation
class PartSet_ParallelValidator: public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! A class to validate a selection for Perpendicular constraint operation
class PartSet_RadiusValidator: public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};


#endif
