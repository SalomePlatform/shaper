// Copyright (C) 2019-2020  CEA/DEN, EDF R&D
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
  GEOMAPI_EXPORT GeomAPI_BSpline2d(const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
                                   const std::list<double>& theWeights,
                                   const int theDegree,
                                   const bool thePeriodic = false);

  /// Returns true if curve is not initialized
  GEOMAPI_EXPORT bool isNull() const;

  /// Returns degree of the curve
  GEOMAPI_EXPORT int degree() const;

  /// \brief Calculate point on B-spline curve accrding to the given parameter
  GEOMAPI_EXPORT void D0(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint);

  /// \brief Calculate point and first derivative for B-spline curve accrding to the given parameter
  GEOMAPI_EXPORT void D1(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint,
                                            std::shared_ptr<GeomAPI_XY>& theDerivative);
};

#endif
