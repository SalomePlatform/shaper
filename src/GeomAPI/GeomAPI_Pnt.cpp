// File:        GeomAPI_Pnt.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Pnt.h>
#include<GeomAPI_XYZ.h>
#include<GeomAPI_Pnt2d.h>
#include<GeomAPI_Dir.h>

#include<gp_Pnt.hxx>

#define MY_PNT static_cast<gp_Pnt*>(myImpl)

GeomAPI_Pnt::GeomAPI_Pnt(const double theX, const double theY, const double theZ)
    : GeomAPI_Interface(new gp_Pnt(theX, theY, theZ))
{
}

GeomAPI_Pnt::GeomAPI_Pnt(const boost::shared_ptr<GeomAPI_XYZ>& theCoords)
    : GeomAPI_Interface(new gp_Pnt(theCoords->x(), theCoords->y(), theCoords->z()))
{
}

double GeomAPI_Pnt::x() const
{
  return MY_PNT->X();
}

double GeomAPI_Pnt::y() const
{
  return MY_PNT->Y();
}

double GeomAPI_Pnt::z() const
{
  return MY_PNT->Z();
}

void GeomAPI_Pnt::setX(const double theX)
{
  return MY_PNT->SetX(theX);
}

void GeomAPI_Pnt::setY(const double theY)
{
  return MY_PNT->SetY(theY);
}

void GeomAPI_Pnt::setZ(const double theZ)
{
  return MY_PNT->SetZ(theZ);
}

const boost::shared_ptr<GeomAPI_XYZ> GeomAPI_Pnt::xyz()
{
  return boost::shared_ptr<GeomAPI_XYZ>(new GeomAPI_XYZ(MY_PNT->X(), MY_PNT->Y(), MY_PNT->Z()));
}

double GeomAPI_Pnt::distance(const boost::shared_ptr<GeomAPI_Pnt>& theOther) const
{
  return MY_PNT->Distance(theOther->impl<gp_Pnt>());
}

boost::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Pnt::to2D(const boost::shared_ptr<GeomAPI_Pnt>& theOrigin,
  const boost::shared_ptr<GeomAPI_Dir>& theDirX, const boost::shared_ptr<GeomAPI_Dir>& theDirY)
{
  gp_Pnt anOriginPnt(theOrigin->x(), theOrigin->y(), theOrigin->z());
  gp_Vec aVec(anOriginPnt, impl<gp_Pnt>());

  double aX = aVec.X() * theDirX->x() + aVec.Y() * theDirX->y() + aVec.Z() * theDirX->z();
  double aY = aVec.X() * theDirY->x() + aVec.Y() * theDirY->y() + aVec.Z() * theDirY->z();
  return boost::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, aY));
}
