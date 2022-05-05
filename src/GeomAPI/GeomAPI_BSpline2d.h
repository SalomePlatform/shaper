// Copyright (C) 2019-2022  CEA/DEN, EDF R&D
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

#ifndef GeomAPI_BSpline2d_H_
#define GeomAPI_BSpline2d_H_

#include <GeomAPI_Interface.h>

#include <list>
#include <memory>

class GeomAPI_Pnt2d;
class GeomAPI_XY;

/** \class GeomAPI_BSpline2d
 *  \ingroup DataModel
 *  \brief B-spline curve in 2D
 */
class GeomAPI_BSpline2d : public GeomAPI_Interface
{
public:
  /// Creation of B-spline curve defined by list of poles and weights
  GEOMAPI_EXPORT GeomAPI_BSpline2d(const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
                                   const std::list<double>& theWeights,
                                   const bool thePeriodic = false);

  /// Creation of B-spline curve defined by list of poles and weights
  GEOMAPI_EXPORT GeomAPI_BSpline2d(const int theDegree,
                                   const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
                                   const std::list<double>& theWeights,
                                   const std::list<double>& theKnots = std::list<double>(),
                                   const std::list<int>& theMults = std::list<int>(),
                                   const bool thePeriodic = false);

  /// Returns true if curve is not initialized
  GEOMAPI_EXPORT bool isNull() const;

  /// Returns degree of the curve
  GEOMAPI_EXPORT int degree() const;

  /// Knots of the curve
  GEOMAPI_EXPORT std::list<double> knots() const;

  /// Multiplicities of the knots
  GEOMAPI_EXPORT std::list<int> mults() const;

  /// \brief Computes the parameter of a given point on a circle. The point must be
  ///        located either on the circle itself or relatively to the latter
  ///        at a distance less than the tolerance value. Return FALSE if the point
  ///        is beyond the tolerance limit or if computation fails.
  ///        Max Tolerance value is currently limited to 1.e-4
  /// \param[in] thePoint point of origin.
  /// \param[in] theTolerance tolerance of computation.
  /// \param[out] theParameter resulting parameter.
  GEOMAPI_EXPORT const bool parameter(const std::shared_ptr<GeomAPI_Pnt2d> thePoint,
                                      const double theTolerance,
                                      double& theParameter) const;

  /// \brief Calculate point on B-spline curve accrding to the given parameter
  GEOMAPI_EXPORT void D0(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// \brief Calculate point and first derivative for B-spline curve accrding to the given parameter
  GEOMAPI_EXPORT void D1(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint,
                                            std::shared_ptr<GeomAPI_XY>& theDerivative);
};

#endif
