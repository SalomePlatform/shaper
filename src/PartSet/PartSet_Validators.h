// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Validators.h
// Created:     09 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_Validators_H
#define PartSet_Validators_H

#include "PartSet.h"

#include <ModuleBase_SelectionValidator.h>
#include <ModuleBase_ISelection.h>
#include <ModelAPI_RefAttrValidator.h>

/*
 * Selector validators
 */

//! A class to validate a selection for Distance constraint operation
class PartSet_DistanceValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! A class to validate a selection for Length constraint operation
class PartSet_LengthValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! A class to validate a selection for Perpendicular constraint operation
class PartSet_PerpendicularValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! A class to validate a selection for Perpendicular constraint operation
class PartSet_ParallelValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! A class to validate a selection for Perpendicular constraint operation
class PartSet_RadiusValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! A class to validate a selection for Perpendicular constraint operation
class PartSet_RigidValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

class PartSet_DifferentObjectsValidator : public ModelAPI_RefAttrValidator
{
 public:
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const ObjectPtr& theObject) const;
  //! Returns true if the attribute is good for the feature attribute
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const AttributePtr& theAttribute) const;

  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments) const;
};


#endif
