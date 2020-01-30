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

#include <list>
#include <memory>

class GeomAPI_BSpline2d;
class GeomAPI_Pnt2d;

namespace GCS {
  /// \brife SHAPER's implementation of B-spline curves in PlaneGCS solver
  class BSplineImpl : public BSpline
  {
  public:
    virtual DeriVector2 Value(double u, double du, double* derivparam = 0);
    virtual DeriVector2 CalculateNormal(Point &p, double* derivparam = 0);

    virtual BSplineImpl* Copy();

  private:
    /// Verify the cached curve satisfies to the parameters
    bool isCacheValid() const;
    /// Poles or weights are changed, cache curve has to be rebuilt
    void rebuildCache();

  private:
    std::shared_ptr<GeomAPI_BSpline2d> myCurve; /// cached B-spline curve
    std::list<std::shared_ptr<GeomAPI_Pnt2d> > myCachedPoles; /// cached B-spline poles
    std::list<double> myCachedWeights; /// cached B-spline weights
    std::list<double> myCachedKnots; /// cached B-spline knots
    std::list<int> myCachedMultiplicities; /// cached B-spline multiplicities
  };
}

#endif
