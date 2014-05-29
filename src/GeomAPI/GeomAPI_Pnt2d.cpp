// File:        GeomAPI_Pnt2d.cpp
// Created:     29 May 2014
// Author:      Artem ZHIDKOV

#include<GeomAPI_Pnt2d.h>
#include<GeomAPI_XYZ.h>

#include<gp_Pnt2d.hxx>

#define MY_PNT2D static_cast<gp_Pnt2d*>(myImpl)

GeomAPI_Pnt2d::GeomAPI_Pnt2d(const double theX, const double theY)
  : GeomAPI_Interface(new gp_Pnt2d(theX, theY))
{}

double GeomAPI_Pnt2d::x() const
{
  return MY_PNT2D->X();
}

double GeomAPI_Pnt2d::y() const
{
  return MY_PNT2D->Y();
}

void GeomAPI_Pnt2d::setX(const double theX)
{
  return MY_PNT2D->SetX(theX);
}

void GeomAPI_Pnt2d::setY(const double theY)
{
  return MY_PNT2D->SetY(theY);
}

double GeomAPI_Pnt2d::distance(const boost::shared_ptr<GeomAPI_Pnt2d>& theOther) const
{
  return MY_PNT2D->Distance(theOther->impl<gp_Pnt2d>());
}
