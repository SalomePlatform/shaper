// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef GeomAPI_XY_H_
#define GeomAPI_XY_H_

#include <GeomAPI_Interface.h>
#include <memory>

/**\class GeomAPI_XY
 * \ingroup DataModel
 * \brief 2 coordinates: they may represent vector or point or something else
 */

class GeomAPI_XY : public GeomAPI_Interface
{
 public:
  /// Creation by coordinates
  GEOMAPI_EXPORT
  GeomAPI_XY(const double theX, const double theY);

  /// returns X coordinate
  GEOMAPI_EXPORT
  double x() const;
  /// returns Y coordinate
  GEOMAPI_EXPORT
  double y() const;

  /// sets X coordinate
  GEOMAPI_EXPORT
  void setX(const double theX);
  /// sets Y coordinate
  GEOMAPI_EXPORT
  void setY(const double theY);

  /// result is sum of coordinates of this and the given argument
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_XY> added(const std::shared_ptr<GeomAPI_XY>& theArg);
  /// result is difference between coordinates of this and the given argument
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_XY> decreased(const std::shared_ptr<GeomAPI_XY>& theArg);
  /// result is coordinates multiplied by the argument
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_XY> multiplied(const double theArg);

  /// result is a scalar product of two triplets
  GEOMAPI_EXPORT
  double dot(const std::shared_ptr<GeomAPI_XY>& theArg) const;
  /// result is a cross product of two triplets
  GEOMAPI_EXPORT
  double cross(const std::shared_ptr<GeomAPI_XY>& theArg) const;

  /// Distance between two pairs
  GEOMAPI_EXPORT
  double distance(const std::shared_ptr<GeomAPI_XY>& theOther) const;
};

#endif

