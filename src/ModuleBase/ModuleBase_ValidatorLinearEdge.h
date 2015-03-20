// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ValidatorLinearEdge.h
// Created:     19 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ValidatorLinearEdge_H
#define ModuleBase_ValidatorLinearEdge_H

#include "ModuleBase.h"
#include "ModelAPI_AttributeValidator.h"

#include <StdSelect_TypeOfEdge.hxx>

/**
* \ingroup Validators
* A validator of selection
*/
class ModuleBase_ValidatorLinearEdge : public ModelAPI_AttributeValidator
{
 public:
  //  the edge type
  enum TypeOfEdge
  {
    AnyEdge,
    Line,
    Circle
  };

 public:
   MODULEBASE_EXPORT ModuleBase_ValidatorLinearEdge() {}
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  MODULEBASE_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                         const std::list<std::string>& theArguments) const;
protected:
  /// Convert string to StdSelect_TypeOfFace value
  /// \param theType a string value
  static TypeOfEdge edgeType(const std::string& theType);
};

#endif
