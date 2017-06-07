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

#ifndef GeomAlgoAPI_Circ2dBuilder_H_
#define GeomAlgoAPI_Circ2dBuilder_H_

#include <GeomAlgoAPI.h>

#include <memory>
#include <vector>

class GeomAPI_Ax3;
class GeomAPI_Circ2d;
class GeomAPI_Pnt2d;
class GeomAPI_Shape;

/// \class GeomAlgoAPI_Circ2dBuilder
/// \ingroup DataAlgo
/// \brief Creates circle in 2D space satisfying combination of the following constraints:
///        * center of a circle
///        * passing through the point
///        * tangent to a curve
///        * fixed radius
class GeomAlgoAPI_Circ2dBuilder
{
public:
  /// \brief Create a builder object.
  ///        Constraints should be applied separately.
  /// \param thePlane [in]  plane to project tangent curves
  GEOMALGOAPI_EXPORT
  GeomAlgoAPI_Circ2dBuilder(const std::shared_ptr<GeomAPI_Ax3>& thePlane);

  /// \brief Set fixed radius of the circle
  GEOMALGOAPI_EXPORT
  void setRadius(const double theRadius);

  /// \brief Set fixed center of the circle
  GEOMALGOAPI_EXPORT
  void setCenter(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter);

  /// \brief Constrain circle to be tangent to the given edge
  GEOMALGOAPI_EXPORT
  void addTangentCurve(const std::shared_ptr<GeomAPI_Shape>& theEdge);

  /// \brief Constrain circle to pass through the given point
  GEOMALGOAPI_EXPORT
  void addPassingPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// \brief Optional constraint to find circle closest to the given point
  GEOMALGOAPI_EXPORT
  void setClosestPoint(const std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// \brief Build circle
  GEOMALGOAPI_EXPORT
  std::shared_ptr<GeomAPI_Circ2d> circle();

  /// \brief Create a circle passing through three points
  GEOMALGOAPI_EXPORT
  static std::shared_ptr<GeomAPI_Circ2d>
      circle(const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
             const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint,
             const std::shared_ptr<GeomAPI_Pnt2d>& theThirdPoint);

private:
  std::shared_ptr<GeomAPI_Ax3>                  myPlane;
  std::shared_ptr<GeomAPI_Pnt2d>                myCenter;
  std::vector< std::shared_ptr<GeomAPI_Pnt2d> > myPassingPoints;
  std::vector< std::shared_ptr<GeomAPI_Shape> > myTangentShapes;
  std::shared_ptr<GeomAPI_Pnt2d>                myClosestPoint;
  double                                        myRadius;
};

#endif
