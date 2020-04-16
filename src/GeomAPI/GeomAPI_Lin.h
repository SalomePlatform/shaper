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

#ifndef GeomAPI_Lin_H_
#define GeomAPI_Lin_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Dir;
class GeomAPI_Pnt;

/**\class GeomAPI_Lin
 * \ingroup DataModel
 * \brief Line in 3D
 */

class GeomAPI_Lin : public GeomAPI_Interface
{
 public:
  /// Creation of line defined by cordinates of start and end points
  GEOMAPI_EXPORT
  GeomAPI_Lin(const double theStartX, const double theStartY, const double theStartZ,
              const double theEndX, const double theEndY, const double theEndZ);
  /// Creation of line defined by start and end points
  GEOMAPI_EXPORT
  GeomAPI_Lin(const std::shared_ptr<GeomAPI_Pnt>& theStart,
              const std::shared_ptr<GeomAPI_Pnt>& theEnd);
  /// Creation of line defined by origin and direction
  GEOMAPI_EXPORT
  GeomAPI_Lin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
              const std::shared_ptr<GeomAPI_Dir>& theDirection);

  /// Returns point on the line (first point)
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> location();

  /// Returns a line direction
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Dir> direction();

  /// Distance between two points
  GEOMAPI_EXPORT
  double distance(const std::shared_ptr<GeomAPI_Pnt>& thePoint) const;
  /// Intersection of two lines
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_Pnt> intersect(
      const std::shared_ptr<GeomAPI_Lin>& theLine) const;
  /// Project point on line
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_Pnt> project(
      const std::shared_ptr<GeomAPI_Pnt>& thePoint) const;

  /// Returns parameter of the point projection
  GEOMAPI_EXPORT
  double projParam(const std::shared_ptr<GeomAPI_Pnt>& thePoint) const;

  /// \return true if this line contains thePoint, that is,
  /// if the distance between thePoint and this line
  ///         is less than or equal to theLinearTolerance.
  GEOMAPI_EXPORT
  bool contains(const std::shared_ptr<GeomAPI_Pnt> thePoint,
                const double theLinearTolerance = 1.e-7) const;

  /// \return true if lines are parallel.
  GEOMAPI_EXPORT
  bool isParallel(const std::shared_ptr<GeomAPI_Lin> theLin) const;

  /// \return true if lines are coplanar.
  GEOMAPI_EXPORT
  bool isCoplanar(const std::shared_ptr<GeomAPI_Lin> theLin) const;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Lin> GeomLinePtr;

#endif

