// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include <GeomAPI_Circ2d.h>
#include <GeomAPI_Pnt2d.h>
#include <GeomAPI_Dir2d.h>

#include <gp_Circ2d.hxx>
#include <gp_Pnt2d.hxx>
#include <GeomLib_Tool.hxx>
#include <Geom2d_Circle.hxx>
#include <Precision.hxx>

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

  gp_Dir2d aDir(thePointX - theCenterX, thePointY - theCenterY);

  return newCirc2d(theCenterX, theCenterY, aDir, aRadius);
}


GeomAPI_Circ2d::GeomAPI_Circ2d(const double theCenterX,
                               const double theCenterY,
                               const double theRadius)
  : GeomAPI_Interface(newCirc2d(theCenterX, theCenterY, gp::DX2d(), theRadius))
{
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
  return GeomLib_Tool::Parameter(aCurve, thePoint->impl<gp_Pnt2d>(),
                                 theTolerance, theParameter) == Standard_True;
}

//=================================================================================================
void GeomAPI_Circ2d::D0(const double theU, std::shared_ptr<GeomAPI_Pnt2d>& thePoint)
{
  Handle(Geom2d_Circle) aCurve = new Geom2d_Circle(*MY_CIRC2D);
  gp_Pnt2d aPnt;
  aCurve->D0(theU, aPnt);
  thePoint.reset(new GeomAPI_Pnt2d(aPnt.X(), aPnt.Y()));
}

