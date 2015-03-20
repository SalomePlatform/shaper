// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ValidatorFace.h
// Created:     20 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ValidatorFace_H
#define ModuleBase_ValidatorFace_H

#include "ModuleBase.h"
#include "ModelAPI_AttributeValidator.h"

#include <GeomAbs_SurfaceType.hxx>

/**
* \ingroup Validators
* A validator of selection
*/
class ModuleBase_ValidatorFace : public ModelAPI_AttributeValidator
{
 public:
   MODULEBASE_EXPORT ModuleBase_ValidatorFace() {}
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  MODULEBASE_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                         const std::list<std::string>& theArguments) const;
protected:
  /// Convert string to TypeOfFace value
  /// \param theType a string value
  static GeomAbs_SurfaceType faceType(const std::string& theType);
};

#endif
