// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Circ2d.cpp
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Dir2d.h>

#include <gp_Dir2d.hxx>
#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Ax2d.hxx>
#include <GeomLib_Tool.hxx>
#include <Geom2d_Circle.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Precision.hxx>

#include <IntAna2d_AnaIntersection.hxx>

#define MY_CIRC2D implPtr<gp_Circ2d>()

static gp_Circ2d* newCirc2d(const double theCenterX, const double theCenterY, const gp_Dir2d theDir,
                            const double theRadius)
{
  gp_Pnt2d aCenter(theCenterX, theCenterY);
  return new gp_Circ2d(gp_Ax2d(aCenter, theDir), theRadius);
}

static gp_Circ2d* newCirc2d(const double theCenterX, const double theCenterY,
                            const double thePointX, const double thePointY)
{
  gp_Pnt2d aCenter(theCenterX, theCenterY);
  gp_Pnt2d aPoint(thePointX, thePointY);

  double aRadius = aCenter.Distance(aPoint);

  if (aCenter.IsEqual(aPoint, Precision::Confusion()))
    return NULL;

  gp_Dir2d aDir(theCenterX - thePointX, theCenterY - thePointY);

  return newCirc2d(theCenterX, theCenterY, aDir, aRadius);
}

static gp_Circ2d* newCirc2d(const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                            const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint,
                            const std::shared_ptr<GeomAPI_Pnt2d>& theThirdPoint)
{
  gp_XY aFirstPnt(theFirstPoint->x(), theFirstPoint->y());
  gp_XY aSecondPnt(theSecondPoint->x(), theSecondPoint->y());
  gp_XY aThirdPnt(theThirdPoint->x(), theThirdPoint->y());

  gp_XY aVec12 = aSecondPnt - aFirstPnt;
  gp_XY aVec23 = aThirdPnt - aSecondPnt;
  gp_XY aVec31 = aFirstPnt - aThirdPnt;
  // square of parallelogram
  double aSquare2 = aVec12.Crossed(aVec23);
  aSquare2 *= aSquare2 * 2.0;
  if (aSquare2 < 1.e-20)
    return NULL;
  // coefficients to calculate center
  double aCoeff1 = aVec23.Dot(aVec23) / aSquare2 * aVec12.Dot(aVec31.Reversed());
  double aCoeff2 = aVec31.Dot(aVec31) / aSquare2 * aVec23.Dot(aVec12.Reversed());
  double aCoeff3 = aVec12.Dot(aVec12) / aSquare2 * aVec31.Dot(aVec23.Reversed());
  // center
  gp_XY aCenter = aFirstPnt * aCoeff1 + aSecondPnt * aCoeff2 + aThirdPnt * aCoeff3;
  // radius
  double aRadius = (aFirstPnt - aCenter).Modulus();

  gp_Dir2d aDir(aFirstPnt - aCenter);
  return newCirc2d(aCenter.X(), aCenter.Y(), aDir, aRadius);
}

GeomAPI_Circ2d::GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theCirclePoint)
    : GeomAPI_Interface(
        newCirc2d(theCenter->x(), theCenter->y(), theCirclePoint->x(), theCirclePoint->y()))
{
}

GeomAPI_Circ2d::GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                               const std::shared_ptr<GeomAPI_Dir2d>& theDir, double theRadius)
    : GeomAPI_Interface(
        newCirc2d(theCenter->x(), theCenter->y(), theDir->impl<gp_Dir2d>(), theRadius))
{
}

GeomAPI_Circ2d::GeomAPI_Circ2d(const std::shared_ptr<GeomAPI_Pnt2d>& theFirstPoint,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theSecondPoint,
                               const std::shared_ptr<GeomAPI_Pnt2d>& theThirdPoint)
    : GeomAPI_Interface(newCirc2d(theFirstPoint, theSecondPoint, theThirdPoint))
{
}

const std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Circ2d::project(
    const std::shared_ptr<GeomAPI_Pnt2d>& thePoint) const
{
  std::shared_ptr<GeomAPI_Pnt2d> aResult;
  if (!MY_CIRC2D)
    return aResult;

  const gp_Pnt2d& aCenter = MY_CIRC2D->Location();
  const gp_Pnt2d& aPoint = thePoint->impl<gp_Pnt2d>();

  double aDist = aCenter.Distance(aPoint);
  if (aDist < Precision::Confusion())
    return aResult;

  if (Abs(aDist - MY_CIRC2D->Radius()) < Precision::Confusion()) {
    // Point on the circle
    aResult = std::shared_ptr<GeomAPI_Pnt2d>(
        new GeomAPI_Pnt2d(thePoint->x(), thePoint->y()));
  } else {
    gp_Dir2d aDir(aPoint.XY() - aCenter.XY());
    gp_XY aNewPoint = aCenter.XY() + aDir.XY() * MY_CIRC2D->Radius();
    aResult = std::shared_ptr<GeomAPI_Pnt2d>(
        new GeomAPI_Pnt2d(aNewPoint.X(), aNewPoint.Y()));
  }

  return aResult;
}

const std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Circ2d::center() const
{
  if (!MY_CIRC2D)
    return std::shared_ptr<GeomAPI_Pnt2d>();
  const gp_Pnt2d& aCenter = MY_CIRC2D->Location();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aCenter.X(), aCenter.Y()));
}

double GeomAPI_Circ2d::radius() const
{
  if (!MY_CIRC2D)
    return 0.0;
  return MY_CIRC2D->Radius();
}

//=================================================================================================
const bool GeomAPI_Circ2d::parameter(const std::shared_ptr<GeomAPI_Pnt2d> thePoint,
                                   const double theTolerance,
                                   double& theParameter) const
{
  Handle(Geom2d_Circle) aCurve = new Geom2d_Circle(*MY_CIRC2D);
  return GeomLib_Tool::Parameter(aCurve, thePoint->impl<gp_Pnt2d>(), theTolerance, theParameter) == Standard_True;
}

//=================================================================================================
void GeomAPI_Circ2d::D0(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  Handle(Geom2d_Circle) aCurve = new Geom2d_Circle(*MY_CIRC2D);
  gp_Pnt2d aPnt;
  aCurve->D0(theU, aPnt);
  thePoint.reset(new GeomAPI_Pnt2d(aPnt.X(), aPnt.Y()));
}

