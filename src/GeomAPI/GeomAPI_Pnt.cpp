// File:        GeomAPI_Pnt.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Pnt.h>

#include<gp_Pnt.hxx>

#define MY_PNT static_cast<gp_Pnt*>(myImpl)

GeomAPI_Pnt::GeomAPI_Pnt(const double theX, const double theY, const double theZ)
  : GeomAPI_Interface(new gp_Pnt(theX, theY, theZ))
{}

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
