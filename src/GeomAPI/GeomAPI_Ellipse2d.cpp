// Copyright (C) 2017-2019  CEA/DEN, EDF R&D
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

// File:        GeomAPI_Ellipse2d.cpp
// Created:     26 April 2017
// Author:      Artem ZHIDKOV

#include <GeomAPI_Ellipse2d.h>
#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Dir2d.h>
#include <GeomAPI_Lin2d.h>
#include <GeomAPI_Pnt2d.h>

#include <Extrema_ExtCC2d.hxx>
#include <Extrema_ExtElC2d.hxx>
#include <Geom2d_Ellipse.hxx>
#include <Geom2dAdaptor_Curve.hxx>
#include <gp_Ax22d.hxx>
#include <gp_Elips2d.hxx>
#include <Precision.hxx>

#define MY_ELLIPSE implPtr<gp_Elips2d>()

static gp_Elips2d* newEllipse(const gp_Pnt2d& theCenter,
                              const gp_Dir2d& theXAxis,
                              const double theMajorRadius,
                              const double theMinorRadius)
{
  if (theMajorRadius < theMinorRadius - Precision::Confusion()) {
    return newEllipse(theCenter, gp_Dir2d(-theXAxis.Y(), theXAxis.X()),
                      theMinorRadius, theMajorRadius);
  }

  gp_Ax22d anAxis(theCenter, theXAxis);
  return new gp_Elips2d(anAxis, theMajorRadius, theMinorRadius);
}

static gp_Elips2d* newEllipse(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                              const std::shared_ptr<GeomAPI_Pnt2d>& theAxisPoint,
                              const std::shared_ptr<GeomAPI_Pnt2d>& thePassingPoint)
{
  const gp_Pnt2d& aCenter = theCenter->impl<gp_Pnt2d>();
  const gp_Pnt2d& anAxisPnt = theAxisPoint->impl<gp_Pnt2d>();
  const gp_Pnt2d& aPassedPnt = thePassingPoint->impl<gp_Pnt2d>();

  gp_Dir2d aXAxis(anAxisPnt.XY() - aCenter.XY());
  double aMajorRadius = anAxisPnt.Distance(aCenter);

  gp_XY aPassedDir = aPassedPnt.XY() - aCenter.XY();

  double X = aPassedDir.Dot(aXAxis.XY()) / aMajorRadius;
  if (Abs(X) > 1.0 - Precision::Confusion())
    return 0; // ellipse cannot be created for such parameters

  double Y = aPassedDir.CrossMagnitude(aXAxis.XY());
  double aMinorRadius = Y / Sqrt(1. - X * X);

  return newEllipse(aCenter, aXAxis, aMajorRadius, aMinorRadius);
}


GeomAPI_Ellipse2d::GeomAPI_Ellipse2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                     const std::shared_ptr<GeomAPI_Dir2d>& theXAxis,
                                     const double theMajorRadius,
                                     const double theMinorRadius)
  : GeomAPI_Interface(newEllipse(theCenter->impl<gp_Pnt2d>(), theXAxis->impl<gp_Dir2d>(),
                                 theMajorRadius, theMinorRadius))
{
}

GeomAPI_Ellipse2d::GeomAPI_Ellipse2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                                     const std::shared_ptr<GeomAPI_Pnt2d>& theAxisPoint,
                                     const std::shared_ptr<GeomAPI_Pnt2d>& thePassingPoint)
  : GeomAPI_Interface(newEllipse(theCenter, theAxisPoint, thePassingPoint))
{
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Ellipse2d::center() const
{
  const gp_Pnt2d& aCenter = MY_ELLIPSE->Location();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aCenter.X(), aCenter.Y()));
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Ellipse2d::firstFocus() const
{
  const gp_Pnt2d& aFirst = MY_ELLIPSE->Focus1();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aFirst.X(), aFirst.Y()));
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Ellipse2d::secondFocus() const
{
  const gp_Pnt2d& aSecond = MY_ELLIPSE->Focus2();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aSecond.X(), aSecond.Y()));
}

double GeomAPI_Ellipse2d::minorRadius() const
{
  return MY_ELLIPSE->MinorRadius();
}

double GeomAPI_Ellipse2d::majorRadius() const
{
  return MY_ELLIPSE->MajorRadius();
}

template <typename EXTREMAPTR>
static double extrema(EXTREMAPTR theAlgo,
                      GeomPnt2dPtr& thePoint1,
                      GeomPnt2dPtr& thePoint2)
{
  double aDistance = Precision::Infinite();
  if (theAlgo->IsDone() && theAlgo->NbExt() > 0) {
    Extrema_POnCurv2d aP1, aP2;
    for (int i = 1; i <= theAlgo->NbExt(); ++i)
      if (theAlgo->SquareDistance(i) < aDistance) {
        aDistance = Sqrt(theAlgo->SquareDistance(i));
        theAlgo->Points(i, aP1, aP2);
      }

    aDistance = Sqrt(aDistance);
    thePoint1 = GeomPnt2dPtr(new GeomAPI_Pnt2d(aP1.Value().X(), aP1.Value().Y()));
    thePoint2 = GeomPnt2dPtr(new GeomAPI_Pnt2d(aP2.Value().X(), aP2.Value().Y()));
  }
  return aDistance;
}

double GeomAPI_Ellipse2d::distance(const std::shared_ptr<GeomAPI_Lin2d>& theLine,
                                   std::shared_ptr<GeomAPI_Pnt2d>& thePointOnMe,
                                   std::shared_ptr<GeomAPI_Pnt2d>& thePointOnLine)
{
  Extrema_ExtElC2d anExtema(theLine->impl<gp_Lin2d>(), *MY_ELLIPSE);
  return extrema(&anExtema, thePointOnLine, thePointOnMe);
}

double GeomAPI_Ellipse2d::distance(const std::shared_ptr<GeomAPI_Circ2d>& theCircle,
                                   std::shared_ptr<GeomAPI_Pnt2d>& thePointOnMe,
                                   std::shared_ptr<GeomAPI_Pnt2d>& thePointOnCircle)
{
  Extrema_ExtElC2d anExtema(theCircle->impl<gp_Circ2d>(), *MY_ELLIPSE);
  return extrema(&anExtema, thePointOnCircle, thePointOnMe);
}

double GeomAPI_Ellipse2d::distance(const std::shared_ptr<GeomAPI_Ellipse2d>& theEllipse,
                                   std::shared_ptr<GeomAPI_Pnt2d>& thePointOnMe,
                                   std::shared_ptr<GeomAPI_Pnt2d>& thePointOnEllipse)
{
  Handle(Geom2d_Ellipse) anEllipse1 = new Geom2d_Ellipse(theEllipse->impl<gp_Elips2d>());
  Handle(Geom2d_Ellipse) anEllipse2 = new Geom2d_Ellipse(*MY_ELLIPSE);

  Extrema_ExtCC2d* anExtema =
      new Extrema_ExtCC2d(Geom2dAdaptor_Curve(anEllipse1), Geom2dAdaptor_Curve(anEllipse2));
  double aDistance = extrema(anExtema, thePointOnEllipse, thePointOnMe);
  delete anExtema;
  return aDistance;
}
