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

#ifndef GeomAPI_BSpline_H_
#define GeomAPI_BSpline_H_

#include <GeomAPI_Interface.h>
#include <GeomAPI_Curve.h>

#include <list>
#include <memory>

class GeomAPI_Pnt;
class GeomAPI_BSpline;

//! Pointer on the object
typedef std::shared_ptr<GeomAPI_BSpline> GeomBSplinePtr;

/**\class GeomAPI_BSpline
 * \ingroup DataModel
 * \brief B-spline in 3D
 */
class GeomAPI_BSpline : public GeomAPI_Interface
{
public:
  /// Creation of B-spline defined by a curve
  GEOMAPI_EXPORT GeomAPI_BSpline (const GeomCurvePtr& theCurve);

  /// Degree of B-spline curve
  GEOMAPI_EXPORT int degree() const;

  /// Poles of B-spline curve
  GEOMAPI_EXPORT std::list<std::shared_ptr<GeomAPI_Pnt> > poles() const;

  /// Weights of B-spline poles
  GEOMAPI_EXPORT std::list<double> weights() const;

  /// Knots of B-spline curve
  GEOMAPI_EXPORT std::list<double> knots() const;

  /// Multiplicities of B-spline knots
  GEOMAPI_EXPORT std::list<int> mults() const;

  /// Return \c true if the curve is periodic
  GEOMAPI_EXPORT bool isPeriodic() const;

  /// Convert any curve into a B-spline curve
  GEOMAPI_EXPORT static GeomBSplinePtr convertToBSpline (const GeomCurvePtr& theCurve);
};

#endif
