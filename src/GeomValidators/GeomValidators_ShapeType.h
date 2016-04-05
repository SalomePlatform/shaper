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
  /// Type of shape
  enum TypeOfShape
  {
    Empty,
    Vertex,
    Edge,
    Line,
    Circle,
    Wire,
    Face,
    Plane,
    Solid,
    Compound,
    AnyShape
  };

 public:
   GEOMVALIDATORS_EXPORT GeomValidators_ShapeType() {}
  //! Returns true if attribute has shape type listed in the parameter arguments
  //! \param[in] theAttribute the checked attribute
  //! \param[in] theArguments arguments of the attribute
  //! \param[out] theError error message.
  GEOMVALIDATORS_EXPORT virtual bool isValid(const AttributePtr& theAttribute,
                                             const std::list<std::string>& theArguments,
                                             std::string& theError) const;
protected:
  /// Convert string to TypeOfShape value
  /// \param theType a string value
  static TypeOfShape shapeType(const std::string& theType);

  /// Returns true if the attibute's object type satisfies the argument value
  /// \param[in] theAttribute a checked attribute
  /// \param[in] theShapeType a type of shape
  /// \param[out] theError error message.
  bool isValidAttribute(const AttributePtr& theAttribute,
                        const TypeOfShape theShapeType,
                        std::string& theError) const;

  /// Returns true if the attibute's object type satisfies the argument value
  /// \param[in] theObject a checked object
  /// \param[in] theShapeType a shape type
  /// \param[out] theError error message.
  bool isValidObject(const ObjectPtr& theObject,
                     const TypeOfShape theShapeType,
                     std::string& theError) const;

  /// Returns true if the attibute's object type satisfies the argument value
  /// \param[in] theShape a checked shape
  /// \param[in] theShapeType a shape type
  /// \param[out] theError error message.
  bool isValidShape(const GeomShapePtr theShape,
                    const TypeOfShape theShapeType,
                    std::string& theError) const;

};

#endif
