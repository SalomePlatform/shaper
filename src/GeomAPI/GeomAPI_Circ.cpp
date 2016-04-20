// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Circ2cpp
// Created:     24 Jun 2014
// Author:      Artem ZHIDKOV

#include <GeomAPI_Circ.h>

#include <GeomAPI_Ax2.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

#include <gp_Dir.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>

#include <Geom_Circle.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomLib_Tool.hxx>

#define MY_CIRC implPtr<gp_Circ>()

static gp_Circ* newCirc(const gp_Pnt& theCenter, const gp_Dir& theDir, const double theRadius)
{
  return new gp_Circ(gp_Ax2(theCenter, theDir), theRadius);
}

//=================================================================================================
GeomAPI_Circ::GeomAPI_Circ(const std::shared_ptr<GeomAPI_Ax2> theAx2,
                           const double theRadius)
: GeomAPI_Interface(new gp_Circ(theAx2->impl<gp_Ax2>(), theRadius))
{

}


//=================================================================================================
GeomAPI_Circ::GeomAPI_Circ(const std::shared_ptr<GeomAPI_Pnt>& theCenter,
                           const std::shared_ptr<GeomAPI_Dir>& theDir, double theRadius)
    : GeomAPI_Interface(newCirc(theCenter->impl<gp_Pnt>(), theDir->impl<gp_Dir>(), theRadius))
{
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Pnt> GeomAPI_Circ::center() const
{
  const gp_Pnt& aCenter = MY_CIRC->Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCenter.X(), aCenter.Y(), aCenter.Z()));
}

//=================================================================================================
double GeomAPI_Circ::radius() const
{
  return MY_CIRC->Radius();
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Pnt> GeomAPI_Circ::project(
    const std::shared_ptr<GeomAPI_Pnt>& thePoint) const
{
  std::shared_ptr<GeomAPI_Pnt> aResult;
  if (!MY_CIRC)
  return aResult;

  Handle(Geom_Circle) aCircle = new Geom_Circle(*MY_CIRC);

  const gp_Pnt& aPoint = thePoint->impl<gp_Pnt>();

  GeomAPI_ProjectPointOnCurve aProj(aPoint, aCircle);
  Standard_Integer aNbPoint = aProj.NbPoints();
  if (aNbPoint > 0) {
    double aMinDistance = 0, aDistance;
    for (Standard_Integer j = 1; j <= aNbPoint; j++) {
      gp_Pnt aNewPoint = aProj.Point(j);
      aDistance = aNewPoint.Distance(aPoint);
      if (!aMinDistance || aDistance < aMinDistance) {
        aMinDistance = aDistance;
        aResult = std::shared_ptr<GeomAPI_Pnt>(
            new GeomAPI_Pnt(aNewPoint.X(), aNewPoint.Y(), aNewPoint.Z()));
      }
    }
  }
  return aResult;
}

//=================================================================================================
const bool GeomAPI_Circ::parameter(const std::shared_ptr<GeomAPI_Pnt> thePoint,
                                   const double theTolerance,
                                   double& theParameter) const
{
  Handle(Geom_Circle) aCurve = new Geom_Circle(*MY_CIRC);
  return GeomLib_Tool::Parameter(aCurve, thePoint->impl<gp_Pnt>(), theTolerance, theParameter) == Standard_True;
}

//=================================================================================================
std::shared_ptr<GeomAPI_Dir> GeomAPI_Circ::normal() const
{
  const gp_Ax1& anAxis = MY_CIRC->Axis();
  const gp_Dir& aDir = anAxis.Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}
