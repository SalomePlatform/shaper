// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_ValidatorLinearEdgeOrVertex.h
// Created:     19 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef ModuleBase_ValidatorLinearEdgeOrVertex_H
#define ModuleBase_ValidatorLinearEdgeOrVertex_H

#include "ModuleBase.h"
#include "ModelAPI_AttributeValidator.h"

#include <StdSelect_TypeOfEdge.hxx>

/**
* \ingroup Validators
* A validator of selection
*/
class ModuleBase_ValidatorLinearEdgeOrVertex : public ModelAPI_AttributeValidator
{
 public:
   MODULEBASE_EXPORT ModuleBase_ValidatorLinearEdgeOrVertex() {}
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  MODULEBASE_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                         const std::list<std::string>& theArguments) const;
};

#endif
