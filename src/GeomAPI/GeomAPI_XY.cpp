// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_XY.cpp
// Created:     30 May 2014
// Author:      Artem ZHIDKOV

#include<GeomAPI_XY.h>

#include<gp_XY.hxx>

#define MY_XY static_cast<gp_XY*>(myImpl)

GeomAPI_XY::GeomAPI_XY(const double theX, const double theY)
    : GeomAPI_Interface(new gp_XY(theX, theY))
{
}

double GeomAPI_XY::x() const
{
  return MY_XY->X();
}

double GeomAPI_XY::y() const
{
  return MY_XY->Y();
}

void GeomAPI_XY::setX(const double theX)
{
  return MY_XY->SetX(theX);
}

void GeomAPI_XY::setY(const double theY)
{
  return MY_XY->SetY(theY);
}

const std::shared_ptr<GeomAPI_XY> GeomAPI_XY::added(const std::shared_ptr<GeomAPI_XY>& theArg)
{
  std::shared_ptr<GeomAPI_XY> aResult(new GeomAPI_XY(
      MY_XY->X() + theArg->x(), MY_XY->Y() + theArg->y()));
  return aResult;
}

const std::shared_ptr<GeomAPI_XY> GeomAPI_XY::decreased(
    const std::shared_ptr<GeomAPI_XY>& theArg)
{
  std::shared_ptr<GeomAPI_XY> aResult(new GeomAPI_XY(
      MY_XY->X() - theArg->x(), MY_XY->Y() - theArg->y()));
  return aResult;
}

const std::shared_ptr<GeomAPI_XY> GeomAPI_XY::multiplied(const double theArg)
{
  std::shared_ptr<GeomAPI_XY> aResult(new GeomAPI_XY(MY_XY->X() * theArg, MY_XY->Y() * theArg));
  return aResult;
}

double GeomAPI_XY::dot(const std::shared_ptr<GeomAPI_XY>& theArg) const
{
  return MY_XY->Dot(theArg->impl<gp_XY>());
}

double GeomAPI_XY::cross(const std::shared_ptr<GeomAPI_XY>& theArg) const
{
  return MY_XY->Crossed(theArg->impl<gp_XY>());
}

double GeomAPI_XY::distance(const std::shared_ptr<GeomAPI_XY>& theOther) const
{
  gp_XY aResult(theOther->x() - x(), theOther->y() - y());
  return aResult.Modulus();
}

