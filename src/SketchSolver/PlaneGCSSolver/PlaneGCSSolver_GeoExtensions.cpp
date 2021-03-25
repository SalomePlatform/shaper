// Copyright (C) 2019-2021  CEA/DEN, EDF R&D
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
#include <PlaneGCSSolver_Tools.h>

#include <GeomAPI_BSpline2d.h>
#include <GeomAPI_Pnt2d.h>

#include <cmath>

namespace GCS
{

  static void periodicNormalization(double& theParam, double thePeriodStart, double thePeriodEnd)
  {
    double aPeriod = thePeriodEnd - thePeriodStart;
    if (aPeriod > tolerance) {
      theParam = std::max(thePeriodStart,
                          theParam + aPeriod * std::ceil((thePeriodStart - theParam) / aPeriod));
    }
  }


DeriVector2 BSplineImpl::Value(double u, double du, double* derivparam)
{
  DeriVector2 value, deriv;
  d1(u, derivparam, value, deriv);
  return value.sum(GCS::DeriVector2(0., 0., deriv.x, deriv.y).mult(du));
}

DeriVector2 BSplineImpl::CalculateNormal(Point &p, double* derivparam)
{
  double u = 0.0;
  if (!parameter(p, u))
    return DeriVector2();

  DeriVector2 value, deriv;
  d1(u, derivparam, value, deriv);
  return deriv.rotate90ccw();
}

BSplineImpl* BSplineImpl::Copy()
{
  return new BSplineImpl(*this);
}

void BSplineImpl::d1(double theU,
                     double* theDerivParam,
                     GCS::DeriVector2& theValue,
                     GCS::DeriVector2& theDerivative)
{
  int aSpan = spanIndex(theU);
  std::vector<GCS::DeriVector2> aPoles;
  std::vector<double> aWeights;
  spanPolesAndWeights(aSpan, theDerivParam, aPoles, aWeights);
  performDeBoor(theU, aSpan, aPoles, aWeights, theValue, theDerivative);
}

int BSplineImpl::spanIndex(double& u)
{
  if (myFlatKnots.empty()) {
    // fill flat knots indices
    for (int i = 0; i < (int)mult.size(); ++i)
      myFlatKnots.resize(myFlatKnots.size() + mult[i], *knots[i]);
    if (periodic) {
      // additional knots at the beginning and the end to complete periodity
      int anExtraBegin = degree + 1 - mult.front();
      int anExtraEnd = degree + 1 - mult.back();
      double aPeriod = *knots.back() - *knots.front();
      VEC_D aNewFlatKnots;
      aNewFlatKnots.reserve(myFlatKnots.size() + (size_t)(anExtraBegin + anExtraEnd));
      auto it = myFlatKnots.end() - mult.back() - anExtraBegin;
      while (anExtraBegin > 0) {
        aNewFlatKnots.push_back(*(it++) - aPeriod);
        --anExtraBegin;
      }
      aNewFlatKnots.insert(aNewFlatKnots.end(), myFlatKnots.begin(), myFlatKnots.end());
      it = myFlatKnots.begin() + mult.front();
      while (anExtraEnd > 0) {
        aNewFlatKnots.push_back(*(it++) + aPeriod);
        --anExtraEnd;
      }
      myFlatKnots = aNewFlatKnots;
    }
  }

  if (periodic)
    periodicNormalization(u, *knots.front(), *knots.back());

  int anIndex = 0;
  for (int i = 1; i < (int)knots.size() - 1; ++i) {
    if (u <= *knots[i])
      break;
    anIndex += mult[i];
  }
  return anIndex;
}

void BSplineImpl::spanPolesAndWeights(int theSpanIndex,
                                      double* theDerivParam,
                                      std::vector<GCS::DeriVector2>& thePoles,
                                      std::vector<double>& theWeights) const
{
  thePoles.reserve(degree + 1);
  theWeights.reserve(degree + 1);
  for (int i = theSpanIndex; i <= theSpanIndex + degree; ++i) {
    // optimization: weighted pole
    int idx = i % (int)poles.size();
    thePoles.push_back(GCS::DeriVector2(poles[idx], theDerivParam).mult(*weights[idx]));
    theWeights.push_back(*weights[idx]);
  }
}

void BSplineImpl::performDeBoor(double theU,
                                int theSpanIndex,
                                std::vector<GCS::DeriVector2>& thePoles,
                                std::vector<double>& theWeights,
                                GCS::DeriVector2& theValue,
                                GCS::DeriVector2& theDerivative) const
{
  std::vector<GCS::DeriVector2> aPDeriv(thePoles.size(), DeriVector2());
  std::vector<double> aWDeriv(theWeights.size(), 0.0);
  for (int i = 0; i < degree; ++i) {
    for (int j = degree; j > i; --j) {
      double denom = (myFlatKnots[theSpanIndex + j + degree - i] -
                      myFlatKnots[theSpanIndex + j]);
      double a = (theU - myFlatKnots[theSpanIndex + j]) / denom;
      aPDeriv[j] = aPDeriv[j].linCombi(a, aPDeriv[j - 1], 1.0 - a).sum(
                   thePoles[j].subtr(thePoles[j - 1]).mult(1.0 / denom));
      aWDeriv[j] = aWDeriv[j] * a + aWDeriv[j - 1] * (1.0 - a)
                 + (theWeights[j] - theWeights[j - 1]) / denom;
      thePoles[j] = thePoles[j].linCombi(a, thePoles[j - 1], 1.0 - a);
      theWeights[j] = theWeights[j] * a + theWeights[j - 1] * (1.0 - a);
    }
  }
  double w = 1 / theWeights[degree];
  theValue = thePoles[degree].mult(w);
  theDerivative = aPDeriv[degree].subtr(theValue.mult(aWDeriv[degree])).mult(w);
}

bool BSplineImpl::parameter(const Point& thePoint, double& theParam) const
{
  std::list<GeomPnt2dPtr> aPoles;
  std::list<double> aWeights;
  std::list<double> aKnots;
  std::list<int> aMults;

  for (GCS::VEC_P::const_iterator anIt = poles.begin(); anIt != poles.end(); ++anIt)
    aPoles.push_back(GeomPnt2dPtr(new GeomAPI_Pnt2d(*anIt->x, *anIt->y)));
  for (GCS::VEC_pD::const_iterator anIt = weights.begin(); anIt != weights.end(); ++anIt)
    aWeights.push_back(**anIt);
  for (GCS::VEC_pD::const_iterator anIt = knots.begin(); anIt != knots.end(); ++anIt)
    aKnots.push_back(**anIt);
  aMults.assign(mult.begin(), mult.end());

  std::shared_ptr<GeomAPI_BSpline2d> aCurve(
      new GeomAPI_BSpline2d(degree, aPoles, aWeights, aKnots, aMults, periodic));

  GeomPnt2dPtr aPoint(new GeomAPI_Pnt2d(*thePoint.x, *thePoint.y));
  return aCurve->parameter(aPoint, 1e100, theParam);
}

} // namespace GCS
