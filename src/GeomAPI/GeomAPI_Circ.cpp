// File:        GeomAPI_Circ2cpp
// Created:     24 Jun 2014
// Author:      Artem ZHIDKOV

#include <GeomAPI_Circ.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

#include <gp_Dir.hxx>
#include <gp_Circ.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax2.hxx>

#include <Geom_Circle.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#define MY_CIRC static_cast<gp_Circ*>(myImpl)

static gp_Circ* newCirc(const gp_Pnt& theCenter,
                        const gp_Dir& theDir,
                        const double  theRadius)
{
  return new gp_Circ(gp_Ax2(theCenter, theDir), theRadius);
}

GeomAPI_Circ::GeomAPI_Circ(const boost::shared_ptr<GeomAPI_Pnt>& theCenter,
                           const boost::shared_ptr<GeomAPI_Dir>& theDir,
                           double                                theRadius)
  : GeomAPI_Interface(newCirc(theCenter->impl<gp_Pnt>(),
                              theDir->impl<gp_Dir>(), theRadius))
{
}

const boost::shared_ptr<GeomAPI_Pnt> GeomAPI_Circ::project(const boost::shared_ptr<GeomAPI_Pnt>& thePoint) const
{
  boost::shared_ptr<GeomAPI_Pnt> aResult;
  if (!MY_CIRC)
    return aResult;

  Handle(Geom_Circle) aCircle = new Geom_Circle(*MY_CIRC);

  const gp_Pnt& aPoint = thePoint->impl<gp_Pnt>();

  GeomAPI_ProjectPointOnCurve aProj(aPoint, aCircle);
  Standard_Integer aNbPoint = aProj.NbPoints();
  if (aNbPoint > 0)
  {
    double aMinDistance = 0, aDistance;
    for (Standard_Integer j = 1; j <= aNbPoint; j++)
    {
      gp_Pnt aNewPoint = aProj.Point(j);
      aDistance = aNewPoint.Distance(aPoint);
      if (!aMinDistance || aDistance < aMinDistance)
      {
        aMinDistance = aDistance;
        aResult = boost::shared_ptr<GeomAPI_Pnt>(
          new GeomAPI_Pnt(aNewPoint.X(), aNewPoint.Y(), aNewPoint.Z()));
      }
    }
  }
  return aResult;
}

