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

#include <GeomAPI_BSpline.h>
#include <GeomAPI_Pnt.h>

#include <Geom_BSplineCurve.hxx>

#include <GeomConvert.hxx>

#define MY_BSPLINE (*(implPtr<Handle_Geom_BSplineCurve>()))

GeomAPI_BSpline::GeomAPI_BSpline (const GeomCurvePtr& theCurve)
{
  GeomCurvePtr anUntrimmedCurve = theCurve->basisCurve();
  Handle(Geom_Curve) aCurve = anUntrimmedCurve->impl<Handle(Geom_Curve)>();
  Handle(Geom_BSplineCurve) aBSpl = Handle(Geom_BSplineCurve)::DownCast(aCurve);
  if (aBSpl.IsNull())
    throw Standard_ConstructionError("GeomAPI_BSpline: Curve is not a B-spline");
  setImpl(new Handle_Geom_BSplineCurve(aBSpl));
}

int GeomAPI_BSpline::degree() const
{
  return MY_BSPLINE->Degree();
}

std::list<GeomPointPtr> GeomAPI_BSpline::poles() const
{
  const TColgp_Array1OfPnt& aBSplPoles = MY_BSPLINE->Poles();

  std::list<GeomPointPtr> aPoles;
  for (int anIndex = aBSplPoles.Lower(); anIndex <= aBSplPoles.Upper(); ++anIndex) {
    const gp_Pnt& aPoint = aBSplPoles.Value(anIndex);
    aPoles.push_back(GeomPointPtr(new GeomAPI_Pnt(aPoint.X(), aPoint.Y(), aPoint.Z())));
  }
  return aPoles;
}

std::list<double> GeomAPI_BSpline::weights() const
{
  std::list<double> aWeights;
  const TColStd_Array1OfReal* aBSplWeights = MY_BSPLINE->Weights();
  if (aBSplWeights)
    aWeights.assign(aBSplWeights->begin(), aBSplWeights->end());
  return aWeights;
}

std::list<double> GeomAPI_BSpline::knots() const
{
  const TColStd_Array1OfReal& aBSplKnots = MY_BSPLINE->Knots();
  return std::list<double>(aBSplKnots.begin(), aBSplKnots.end());
}

std::list<int> GeomAPI_BSpline::mults() const
{
  const TColStd_Array1OfInteger& aBSplMults = MY_BSPLINE->Multiplicities();
  return std::list<int>(aBSplMults.begin(), aBSplMults.end());
}

bool GeomAPI_BSpline::isPeriodic() const
{
  return MY_BSPLINE->IsPeriodic();
}

GeomBSplinePtr GeomAPI_BSpline::convertToBSpline (const GeomCurvePtr& theCurve)
{
  GeomCurvePtr anUntrimmedCurve = theCurve->basisCurve();
  Handle(Geom_Curve) aCurve = anUntrimmedCurve->impl<Handle(Geom_Curve)>();
  Handle(Geom_BSplineCurve) aBSpl = Handle(Geom_BSplineCurve)::DownCast(aCurve);
  if (aBSpl.IsNull()) {
    // convert to b-spline
    aBSpl = GeomConvert::CurveToBSplineCurve(aCurve);
  }
  if (aBSpl.IsNull())
    throw Standard_ConstructionError("GeomAPI_BSpline: Conversion to B-spline failed");
  GeomCurvePtr aResCurve (new GeomAPI_Curve());
  aResCurve->setImpl(new Handle_Geom_BSplineCurve(aBSpl));
  GeomBSplinePtr aResult (new GeomAPI_BSpline(aResCurve));
  return aResult;
}
