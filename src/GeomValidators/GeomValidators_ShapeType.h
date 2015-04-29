// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_ShapeType.h
// Created:     19 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_ShapeType_H
#define GeomValidators_ShapeType_H

#include "GeomValidators.h"
#include "ModelAPI_AttributeValidator.h"

#include <string>

/**
* \ingroup Validators
* A validator for shape types, such as vertex, line, circe or arc.
* If there are some argument parameters, this validator returns true if the attribute satisfied
* at least one argument (OR combination of arguments).
*/
class GeomValidators_ShapeType : public ModelAPI_AttributeValidator
{
 public:
  //  the edge type
  enum TypeOfShape
  {
    AnyShape,
    Vertex,
    Line,
    Circle
  };

 public:
   GEOMVALIDATORS_EXPORT GeomValidators_ShapeType() {}
  //! returns true if attribute is valid
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments) const;
protected:
  /// Convert string to TypeOfShape value
  /// \param theType a string value
  static TypeOfShape shapeType(const std::string& theType);

  bool isValidArgument(const AttributePtr& theAttribute,
                       const std::string& theArgument) const;

};

#endif
