// File:        GeomAPI_XYZ.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_XYZ.h>

#include<gp_XYZ.hxx>

#define MY_XYZ static_cast<gp_XYZ*>(myImpl)

GeomAPI_XYZ::GeomAPI_XYZ(const double theX, const double theY, const double theZ)
    : GeomAPI_Interface(new gp_XYZ(theX, theY, theZ))
{
}

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

const std::shared_ptr<GeomAPI_XYZ> GeomAPI_XYZ::added(
    const std::shared_ptr<GeomAPI_XYZ>& theArg)
{
  std::shared_ptr<GeomAPI_XYZ> aResult(new GeomAPI_XYZ(MY_XYZ->X() + theArg->x(),
  MY_XYZ->Y() + theArg->y(), MY_XYZ->Z() + theArg->z()));
  return aResult;
}

const std::shared_ptr<GeomAPI_XYZ> GeomAPI_XYZ::decreased(
    const std::shared_ptr<GeomAPI_XYZ>& theArg)
{
  std::shared_ptr<GeomAPI_XYZ> aResult(new GeomAPI_XYZ(MY_XYZ->X() - theArg->x(),
  MY_XYZ->Y() - theArg->y(), MY_XYZ->Z() - theArg->z()));
  return aResult;
}

const std::shared_ptr<GeomAPI_XYZ> GeomAPI_XYZ::multiplied(const double theArg)
{
  std::shared_ptr<GeomAPI_XYZ> aResult(new GeomAPI_XYZ(MY_XYZ->X() * theArg,
  MY_XYZ->Y() * theArg, MY_XYZ->Z() * theArg));
  return aResult;
}

double GeomAPI_XYZ::dot(const std::shared_ptr<GeomAPI_XYZ>& theArg) const
{
  return MY_XYZ->Dot(theArg->impl<gp_XYZ>());
}

const std::shared_ptr<GeomAPI_XYZ> GeomAPI_XYZ::cross(
    const std::shared_ptr<GeomAPI_XYZ>& theArg) const
{
  gp_XYZ aResult = MY_XYZ->Crossed(theArg->impl<gp_XYZ>());
  return std::shared_ptr<GeomAPI_XYZ>(new GeomAPI_XYZ(aResult.X(), aResult.Y(), aResult.Z()));
}

double GeomAPI_XYZ::distance(const std::shared_ptr<GeomAPI_XYZ>& theOther) const
{
  gp_XYZ aResult(theOther->x() - x(), theOther->y() - y(), theOther->z() - z());
  return aResult.Modulus();
}

