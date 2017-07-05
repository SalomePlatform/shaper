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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef GeomAPI_Pln_H_
#define GeomAPI_Pln_H_

#include <memory>
#include <GeomAPI_Interface.h>

class GeomAPI_Ax3;
class GeomAPI_Pnt;
class GeomAPI_Dir;
class GeomAPI_Lin;

/**\class GeomAPI_Pln
 * \ingroup DataModel
 * \brief 3D point defined by three coordinates
 */

class GeomAPI_Pln : public GeomAPI_Interface
{
 public:
  /// Creation of plane by the axis placement
  GEOMAPI_EXPORT
  GeomAPI_Pln(const std::shared_ptr<GeomAPI_Ax3>& theAxis);

  /// Creation of plane by the point and normal
  GEOMAPI_EXPORT
  GeomAPI_Pln(const std::shared_ptr<GeomAPI_Pnt>& thePoint,
              const std::shared_ptr<GeomAPI_Dir>& theNormal);

  /// Creation of plane by coefficients (Ax+By+Cz+D=0)
  GEOMAPI_EXPORT
  GeomAPI_Pln(const double theA, const double theB, const double theC, const double theD);

  /// Returns a point of this plane
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> location() const;

  /// Returns a plane normal
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Dir> direction() const;

  /// Returns a plane x direction
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Dir> xDirection() const;

  /// Returns the plane coefficients (Ax+By+Cz+D=0)
  GEOMAPI_EXPORT
  void coefficients(double& theA, double& theB, double& theC, double& theD);

  /// Returns true if planes are coincident.
  GEOMAPI_EXPORT
  bool isCoincident(const std::shared_ptr<GeomAPI_Pln> thePlane, const double theTolerance = 1.e-7);

  /// Returns true if plane is parallel to theLine.
  GEOMAPI_EXPORT
  bool isParallel(const std::shared_ptr<GeomAPI_Lin> theLine);

  /// Returns intersection point or empty if no intersections
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> intersect(const std::shared_ptr<GeomAPI_Lin>& theLine) const;

  /// Returns projection of the given point onto the plane
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> project(const std::shared_ptr<GeomAPI_Pnt>& thePoint) const;

  /// \return distance between planes.
  GEOMAPI_EXPORT
  double distance(const std::shared_ptr<GeomAPI_Pln> thePlane) const;

  /// Translates the plane along direction theDir on distance theDist
  GEOMAPI_EXPORT
  void translate(const std::shared_ptr<GeomAPI_Dir> theDir, const double theDist);

  /// \return intersection line of two planes. Empty if they are parallel.
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Lin> intersect(const std::shared_ptr<GeomAPI_Pln> thePlane) const;
};

#endif

