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
#include <ModelAPI_ResultValidator.h>

/*
 * Selector validators
 */

//! \ingroup Module
//! A class to validate a selection for Distance constraint operation
class PartSet_DistanceValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Module
//! A class to validate a selection for Length constraint operation
class PartSet_LengthValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Module
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_PerpendicularValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Module
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_ParallelValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Module
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_RadiusValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

//! \ingroup Module
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_RigidValidator : public ModuleBase_SelectionValidator
{
 public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection) const;
};

/**
* \ingroup Module
* A validator which checks that objects selected for feature attributes are different (not the same)
*/
class PartSet_DifferentObjectsValidator : public ModelAPI_RefAttrValidator
{
 public:
   /// Returns True if the feature is valid
   /// \param theFeature a feature to check
   /// \param theArguments a list of arguments (names of attributes to check)
   /// \param theObject a selected object
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const ObjectPtr& theObject) const;

  //! Returns true if the attribute is good for the feature attribute
  //! \param theFeature a feature to check
  //! \param theArguments a list of arguments (names of attributes to check)
  //! \param theAttribute an attribute
  virtual bool isValid(const FeaturePtr& theFeature, const std::list<std::string>& theArguments,
                       const AttributePtr& theAttribute) const;

  //! Returns true if the attribute is good for the feature attribute
  //! \param theAttribute an attribute
  //! \param theArguments a list of arguments (names of attributes to check)
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments) const;
};

/**
* \ingroup Module
* A Validator which validates tha selected object is a Sketch
*/
class PartSet_SketchValidator : public ModelAPI_ResultValidator
{
 public:
   /// Returns True if the given object is a sketch
   /// \param theObject an object
  virtual bool isValid(const ObjectPtr theObject) const;
};


#endif
