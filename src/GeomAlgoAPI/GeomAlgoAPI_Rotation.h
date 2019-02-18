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

#ifndef GeomAlgoAPI_Rotation_H_
#define GeomAlgoAPI_Rotation_H_

#include <GeomAlgoAPI.h>
#include <GeomAlgoAPI_MakeShape.h>

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Shape.h>

/// \class GeomAlgoAPI_Rotation
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by rotating it around the axis.
class GeomAlgoAPI_Rotation : public GeomAlgoAPI_MakeShape
{
public:
  /// Type of rotation operation
  enum MethodType {
    BY_ANGLE, ///< Rotation by axis and an angle
    BY_POINTS ///< Rotation by a center and two points
  };

  /// \brief Creates an object which is obtained from current object by rotating it around the axis
  ///        with the angle.
  /// \param[in] theSourceShape  a shape to be rotated.
  /// \param[in] theAxis         rotation axis.
  /// \param[in] theAngle        rotation angle(in degree).
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                          double                         theAngle);

  /// \brief Creates an object which is obtained from current object by rotating it around the axis
  ///        withe angle using the center and two points.
  /// \param[in] theSourceShape  a shape to be rotated.
  /// \param[in] theCenterPoint  center point.
  /// \param[in] theStartPoint   start point.
  /// \param[in] theEndPoint     end point.
  GEOMALGOAPI_EXPORT GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                          std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                          std::shared_ptr<GeomAPI_Pnt>   theStartPoint,
                                          std::shared_ptr<GeomAPI_Pnt>   theEndPoint);

  /// Checks if data for the translation execution is OK.
  GEOMALGOAPI_EXPORT bool check();

  /// Execute the translation.
  GEOMALGOAPI_EXPORT void build();

private:
  MethodType myMethodType; /// Type of method used.
  std::shared_ptr<GeomAPI_Shape> mySourceShape; /// Shape to be rotated.
  std::shared_ptr<GeomAPI_Ax1> myAxis; /// Rotation axis.
  double myAngle; /// Rotation angle.
  std::shared_ptr<GeomAPI_Pnt> myCenterPoint; /// Rotation center point.
  std::shared_ptr<GeomAPI_Pnt> myStartPoint; /// Rotation start point.
  std::shared_ptr<GeomAPI_Pnt> myEndPoint; /// Rotation end point.
};

#endif
