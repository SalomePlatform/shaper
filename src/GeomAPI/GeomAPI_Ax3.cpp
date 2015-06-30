// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Ax3.cpp
// Created:     16 February 2015
// Author:      Vitaly SMETANNIKOV


#include "GeomAPI_Ax3.h"
#include "GeomAPI_XYZ.h"
#include "GeomAPI_Pnt2d.h"

#include <gp_Dir.hxx>
#include <gp_Pnt.hxx>
#include <gp_Ax3.hxx>
#include <Precision.hxx>


#define MY_AX3 implPtr<gp_Ax3>()


GeomAPI_Ax3::GeomAPI_Ax3()
: GeomAPI_Interface(new gp_Ax3())
{
}

GeomAPI_Ax3::GeomAPI_Ax3(std::shared_ptr<GeomAPI_Pnt> theOrigin,
                         std::shared_ptr<GeomAPI_Dir> theDirX,
                         std::shared_ptr<GeomAPI_Dir> theNorm)
: GeomAPI_Interface(new gp_Ax3(theOrigin->impl<gp_Pnt>(), 
                               theNorm->impl<gp_Dir>(), 
                               theDirX->impl<gp_Dir>()))
 {
 }

void GeomAPI_Ax3::setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin)
{
  gp_Ax1 aAx1 = MY_AX3->Axis();
  aAx1.SetLocation(theOrigin->impl<gp_Pnt>());
  MY_AX3->SetAxis(aAx1);
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Ax3::origin() const
{
  gp_Pnt aPnt = MY_AX3->Axis().Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPnt.X(),aPnt.Y(),aPnt.Z()));
}

void GeomAPI_Ax3::setDirX(const std::shared_ptr<GeomAPI_Dir>& theDirX)
{
  MY_AX3->SetXDirection(theDirX->impl<gp_Dir>());
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_Ax3::dirX() const
{
  gp_Dir aDir = MY_AX3->XDirection();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}

void GeomAPI_Ax3::setDirY(const std::shared_ptr<GeomAPI_Dir>& theDirY)
{
  MY_AX3->SetYDirection(theDirY->impl<gp_Dir>());
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_Ax3::dirY() const
{
  gp_Dir aDir = MY_AX3->YDirection();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}

void GeomAPI_Ax3::setNorm(const std::shared_ptr<GeomAPI_Dir>& theNorm)
{
  gp_Ax1 aAx1 = MY_AX3->Axis();
  aAx1.SetDirection(theNorm->impl<gp_Dir>());
  MY_AX3->SetAxis(aAx1);
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_Ax3::norm() const
{
  gp_Dir aDir = MY_AX3->Axis().Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(),aDir.Y(),aDir.Z()));
}


std::shared_ptr<GeomAPI_Pnt> GeomAPI_Ax3::to3D(double theX, double theY) const
{
  gp_Pnt aPnt = MY_AX3->Axis().Location();
  gp_Dir aXDir = MY_AX3->XDirection();
  gp_Dir aYDir = MY_AX3->YDirection();
  gp_XYZ aSum = aPnt.XYZ().Added(aXDir.XYZ().Multiplied(theX)).Added(aYDir.XYZ().Multiplied(theY));

  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aSum.X(), aSum.Y(), aSum.Z()));
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Ax3::to2D(double theX, double theY, double theZ) const
{
  gp_Pnt anOriginPnt = MY_AX3->Axis().Location();
  gp_Vec aVec(anOriginPnt, gp_Pnt(0, 0, 0));

  gp_Dir aXDir = MY_AX3->XDirection();
  gp_Dir aYDir = MY_AX3->YDirection();

  double aX = aVec.X() * aXDir.X() + aVec.Y() * aXDir.Y() + aVec.Z() * aXDir.Z();
  double aY = aVec.X() * aYDir.X() + aVec.Y() * aYDir.Y() + aVec.Z() * aYDir.Y();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, aY));
}
