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
#include <GeomAlgoAPI_Transform.h>

class GeomAPI_Ax1;
class GeomAPI_Pnt;

/// \class GeomAlgoAPI_Rotation
/// \ingroup DataAlgo
/// \brief Creates a copy of the object by rotating it around the axis.
class GeomAlgoAPI_Rotation : public GeomAlgoAPI_Transform
{
public:
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
};

#endif
