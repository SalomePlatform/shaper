// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

// File:        GeomAPI_Ellipse.h
// Created:     25 April 2017
// Author:      Vitaly Smetannikov

#ifndef GeomAPI_Ellipse_H_
#define GeomAPI_Ellipse_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Ax2;
class GeomAPI_Curve;
class GeomAPI_Dir;
class GeomAPI_Pnt;


/**\class GeomAPI_Ellipse
 * \ingroup DataModel
 * \brief Ellipse in 3D
 */
class GeomAPI_Ellipse : public GeomAPI_Interface
{
public:

  /// \brief Constructs an epty ellipse
  GEOMAPI_EXPORT GeomAPI_Ellipse() : GeomAPI_Interface() {}

  /** \brief Constructs an ellipse with major and minor radiuses,
   *  where theAx2 locates the ellipse and defines its orientation in 3D space such that:\n
   *  - the center of the circle is the origin of theAx2;\n
   *  - the origin, "X Direction" and "Y Direction" of theAx2 define the plane of the circle;\n
   *  - theAx2 is the local coordinate system of the circle.\n
   *    Note: It is possible to create a circle where Radius is equal to 0.0. raised if Radius < 0.
   */
  GEOMAPI_EXPORT GeomAPI_Ellipse(const std::shared_ptr<GeomAPI_Ax2>& theAx2,
                                 double theMajorRadius, double theMinorRadius);

  GEOMAPI_EXPORT GeomAPI_Ellipse(std::shared_ptr<GeomAPI_Curve> theCurve);

  /// Returns center of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> center() const;

  /// Returns first focus of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> firstFocus() const;

  /// Returns second focus of the ellipse
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Pnt> secondFocus() const;

  /// Return orthogonal direction to the ellipse's plane
  GEOMAPI_EXPORT std::shared_ptr<GeomAPI_Dir> normal() const;

  /// Returns minor radius of the ellipse
  GEOMAPI_EXPORT double minorRadius() const;

  /// Returns major radius of the ellipse
  GEOMAPI_EXPORT double majorRadius() const;

  /// Project point on ellipse
  GEOMAPI_EXPORT const std::shared_ptr<GeomAPI_Pnt> project(
      const std::shared_ptr<GeomAPI_Pnt>& thePoint) const;

  /** \brief Computes the parameter of a given point on an ellipse. The point must be
   *         located either on the circle itself or relatively to the latter
   *         at a distance less than the tolerance value. Return FALSE if the point
   *         is beyond the tolerance limit or if computation fails.
   *         Max Tolerance value is currently limited to 1.e-4
   *  \param[in]  thePoint point of origin.
   *  \param[in]  theTolerance tolerance of computation.
   *  \param[out] theParameter resulting parameter.
   */
  GEOMAPI_EXPORT const bool parameter(const std::shared_ptr<GeomAPI_Pnt> thePoint,
                                      const double theTolerance,
                                      double& theParameter) const;
};

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_Ellipse> GeomEllipsePtr;

#endif
