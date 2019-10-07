// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

// File:        GeomAPI_Ellipse2d.h
// Created:     26 April 2017
// Author:      Artem ZHIDKOV

#ifndef GeomAPI_Ellipse2d_H_
#define GeomAPI_Ellipse2d_H_

#include <GeomAPI_Interface.h>

class GeomAPI_Circ2d;
class GeomAPI_Dir2d;
class GeomAPI_Lin2d;
class GeomAPI_Pnt2d;

/**\class GeomAPI_Ellipse2d
 * \ingroup DataModel
 * \brief Ellipse in 2D
 */
class GeomAPI_Ellipse2d : public GeomAPI_Interface
{
public:
  /// \brief Constructs ellipse by center, X-axis and given radii
  GEOMAPI_EXPORT GeomAPI_Ellipse2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                   const std::shared_ptr<GeomAPI_Dir2d>& theXAxis,
                                   const double theMajorRadius,
                                   const double theMinorRadius);

  /// \brief Constructs ellipse by center and two points lying on the ellipse:
  ///        first of them defines an axis of the ellipse
  ///        and another is just placed on the ellipse.
  GEOMAPI_EXPORT GeomAPI_Ellipse2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& theAxisPoint,
                                   const std::shared_ptr<GeomAPI_Pnt2d>& thePassingPoint);

  /// Returns center of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt2d> center() const;

  /// Returns first focus of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt2d> firstFocus() const;

  /// Returns second focus of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt2d> secondFocus() const;

  /// Returns minor radius of the ellipse
  GEOMAPI_EXPORT double minorRadius() const;

  /// Returns major radius of the ellipse
  GEOMAPI_EXPORT double majorRadius() const;

  /// Project point on ellipse
  GEOMAPI_EXPORT const std::shared_ptr<GeomAPI_Pnt2d> project(
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) const;

  /** \brief Computes the parameter of a given point on an ellipse. The point must be
   *         located either on the circle itself or relatively to the latter
   *         at a distance less than the tolerance value. Return FALSE if the point
   *         is beyond the tolerance limit or if computation fails.
   *         Max Tolerance value is currently limited to 1.e-4
   *  \param[in]  thePoint point of origin.
   *  \param[in]  theTolerance tolerance of computation.
   *  \param[out] theParameter resulting parameter.
   */
  GEOMAPI_EXPORT const bool parameter(const std::shared_ptr<GeomAPI_Pnt2d> thePoint,
                                      const double theTolerance,
                                      double& theParameter) const;

  /// Calculate minimal distance between the ellipse and a line.
  /// Return corresponding points on the ellipse and on the line.
  GEOMAPI_EXPORT double distance(const std::shared_ptr<GeomAPI_Lin2d>& theLine,
                                 std::shared_ptr<GeomAPI_Pnt2d>& thePointOnMe,
                                 std::shared_ptr<GeomAPI_Pnt2d>& thePointOnLine);

  /// Calculate minimal distance between the ellipse and a circle.
  /// Return corresponding points on the ellipse and on the circle.
  GEOMAPI_EXPORT double distance(const std::shared_ptr<GeomAPI_Circ2d>& theCircle,
                                 std::shared_ptr<GeomAPI_Pnt2d>& thePointOnMe,
                                 std::shared_ptr<GeomAPI_Pnt2d>& thePointOnCircle);

  /// Calculate minimal distance between two ellipses.
  /// Return corresponding points on the ellipses.
  GEOMAPI_EXPORT double distance(const std::shared_ptr<GeomAPI_Ellipse2d>& theEllipse,
                                 std::shared_ptr<GeomAPI_Pnt2d>& thePointOnMe,
                                 std::shared_ptr<GeomAPI_Pnt2d>& thePointOnEllipse);
};

#endif
