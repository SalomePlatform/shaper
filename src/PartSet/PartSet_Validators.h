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
class PartSet_DistanceSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Length constraint operation
class PartSet_LengthSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_PerpendicularSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Parallel constraint operation
class PartSet_ParallelSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Radius constraint operation
class PartSet_RadiusSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Rigid constraint operation
class PartSet_RigidSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};


//! \ingroup Validators
//! A class to validate a selection for coincedence constraint operation
class PartSet_CoincidentSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Horizontal and Vertical constraints operation
class PartSet_HVDirSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Tangential constraints operation
class PartSet_TangentSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Fillet constraints operation
class PartSet_FilletSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Angle constraints operation
class PartSet_AngleSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Equal constraints operation
class PartSet_EqualSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection, ModuleBase_Operation* theOperation) const;
};

////////////// Attribute validators ////////////////


/**
* \ingroup Validators
* A validator which checks that objects selected for feature attributes are different (not the same)
*/
class PartSet_DifferentObjectsValidator : public ModelAPI_AttributeValidator
{
  //! Validator possible error types
  enum ErrorType {
    EqualObjects,
    EqualAttributes,
    EqualShapes,
    EmptyShapes
  };
 public:
  //! Returns true if the attribute is good for the feature attribute
  //! \param theAttribute an attribute
  //! \param theArguments a list of arguments (names of attributes to check)
  //! \param theError an output error string
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
private:
  //! Returns error message for the error type
  //! \param theType a type of error
  //! \param thEqualObjectInfo an 
  std::string errorMessage(const PartSet_DifferentObjectsValidator::ErrorType& theType,
                           const std::string& thEqualObject, const std::string& theFirstAttribute,
                           const std::string& theSecondAttribute) const;

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
  //! \param theError an output error string
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};

/**\class PartSet_CoincidentAttr
 * \ingroup Validators
 * \brief Validator to check whether there is a coincident constraint between
 * the attribute and attribute of argument.
 */
class PartSet_CoincidentAttr : public ModelAPI_AttributeValidator
{
 public:
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  //! \param theError an output error string
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       std::string& theError) const;
};


#endif
