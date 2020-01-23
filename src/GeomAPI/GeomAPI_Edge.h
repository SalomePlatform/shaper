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

#ifndef GeomAPI_Edge_H_
#define GeomAPI_Edge_H_

#include <GeomAPI_Shape.h>

class GeomAPI_Pln;
class GeomAPI_Pnt;
class GeomAPI_Circ;
class GeomAPI_Lin;
class GeomAPI_Ellipse;
class GeomAPI_Vertex;

/**\class GeomAPI_Edge
* \ingroup DataModel
 * \brief Interface to the edge object
 */

class GeomAPI_Edge : public GeomAPI_Shape
{
public:
   /// Makes an undefined Edge (no geometry).
  GEOMAPI_EXPORT
   GeomAPI_Edge();

   /// Creation of edge by the edge-shape
  GEOMAPI_EXPORT
   GeomAPI_Edge(const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Return vertices of the edge;
  GEOMAPI_EXPORT
  void vertices(std::shared_ptr<GeomAPI_Vertex>& theStartVertex,
                std::shared_ptr<GeomAPI_Vertex>& theEndVertex) const;

  /// Returns \c true if edges have same underlying curve
  GEOMAPI_EXPORT
  virtual bool isSameGeometry(const std::shared_ptr<GeomAPI_Shape> theShape) const;

  /// Verifies that the edge is a line
  GEOMAPI_EXPORT
  bool isLine() const;

  /// Verifies that the edge is a circle
  GEOMAPI_EXPORT
  bool isCircle() const;

  /// Verifies that the edge is an arc of circle
  GEOMAPI_EXPORT
  bool isArc() const;

  /// Verifies that the edge is an arc of circle
  GEOMAPI_EXPORT
  bool isEllipse() const;

  /// Verifies that the edge is based on a B-spline curve
  GEOMAPI_EXPORT
  bool isBSpline() const;

  /// Returns the first vertex coordinates of the edge
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> firstPoint();

  /// Returns the Last vertex coordinates of the edge
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Pnt> lastPoint();

  /// Returns a circle if edge is based on the circle curve
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Circ> circle() const;

  /// Returns an ellipse if edge is based on the ellipse curve
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Ellipse> ellipse() const;

  /// Returns a line if edge is based on the linear curve
  GEOMAPI_EXPORT
  std::shared_ptr<GeomAPI_Lin> line() const;

  /// Returns true if the current edge is geometrically equal to the given edge
  GEOMAPI_EXPORT
  bool isEqual(const std::shared_ptr<GeomAPI_Shape> theEdge) const;

  /// Change parametric range of the curve
  GEOMAPI_EXPORT
  void setRange(const double& theFirst, const double& theLast);

  /// Returns range of parameter on the curve
  GEOMAPI_EXPORT
  void getRange(double& theFirst, double& theLast) const;

  /// Returns true, if the edge is fully placed in the specified plane
  /// \param thePlane a plane for intersection
  GEOMAPI_EXPORT
  bool isInPlane(const std::shared_ptr<GeomAPI_Pln> thePlane) const;

  /// Returns list of intersection points if the edge has intersections with the given plane
  /// \param thePlane a plane for intersection
  GEOMAPI_EXPORT
  void intersectWithPlane(const std::shared_ptr<GeomAPI_Pln> thePlane,
                          std::list<std::shared_ptr<GeomAPI_Pnt> >& theResult) const;

  /// Returns edge length.
  GEOMAPI_EXPORT
  double length() const;

  /// Returns true if the edge is closed (like full circle)
  GEOMAPI_EXPORT
  bool isClosed() const;

  /// Returns true if the edge is degenerated (has no 3D curve)
  GEOMAPI_EXPORT
  bool isDegenerated() const;

  GEOMAPI_EXPORT
  void setFirstPointTolerance(const double theTolerance);

  GEOMAPI_EXPORT
  void setLastPointTolerance(const double theTolerance);

  /// Return middle point on the edge
  GEOMAPI_EXPORT
  virtual std::shared_ptr<GeomAPI_Pnt> middlePoint() const;
};

//! Pointer on object
typedef std::shared_ptr<GeomAPI_Edge> GeomEdgePtr;

#endif

