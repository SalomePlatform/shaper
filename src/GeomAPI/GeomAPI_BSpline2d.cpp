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

#include <GeomAPI_BSpline2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_XY.h>

#include <Geom2d_BSplineCurve.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <GeomLib_Tool.hxx>
#include <Precision.hxx>

#define MY_BSPLINE (*(implPtr<Handle_Geom2d_BSplineCurve>()))


static Handle_Geom2d_BSplineCurve* newBSpline2d(
  const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
  const std::list<double>& theWeights,
  const int theDegree,
  const bool thePeriodic);


static Handle_Geom2d_BSplineCurve* newBSpline2d(
    const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
    const std::list<double>& theWeights,
    const std::list<double>& theKnots,
    const std::list<int>& theMults,
    const int theDegree,
    const bool thePeriodic)
{
  if (theKnots.empty() || theMults.empty())
    return newBSpline2d(thePoles, theWeights, theDegree, thePeriodic);

  int anAuxPole = 0;
  if (thePeriodic && thePoles.front()->distance(thePoles.back()) < Precision::Confusion())
    anAuxPole = -1;

  // collect arrays of poles, weights, knots and multiplicities
  TColgp_Array1OfPnt2d aPoles(1, (int)thePoles.size() + anAuxPole);
  TColStd_Array1OfReal aWeights(1, (int)theWeights.size() + anAuxPole);
  TColStd_Array1OfReal aKnots(1, (int)theKnots.size());
  TColStd_Array1OfInteger aMults(1, (int)theMults.size());

  int anIndex = 1;
  for (std::list<GeomPnt2dPtr>::const_iterator aPIt = thePoles.begin();
       aPIt != thePoles.end() && anIndex <= aPoles.Upper(); ++aPIt, ++anIndex)
    aPoles.SetValue(anIndex, gp_Pnt2d((*aPIt)->x(), (*aPIt)->y()));
  anIndex = 1;
  for (std::list<double>::const_iterator aWIt = theWeights.begin();
       aWIt != theWeights.end() && anIndex <= aWeights.Upper(); ++aWIt, ++anIndex)
    aWeights.SetValue(anIndex, *aWIt);
  anIndex = 1;
  for (std::list<double>::const_iterator aKIt = theKnots.begin();
       aKIt != theKnots.end(); ++aKIt, ++anIndex)
    aKnots.SetValue(anIndex, *aKIt);
  anIndex = 1;
  for (std::list<int>::const_iterator aMIt = theMults.begin();
       aMIt != theMults.end(); ++aMIt, ++anIndex)
    aMults.SetValue(anIndex, *aMIt);

  Handle(Geom2d_BSplineCurve) aCurve =
      new Geom2d_BSplineCurve(aPoles, aWeights, aKnots, aMults, theDegree, thePeriodic);
  return new Handle_Geom2d_BSplineCurve(aCurve);
}

Handle_Geom2d_BSplineCurve* newBSpline2d(
    const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
    const std::list<double>& theWeights,
    const int theDegree,
    const bool thePeriodic)
{
  std::list<std::shared_ptr<GeomAPI_Pnt2d> > aPoles = thePoles;
  std::list<double> aWeights = theWeights;
  int aMult = theDegree + 1;
  int aNbKnots = (int)thePoles.size() - theDegree + 1;
  if (thePeriodic) {
    if (aPoles.front()->distance(aPoles.back()) < Precision::Confusion()) {
      aPoles.pop_back();
      aWeights.pop_back();
    }
    aMult = 1;
    aNbKnots = (int)aPoles.size() + 1;
  }

  if (aNbKnots < 2)
    return new Handle_Geom2d_BSplineCurve();

  static const double aStartParam = 0.0;
  static const double aEndParam = 1.0;
  double aStep = aEndParam / (aNbKnots - 1);
  int anIndex = 1;
  std::list<double> aKnots;
  for (double aKnot = aStartParam; anIndex < aNbKnots; ++anIndex, aKnot += aStep)
    aKnots.push_back(aKnot);
  aKnots.push_back(aEndParam);

  std::list<int> aMults(aNbKnots - 2, 1);
  aMults.push_front(aMult);
  aMults.push_back(aMult);

  return newBSpline2d(aPoles, aWeights, aKnots, aMults, theDegree, thePeriodic);
}

static Handle_Geom2d_BSplineCurve* newBSpline2d(
    const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
    const std::list<double>& theWeights,
    const bool thePeriodic)
{
  int aDegree = 3;
  if ((int)thePoles.size() <= aDegree)
    aDegree = (int)thePoles.size() - 1;
  if (aDegree <= 0)
    return new Handle_Geom2d_BSplineCurve();
  return newBSpline2d(thePoles, theWeights, aDegree, thePeriodic);
}


GeomAPI_BSpline2d::GeomAPI_BSpline2d(const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
                                     const std::list<double>& theWeights,
                                     const bool thePeriodic)
  : GeomAPI_Interface(newBSpline2d(thePoles, theWeights, thePeriodic))
{
  if (isNull())
    throw Standard_ConstructionError("GeomAPI_BSpline2d: Impossible to create B-spline curve");
}

GeomAPI_BSpline2d::GeomAPI_BSpline2d(const int theDegree,
                                     const std::list<std::shared_ptr<GeomAPI_Pnt2d> >& thePoles,
                                     const std::list<double>& theWeights,
                                     const std::list<double>& theKnots,
                                     const std::list<int>& theMults,
                                     const bool thePeriodic)
  : GeomAPI_Interface(newBSpline2d(thePoles, theWeights, theKnots, theMults,
                                   theDegree, thePeriodic))
{
  if (isNull())
    throw Standard_ConstructionError("GeomAPI_BSpline2d: Impossible to create B-spline curve");
}

bool GeomAPI_BSpline2d::isNull() const
{
  return MY_BSPLINE.IsNull();
}

int GeomAPI_BSpline2d::degree() const
{
  return MY_BSPLINE->Degree();
}

std::list<double> GeomAPI_BSpline2d::knots() const
{
  const TColStd_Array1OfReal& aBSplKnots = MY_BSPLINE->Knots();
  return std::list<double>(aBSplKnots.begin(), aBSplKnots.end());
}

std::list<int> GeomAPI_BSpline2d::mults() const
{
  const TColStd_Array1OfInteger& aBSplMults = MY_BSPLINE->Multiplicities();
  return std::list<int>(aBSplMults.begin(), aBSplMults.end());
}

const bool GeomAPI_BSpline2d::parameter(const std::shared_ptr<GeomAPI_Pnt2d> thePoint,
                                        const double theTolerance,
                                        double& theParameter) const
{
  return GeomLib_Tool::Parameter(MY_BSPLINE, thePoint->impl<gp_Pnt2d>(),
                                 theTolerance, theParameter) == Standard_True;
}

void GeomAPI_BSpline2d::D0(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  gp_Pnt2d aPnt;
  MY_BSPLINE->D0(theU, aPnt);
  thePoint.reset(new GeomAPI_Pnt2d(aPnt.X(), aPnt.Y()));
}

void GeomAPI_BSpline2d::D1(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint,
                                              std::shared_ptr<GeomAPI_XY>& theDerivative)
{
  gp_Pnt2d aPnt;
  gp_Vec2d aVec;
  MY_BSPLINE->D1(theU, aPnt, aVec);
  thePoint.reset(new GeomAPI_Pnt2d(aPnt.X(), aPnt.Y()));
  theDerivative.reset(new GeomAPI_XY(aVec.X(), aVec.Y()));
}
