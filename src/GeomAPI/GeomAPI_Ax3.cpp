// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

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

void GeomAPI_Ax3::setNormal(const std::shared_ptr<GeomAPI_Dir>& theNorm)
{
  gp_Ax1 aAx1 = MY_AX3->Axis();
  aAx1.SetDirection(theNorm->impl<gp_Dir>());
  MY_AX3->SetAxis(aAx1);
}

std::shared_ptr<GeomAPI_Dir> GeomAPI_Ax3::normal() const
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
