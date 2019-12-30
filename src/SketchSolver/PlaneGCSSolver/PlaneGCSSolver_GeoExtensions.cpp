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

#include <PlaneGCSSolver_GeoExtensions.h>

#include <GeomAPI_BSpline2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <cmath>

namespace GCS
{

DeriVector2 BSplineImpl::Value(double u, double du, double* derivparam)
{
  if (!isCacheValid())
    rebuildCache();

  std::shared_ptr<GeomAPI_Pnt2d> aValue;
  std::shared_ptr<GeomAPI_XY> aDeriv;
  myCurve->D1(u, aValue, aDeriv);

  return DeriVector2(aValue->x(), aValue->y(), aDeriv->x() * du, aDeriv->y() * du);
}

BSplineImpl* BSplineImpl::Copy()
{
  return new BSplineImpl(*this);
}


bool BSplineImpl::isCacheValid() const
{
  // curve has to be initialized
  bool isValid = myCurve.get() && !myCurve->isNull();

  static const double THE_TOLERANCE = 1.e-7;
  // compare poles
  isValid = isValid && poles.size() == myCachedPoles.size();
  std::list<GeomPnt2dPtr>::const_iterator aCachePIt = myCachedPoles.begin();
  GCS::VEC_P::const_iterator aPolesIt = poles.begin();
  for (; isValid && aPolesIt != poles.end(); ++aPolesIt, ++aCachePIt) {
    isValid = isValid && fabs((*aCachePIt)->x() - *aPolesIt->x) < THE_TOLERANCE
                      && fabs((*aCachePIt)->y() - *aPolesIt->y) < THE_TOLERANCE;
  }

  // compare weights
  isValid = isValid && weights.size() == myCachedWeights.size();
  std::list<double>::const_iterator aCacheWIt = myCachedWeights.begin();
  GCS::VEC_pD::const_iterator aWeightsIt = weights.begin();
  for (; isValid && aWeightsIt != weights.end(); ++aWeightsIt, ++aCacheWIt)
    isValid = isValid && fabs(*aCacheWIt - **aWeightsIt) < THE_TOLERANCE;

  return isValid;
}

void BSplineImpl::rebuildCache()
{
  myCachedPoles.clear();
  myCachedWeights.clear();

  for (GCS::VEC_P::iterator anIt = poles.begin(); anIt != poles.end(); ++anIt)
    myCachedPoles.push_back(GeomPnt2dPtr(new GeomAPI_Pnt2d(*anIt->x, *anIt->y)));
  for (GCS::VEC_pD::iterator anIt = weights.begin(); anIt != weights.end(); ++anIt)
    myCachedWeights.push_back(**anIt);

  myCurve.reset(new GeomAPI_BSpline2d(myCachedPoles, myCachedWeights, degree, periodic));
}

} // namespace GCS
