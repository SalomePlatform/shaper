// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomValidators_ShapeType.h
// Created:     19 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef GeomValidators_ShapeType_H
#define GeomValidators_ShapeType_H

#include "GeomValidators.h"
#include "ModelAPI_AttributeValidator.h"

#include <GeomAPI_Shape.h>

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
    Empty,
    Vertex,
    Edge,
    Line,
    Circle,
    Face,
    Solid,
    Compound,
    Plane,
    AnyShape
  };

 public:
   GEOMVALIDATORS_EXPORT GeomValidators_ShapeType() {}
  //! Returns true if attribute has shape type listed in the parameter arguments
  //! \param theAttribute the checked attribute
  //! \param theArguments arguments of the attribute
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const;
protected:
  /// Convert string to TypeOfShape value
  /// \param theType a string value
  static TypeOfShape shapeType(const std::string& theType);

  /// Returns true if the attibute's object type satisfies the argument value
  /// \param theAttribute a checked attribute
  /// \param theArgument a parameter
  bool isValidAttribute(const AttributePtr& theAttribute,
                        const TypeOfShape theShapeType,
                        std::string& theError) const;

  /// Returns true if the attibute's object type satisfies the argument value
  /// \param theAttribute a checked object
  /// \param theShapeType a shape type
  bool isValidObject(const ObjectPtr& theObject,
                     const TypeOfShape theShapeType,
                     std::string& theError) const;

  /// Returns true if the attibute's object type satisfies the argument value
  /// \param theShape a checked shape
  /// \param theShapeType a shape type
  bool isValidShape(const GeomShapePtr theShape,
                    const TypeOfShape theShapeType,
                    std::string& theError) const;

};

#endif
