// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GeomAPI_Pnt2d_H_
#define GeomAPI_Pnt2d_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_XY;
class GeomAPI_Pnt;
class GeomAPI_Dir;

/**\class GeomAPI_Pnt2d
 * \ingroup DataModel
 * \brief 2D point defined by two coordinates
 */

class GeomAPI_Pnt2d : public GeomAPI_Interface
{
 public:
  /// Creation of point by coordinates
  GEOMAPI_EXPORT
  GeomAPI_Pnt2d(const double theX, const double theY);
  /// Creation of point by coordinates
  GEOMAPI_EXPORT
  GeomAPI_Pnt2d(const std::shared_ptr<GeomAPI_XY>& theCoords);

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

  /// Returns the 3D point
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> to3D(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                                      const std::shared_ptr<GeomAPI_Dir>& theDirX,
                                      const std::shared_ptr<GeomAPI_Dir>& theDirY);

  /// returns coordinates of the point
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_XY> xy();

  /// Distance between two points
  GEOMAPI_EXPORT
  double distance(const std::shared_ptr<GeomAPI_Pnt2d>& theOther) const;

  /// Returns whether the distance between two points is less then precision confusion
  GEOMAPI_EXPORT
  bool isEqual(const std::shared_ptr<GeomAPI_Pnt2d>& theOther) const;
};

#endif

