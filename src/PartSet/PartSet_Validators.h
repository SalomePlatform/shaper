// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_Validators.h
// Created:     09 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef PartSet_Validators_H
#define PartSet_Validators_H

#include "PartSet.h"

#include <ModuleBase_SelectionValidator.h>
#include <ModuleBase_ISelection.h>
#include <ModelAPI_AttributeValidator.h>

/*
 * Selector validators
 */

//! \ingroup Validators
//! A class to validate a selection for Distance constraint operation
class PartSet_DistanceValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Length constraint operation
class PartSet_LengthValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_PerpendicularValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_ParallelValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_RadiusValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_RigidValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

/**
* \ingroup Validators
* A validator which checks that objects selected for feature attributes are different (not the same)
*/
class PartSet_DifferentObjectsValidator : public ModelAPI_AttributeValidator
{
 public:
  //! Returns true if the attribute is good for the feature attribute
  //! \param theAttribute an attribute
  //! \param theArguments a list of arguments (names of attributes to check)
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments) const;

protected:
  //! Checks whethe other feature attributes has a reference to the given attribute
  //! \param theAttribute a source attribute to find object
  //! \return a boolean value
  bool featureHasReferences(const AttributePtr& theAttribute) const;
};

/**
* \ingroup Validators
* A validator which checks that objects selected for feature attributes are different (not the same)
*/
class PartSet_SketchEntityValidator : public ModelAPI_AttributeValidator
{
 public:
  //! Returns true if the attribute is good for the feature attribute
  //! \param theAttribute an attribute
  //! \param theArguments a list of arguments (names of attributes to check)
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments) const;
};

#endif
