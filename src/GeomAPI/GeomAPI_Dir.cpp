// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Dir.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include <GeomAPI_Dir.h>
#include <GeomAPI_XYZ.h>

#include <gp_Dir.hxx>

#define MY_DIR static_cast<gp_Dir*>(myImpl)

GeomAPI_Dir::GeomAPI_Dir(const double theX, const double theY, const double theZ)
    : GeomAPI_Interface(new gp_Dir(theX, theY, theZ))
{
}

GeomAPI_Dir::GeomAPI_Dir(const std::shared_ptr<GeomAPI_XYZ>& theCoords)
    : GeomAPI_Interface(new gp_Dir(theCoords->x(), theCoords->y(), theCoords->z()))
{
}

double GeomAPI_Dir::x() const
{
  return MY_DIR->X();
}

double GeomAPI_Dir::y() const
{
  return MY_DIR->Y();
}

double GeomAPI_Dir::z() const
{
  return MY_DIR->Z();
}

const std::shared_ptr<GeomAPI_XYZ> GeomAPI_Dir::xyz()
{
  return std::shared_ptr<GeomAPI_XYZ>(new GeomAPI_XYZ(MY_DIR->X(), MY_DIR->Y(), MY_DIR->Z()));
}

double GeomAPI_Dir::dot(const std::shared_ptr<GeomAPI_Dir>& theArg) const
{
  return MY_DIR->Dot(theArg->impl<gp_Dir>());
}

const std::shared_ptr<GeomAPI_XYZ> GeomAPI_Dir::cross(
    const std::shared_ptr<GeomAPI_Dir>& theArg) const
{
  gp_XYZ aResult = MY_DIR->XYZ().Crossed(theArg->impl<gp_Dir>().XYZ());
  return std::shared_ptr<GeomAPI_XYZ>(new GeomAPI_XYZ(aResult.X(), aResult.Y(), aResult.Z()));
}

