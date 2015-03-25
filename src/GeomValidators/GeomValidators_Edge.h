// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_Edge.h
// Created:     19 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_Edge_H
#define GeomValidators_Edge_H

#include "GeomValidators.h"
#include "ModelAPI_AttributeValidator.h"

#include <string>

/**
* \ingroup Validators
* A validator of selection
*/
class GeomValidators_Edge : public ModelAPI_AttributeValidator
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
   GEOMVALIDATORS_EXPORT GeomValidators_Edge() {}
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments) const;
protected:
  /// Convert string to TypeOfEdge value
  /// \param theType a string value
  static TypeOfEdge edgeType(const std::string& theType);
};

#endif
