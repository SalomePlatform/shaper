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

#ifndef GeomAlgoAPI_EdgeBuilder_H_
#define GeomAlgoAPI_EdgeBuilder_H_

#include <GeomAlgoAPI.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Circ.h>
#include <memory>
#include <vector>

class GeomAPI_Ax3;
class GeomAPI_BSpline2d;

/**\class GeomAlgoAPI_EdgeBuilder
 * \ingroup DataAlgo
 * \brief Allows to create face-shapes by different parameters
 */

class GEOMALGOAPI_EXPORT GeomAlgoAPI_EdgeBuilder
{
 public:
  /// Creates linear edge by two points.
  /// \param theStart a first point of an edge
  /// \param theEnd an end point of an edge
  static std::shared_ptr<GeomAPI_Edge> line(std::shared_ptr<GeomAPI_Pnt> theStart,
                                            std::shared_ptr<GeomAPI_Pnt> theEnd);

  /// Creates linear edge by three dimensions.
  /// \param theX the X dimension
  /// \param theY the Y dimension
  /// \param theZ the Z dimension
  static std::shared_ptr<GeomAPI_Edge> line(double theDX,
                                            double theDY,
                                            double theDZ);

  /// Creates linear edge by GeomAPI_Lin.
  /// \param theLin line.
  static std::shared_ptr<GeomAPI_Edge> line(const std::shared_ptr<GeomAPI_Lin> theLin);

  /// Creates edge - axis of the given cylindrical face. The result axis edge is infinite
  static std::shared_ptr<GeomAPI_Edge> cylinderAxis(
    std::shared_ptr<GeomAPI_Shape> theCylindricalFace);

  /// Creates linear edge in a form of a circle by a point and a circle radius
  static std::shared_ptr<GeomAPI_Edge> lineCircle(std::shared_ptr<GeomAPI_Pnt> theCenter,
                                                    std::shared_ptr<GeomAPI_Dir> theNormal,
                                                    double theRadius);

  /// Creates linear edge in a form of a circle by GeomAPI_Circle
  static std::shared_ptr<GeomAPI_Edge> lineCircle(std::shared_ptr<GeomAPI_Circ> theCircle);

  /// Creates linear edge in a form of a circle arc by a three points
  static std::shared_ptr<GeomAPI_Edge> lineCircleArc(std::shared_ptr<GeomAPI_Pnt> theCenter,
                                                       std::shared_ptr<GeomAPI_Pnt> theStartPoint,
                                                       std::shared_ptr<GeomAPI_Pnt> theEndPoint,
                                                       std::shared_ptr<GeomAPI_Dir> theNormal);

  /// Creates elliptic edge
  static std::shared_ptr<GeomAPI_Edge> ellipse(const std::shared_ptr<GeomAPI_Pnt>& theCenter,
                                               const std::shared_ptr<GeomAPI_Dir>& theNormal,
                                               const std::shared_ptr<GeomAPI_Dir>& theMajorAxis,
                                               const double                        theMajorRadius,
                                               const double                        theMinorRadius);


  /// Creates elliptic edge
  static std::shared_ptr<GeomAPI_Edge> ellipticArc(
      const std::shared_ptr<GeomAPI_Pnt>& theCenter,
      const std::shared_ptr<GeomAPI_Dir>& theNormal,
      const std::shared_ptr<GeomAPI_Dir>& theMajorAxis,
      const double                        theMajorRadius,
      const double                        theMinorRadius,
      const std::shared_ptr<GeomAPI_Pnt>& theStart,
      const std::shared_ptr<GeomAPI_Pnt>& theEnd);

  /// Creates planar B-spline edge
  static GeomEdgePtr bsplineOnPlane(const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                    const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
                                    const std::list<double>& theWeights,
                                    const std::list<double>& theKnots,
                                    const std::list<int>& theMults,
                                    const int theDegree,
                                    const bool thePeriodic);

  /// Creates planar B-spline edge
  static GeomEdgePtr bsplineOnPlane(const std::shared_ptr<GeomAPI_Ax3>& thePlane,
                                    const std::shared_ptr<GeomAPI_BSpline2d>& theCurve);
};

#endif
