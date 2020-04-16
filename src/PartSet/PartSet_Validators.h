// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef PartSet_Validators_H
#define PartSet_Validators_H

#include "PartSet.h"

#include <ModuleBase_SelectionValidator.h>
#include <ModuleBase_ISelection.h>
#include <ModelAPI_AttributeValidator.h>

class GeomDataAPI_Point2D;

/*
 * Selector validators
 */


//! \ingroup Validators
//! A class to validate a selection for Distance constraint operation
class PartSet_DistanceSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Length constraint operation
class PartSet_LengthSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Perpendicular constraint operation
class PartSet_PerpendicularSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Parallel constraint operation
class PartSet_ParallelSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Radius constraint operation
class PartSet_RadiusSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Rigid constraint operation
class PartSet_RigidSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};


//! \ingroup Validators
//! A class to validate a selection for coincedence constraint operation
class PartSet_CoincidentSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Horizontal and Vertical constraints operation
class PartSet_HVDirSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Tangential constraints operation
class PartSet_TangentSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Fillet constraints operation
class PartSet_FilletSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Angle constraints operation
class PartSet_AngleSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Equal constraints operation
class PartSet_EqualSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Collinear constraints operation
class PartSet_CollinearSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Middle point constraints operation
class PartSet_MiddlePointSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Middle point constraints operation
class PartSet_MultyTranslationSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Middle point constraints operation
class PartSet_SplitSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for Middle point constraints operation
class PartSet_ProjectionSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
};

//! \ingroup Validators
//! A class to validate a selection for intersection operation
class PartSet_IntersectionSelection : public ModuleBase_SelectionValidator
{
public:
  PARTSET_EXPORT virtual bool isValid(const ModuleBase_ISelection* theSelection,
                                      ModuleBase_Operation* theOperation) const;
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
                       Events_InfoMessage& theError) const;
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
* A validator which checks that Point2D selected for feature attributes are different (not the same)
* It iterates by the feature ModelAPI_AttributeRefAttr attributes, finds GeomDataAPI_Point2D attribute in
* value or attribute of the attributes and if the point of the given attribute is geometrical equal to
* a point of another attribute, returns false
*/
class PartSet_DifferentPointsValidator : public ModelAPI_AttributeValidator
{
 public:
  //! Returns true if the attribute is good for the feature attribute
  //! \param theAttribute an attribute
  //! \param theArguments a list of arguments (names of attributes to check)
  //! \param theError an output error string
  virtual bool isValid(const AttributePtr& theAttribute,
                       const std::list<std::string>& theArguments,
                       Events_InfoMessage& theError) const;
private:
  //! Finds Point2D attribute by reference attribute. It might be:
  //! - COORD_ID attribute of SketchPlugin_Point if object
  //! - Attribute casted to point if attribute
  //! \param theAttribute an attribute
  //! \return point 2d attribute or NULL
  std::shared_ptr<GeomDataAPI_Point2D> getRefPointAttribute
                      (const AttributePtr& theAttribute) const;
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
                       Events_InfoMessage& theError) const;
};


#endif
