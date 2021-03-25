// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#ifndef GeomAPI_Dir2d_H_
#define GeomAPI_Dir2d_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_XY;

/**\class GeomAPI_Dir2d
 * \ingroup DataModel
 * \brief 2D direction defined by three normalized coordinates
 */

class GeomAPI_Dir2d : public GeomAPI_Interface
{
 public:
  /// Creation of direction by coordinates
  GEOMAPI_EXPORT
  GeomAPI_Dir2d(const double theX, const double theY);
  /// Creation of direction by coordinates
  GEOMAPI_EXPORT
  GeomAPI_Dir2d(const std::shared_ptr<GeomAPI_XY>& theCoords);

  /// returns X coordinate
  GEOMAPI_EXPORT
  double x() const;
  /// returns Y coordinate
  GEOMAPI_EXPORT
  double y() const;

  /// returns coordinates of the direction
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_XY> xy();

  /// inverses the direction
  GEOMAPI_EXPORT void reverse();

  /// result is a scalar product of directions
  GEOMAPI_EXPORT
  double dot(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const;
  /// result is a cross product of two directions
  GEOMAPI_EXPORT
  double cross(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const;

  /// calculates angle between two directions
  GEOMAPI_EXPORT
  double angle(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Dir2d> GeomDir2dPtr;

#endif

