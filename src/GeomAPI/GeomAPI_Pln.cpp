// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Pln.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include <GeomAPI_Pln.h>
#include <GeomAPI_Ax3.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_XYZ.h>

#include <gp_Pln.hxx>

using namespace std;

GeomAPI_Pln::GeomAPI_Pln(const std::shared_ptr<GeomAPI_Ax3>& theAxis)
: GeomAPI_Interface(new gp_Ax3(theAxis->impl<gp_Ax3>()))
{
}

GeomAPI_Pln::GeomAPI_Pln(const std::shared_ptr<GeomAPI_Pnt>& thePoint,
                         const std::shared_ptr<GeomAPI_Dir>& theNormal)
    : GeomAPI_Interface(new gp_Pln(thePoint->impl<gp_Pnt>(), theNormal->impl<gp_Dir>()))
{
}

GeomAPI_Pln::GeomAPI_Pln(const double theA, const double theB, const double theC, const double theD)
    : GeomAPI_Interface(new gp_Pln(theA, theB, theC, theD))
{
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Pln::location() const
{
  gp_Pnt aLoc = impl<gp_Pln>().Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_Pln::direction() const
{
  const gp_Dir& aDir = impl<gp_Pln>().Axis().Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}

void GeomAPI_Pln::coefficients(double& theA, double& theB, double& theC, double& theD)
{
  impl<gp_Pln>().Coefficients(theA, theB, theC, theD);
}

bool GeomAPI_Pln::isCoincident(const std::shared_ptr<GeomAPI_Pln> thePlane, const double theTolerance)
{
  if(!thePlane.get()) {
    return false;
  }

  const gp_Pln& aMyPln = impl<gp_Pln>();
  const gp_Pln& anOtherPln = thePlane->impl<gp_Pln>();
  return (aMyPln.Contains(anOtherPln.Location(), theTolerance) && aMyPln.Axis().IsParallel(anOtherPln.Axis(), theTolerance));
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Pln::intersect(const std::shared_ptr<GeomAPI_Lin>& theLine) const
{
  std::shared_ptr<GeomAPI_XYZ> aLineDir = theLine->direction()->xyz();
  std::shared_ptr<GeomAPI_XYZ> aLineLoc = theLine->location()->xyz();

  std::shared_ptr<GeomAPI_XYZ> aNormal = direction()->xyz();
  std::shared_ptr<GeomAPI_XYZ> aLocation = location()->xyz();

  double aDot = aNormal->dot(aLineDir);
  if (Abs(aDot) < Precision::SquareConfusion())
    return std::shared_ptr<GeomAPI_Pnt>();

  double aParam = aNormal->dot(aLocation->decreased(aLineLoc)) / aDot;
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aLineLoc->added(aLineDir->multiplied(aParam))));
}
