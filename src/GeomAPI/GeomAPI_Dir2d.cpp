// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Dir2d.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>

#include <gp_Dir2d.hxx>

#define MY_DIR static_cast<gp_Dir2d*>(myImpl)

GeomAPI_Dir2d::GeomAPI_Dir2d(const double theX, const double theY)
    : GeomAPI_Interface(new gp_Dir2d(theX, theY))
{
}

GeomAPI_Dir2d::GeomAPI_Dir2d(const std::shared_ptr<GeomAPI_XY>& theCoords)
    : GeomAPI_Interface(new gp_Dir2d(theCoords->x(), theCoords->y()))
{
}

double GeomAPI_Dir2d::x() const
{
  return MY_DIR->X();
}

double GeomAPI_Dir2d::y() const
{
  return MY_DIR->Y();
}

const std::shared_ptr<GeomAPI_XY> GeomAPI_Dir2d::xy()
{
  return std::shared_ptr<GeomAPI_XY>(new GeomAPI_XY(MY_DIR->X(), MY_DIR->Y()));
}

double GeomAPI_Dir2d::dot(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const
{
  return MY_DIR->Dot(theArg->impl<gp_Dir2d>());
}

double GeomAPI_Dir2d::cross(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const
{
  return MY_DIR->XY().Crossed(theArg->impl<gp_Dir2d>().XY());
}

double GeomAPI_Dir2d::angle(const std::shared_ptr<GeomAPI_Dir2d>& theArg) const
{
  return MY_DIR->Angle(theArg->impl<gp_Dir2d>());
}
