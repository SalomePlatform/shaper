// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_EdgeOrVertex.h
// Created:     19 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_EdgeOrVertex_H
#define GeomValidators_EdgeOrVertex_H

#include "GeomValidators.h"
#include "ModelAPI_AttributeValidator.h"

#include <StdSelect_TypeOfEdge.hxx>

/**
* \ingroup Validators
* A validator of selection
*/
class GeomValidators_EdgeOrVertex : public ModelAPI_AttributeValidator
{
 public:
   GEOMVALIDATORS_EXPORT GeomValidators_EdgeOrVertex() {}
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments) const;
};

#endif
