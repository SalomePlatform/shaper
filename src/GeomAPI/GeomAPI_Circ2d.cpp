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
#include <Geom2d_Circle.hxx>
#include <Geom2dAPI_ProjectPointOnCurve.hxx>
#include <Precision.hxx>

#include <IntAna2d_AnaIntersection.hxx>

#define MY_CIRC2D static_cast<gp_Circ2d*>(myImpl)

static gp_Circ2d* newCirc2d(const double theCenterX, const double theCenterY,
                            const gp_Dir2d theDir, const double theRadius)
{
  gp_Pnt2d aCenter(theCenterX, theCenterY);
  return new gp_Circ2d(gp_Ax2d(aCenter, theDir), theRadius);
}

static gp_Circ2d* newCirc2d(const double theCenterX, const double theCenterY,
                            const double thePointX,   const double thePointY)
{
  gp_Pnt2d aCenter(theCenterX, theCenterY);
  gp_Pnt2d aPoint(thePointX, thePointY);

  double aRadius = aCenter.Distance(aPoint);

  if (aCenter.IsEqual(aPoint, Precision::Confusion()))
      return NULL;

  gp_Dir2d aDir(theCenterX - thePointX, theCenterY - thePointY);
  
  return newCirc2d(theCenterX, theCenterY, aDir, aRadius);
}

GeomAPI_Circ2d::GeomAPI_Circ2d(const boost::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                               const boost::shared_ptr<GeomAPI_Pnt2d>& theCirclePoint)
  : GeomAPI_Interface(newCirc2d(theCenter->x(), theCenter->y(),
                                theCirclePoint->x(),   theCirclePoint->y()))
{}

GeomAPI_Circ2d::GeomAPI_Circ2d(const boost::shared_ptr<GeomAPI_Pnt2d>& theCenter,
                               const boost::shared_ptr<GeomAPI_Dir2d>& theDir,
                               double theRadius)
 : GeomAPI_Interface(newCirc2d(theCenter->x(), theCenter->y(),
                               theDir->impl<gp_Dir2d>(), theRadius))
{

}

const boost::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Circ2d::project(const boost::shared_ptr<GeomAPI_Pnt2d>& thePoint) const
{
  boost::shared_ptr<GeomAPI_Pnt2d> aResult;
  if (!MY_CIRC2D)
    return aResult;

  Handle(Geom2d_Circle) aCircle = new Geom2d_Circle(MY_CIRC2D->Axis(), MY_CIRC2D->Radius());//(aCirc);

  const gp_Pnt2d& aPoint = thePoint->impl<gp_Pnt2d>();

  Geom2dAPI_ProjectPointOnCurve aProj(aPoint, aCircle);
  Standard_Integer aNbPoint = aProj.NbPoints();
  double aX, anY;
  if (aNbPoint > 0) {
    double aMinDistance = 0, aDistance;
    for (Standard_Integer j = 1; j <= aNbPoint; j++) {
      gp_Pnt2d aNewPoint = aProj.Point(j);
      aDistance = aNewPoint.Distance(aPoint);
      if (!aMinDistance || aDistance < aMinDistance) {
        aX = aNewPoint.X();
        anY = aNewPoint.Y();
        aMinDistance = aDistance;
        aResult = boost::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, anY));
      }
    }
  }
  return aResult;
}

const boost::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Circ2d::center() const
{
  const gp_Pnt2d& aCenter = MY_CIRC2D->Location();
  return boost::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aCenter.X(), aCenter.Y()));
}

double GeomAPI_Circ2d::radius() const
{
  return MY_CIRC2D->Radius();
}

