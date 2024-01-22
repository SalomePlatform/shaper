// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef GeomAPI_Circ2d_H_
#define GeomAPI_Circ2d_H_

#include <GeomAPI_Interface.h>
#include <memory>

class GeomAPI_Ax3;
class GeomAPI_Pnt2d;
class GeomAPI_Dir2d;
class GeomAPI_Shape;

/**\class GeomAPI_Circ2d
 * \ingroup DataModel
 * \brief Circle in 2D
 */

class GeomAPI_Circ2d : public GeomAPI_Interface
{
public:
  /// Creation of circle defined by center point and circle radius
  GEOMAPI_EXPORT
  GeomAPI_Circ2d(const double theCenterX, const double theCenterY, const double theRadius);

  /// Creation of circle defined by center point and circle radius
  GEOMAPI_EXPORT
  GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                 const std::shared_ptr<GeomAPI_Pnt2d>& theCirclePoint);

  /// Creation of circle defined by center point, direction and circle radius
  GEOMAPI_EXPORT
  GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                 const std::shared_ptr<GeomAPI_Dir2d>& theDir, double theRadius);

  /// Return center of the circle
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_Pnt2d> center() const;

  /// Return radius of the circle
  GEOMAPI_EXPORT
  double radius() const;

  /// Project point on line
  GEOMAPI_EXPORT
  const std::shared_ptr<GeomAPI_Pnt2d> project(
      const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) const;

  /** \brief Computes the parameter of a given point on a circle. The point must be
   *         located either on the circle itself or relatively to the latter
   *         at a distance less than the tolerance value. Return FALSE if the point
   *         is beyond the tolerance limit or if computation fails.
   *         Max Tolerance value is currently limited to 1.e-4
   *  \param[in] thePoint point of origin.
   *  \param[in] theTolerance tolerance of computation.
   *  \param[out] theParameter resulting parameter.
   */
  GEOMAPI_EXPORT const bool parameter(const std::shared_ptr<GeomAPI_Pnt2d> thePoint,
                                      const double theTolerance,
                                      double& theParameter) const;

  /** \brief Returns in thePoint the point of parameter theU.
   *  P = C + R * Cos (U) * XDir + R * Sin (U) * YDir where C is the center of the circle,
   *  XDir the XDirection and YDir the YDirection of the circle's local coordinate system.
   *  \param[in] theU parameter.
   *  \param[out] thePoint resulting point.
   */
  GEOMAPI_EXPORT void D0(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint);
};

#endif

