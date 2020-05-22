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

#ifndef PlaneGCSSolver_GeomExtensions_H_
#define PlaneGCSSolver_GeomExtensions_H_

#include <PlaneGCSSolver_Defs.h>

namespace GCS {
  /// \brife SHAPER's implementation of B-spline curves in PlaneGCS solver
  class BSplineImpl : public BSpline
  {
  public:
    virtual DeriVector2 Value(double u, double du, double* derivparam = 0);
    virtual DeriVector2 CalculateNormal(Point &p, double* derivparam = 0);

    virtual BSplineImpl* Copy();

  private:
    /// Return the index of start knot for the given parameter.
    /// Parameter is updated accordingly, if the B-spline curve is periodic
    /// and the parameter is out of period.
    int spanIndex(double& u);

    /// Collect the list of poles and their weights affecting the given span
    void spanPolesAndWeights(int theSpanIndex,
                             double* theDerivParam,
                             std::vector<GCS::DeriVector2>& thePoles,
                             std::vector<double>& theWeights) const;

    /// Execute De Boor algorithm to calculate B-spline curve's value
    void performDeBoor(double theU, int theSpanIndex,
                       std::vector<GCS::DeriVector2>& thePoles, std::vector<double>& theWeights,
                       GCS::DeriVector2& theValue, GCS::DeriVector2& theDerivative) const;

    /// Calculate the value and the first derivative for the given parameter on B-spline
    void d1(double theU, double* theDerivParam,
            GCS::DeriVector2& theValue, GCS::DeriVector2& theDerivative);

    /// Find the parameter on B-spline corresponding to the given point
    /// \return \c false if it is unable to calculate the parameter
    bool parameter(const Point& thePoint, double& theParam) const;

  private:
    VEC_D myFlatKnots; /// indices of knots duplicated by multiplicity
  };
}

#endif
