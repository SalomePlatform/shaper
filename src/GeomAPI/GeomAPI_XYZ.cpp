// File:        GeomAPI_XYZ.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_XYZ.h>

#include<gp_XYZ.hxx>

#define MY_XYZ static_cast<gp_XYZ*>(myImpl)

GeomAPI_XYZ::GeomAPI_XYZ(const double theX, const double theY, const double theZ)
  : GeomAPI_Interface(new gp_XYZ(theX, theY, theZ))
{}

double GeomAPI_XYZ::x() const
{
  return MY_XYZ->X();
}

double GeomAPI_XYZ::y() const
{
  return MY_XYZ->Y();
}

double GeomAPI_XYZ::z() const
{
  return MY_XYZ->Z();
}

void GeomAPI_XYZ::setX(const double theX)
{
  return MY_XYZ->SetX(theX);
}

void GeomAPI_XYZ::setY(const double theY)
{
  return MY_XYZ->SetY(theY);
}

void GeomAPI_XYZ::setZ(const double theZ)
{
  return MY_XYZ->SetZ(theZ);
}

const boost::shared_ptr<GeomAPI_XYZ> GeomAPI_XYZ::added(
  const boost::shared_ptr<GeomAPI_XYZ>& theArg)
{
  boost::shared_ptr<GeomAPI_XYZ> aResult(new GeomAPI_XYZ(MY_XYZ->X() + theArg->x(),
    MY_XYZ->Y() + theArg->y(), MY_XYZ->Z() + theArg->z()));
  return aResult;
}

const boost::shared_ptr<GeomAPI_XYZ> GeomAPI_XYZ::multiplied(const double theArg)
{
  boost::shared_ptr<GeomAPI_XYZ> aResult(new GeomAPI_XYZ(MY_XYZ->X() * theArg,
    MY_XYZ->Y() * theArg, MY_XYZ->Z() * theArg));
  return aResult;
}
