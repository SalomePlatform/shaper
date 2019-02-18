// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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
    Shell,
    Solid,
    CompSolid,
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
                                             Events_InfoMessage& theError) const;
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
                        Events_InfoMessage& theError) const;

  /// Returns true if the attibute's object type satisfies the argument value
  /// \param[in] theObject a checked object
  /// \param[in] theShapeType a shape type
  /// \param[out] theError error message.
  bool isValidObject(const ObjectPtr& theObject,
                     const TypeOfShape theShapeType,
                     const bool theIsGeometricalSelection,
                     Events_InfoMessage& theError) const;

  /// Returns true if the attibute's object type satisfies the argument value
  /// \param[in] theShape a checked shape
  /// \param[in] theShapeType a shape type
  /// \param[out] theError error message.
  bool isValidShape(const GeomShapePtr theShape,
                    const TypeOfShape theShapeType,
                    const bool theIsGeometricalSelection,
                    Events_InfoMessage& theError) const;

};

#endif
