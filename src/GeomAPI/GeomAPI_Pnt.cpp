// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include<GeomAPI_Pnt.h>
#include <GeomAPI_Ax1.h>
#include<GeomAPI_XYZ.h>
#include<GeomAPI_Pnt2d.h>
#include<GeomAPI_Dir.h>
#include<GeomAPI_Pln.h>

#include<gp_Pnt.hxx>
#include<gp_Pln.hxx>
#include<ProjLib.hxx>

#define MY_PNT implPtr<gp_Pnt>()

GeomAPI_Pnt::GeomAPI_Pnt(const double theX, const double theY, const double theZ)
    : GeomAPI_Interface(new gp_Pnt(theX, theY, theZ))
{
}

GeomAPI_Pnt::GeomAPI_Pnt(const std::shared_ptr<GeomAPI_XYZ>& theCoords)
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

const std::shared_ptr<GeomAPI_XYZ> GeomAPI_Pnt::xyz()
{
  return std::shared_ptr<GeomAPI_XYZ>(new GeomAPI_XYZ(MY_PNT->X(), MY_PNT->Y(), MY_PNT->Z()));
}

double GeomAPI_Pnt::distance(const std::shared_ptr<GeomAPI_Pnt>& theOther) const
{
  return MY_PNT->Distance(theOther->impl<gp_Pnt>());
}

bool GeomAPI_Pnt::isEqual(const std::shared_ptr<GeomAPI_Pnt>& theOther) const
{
  return distance(theOther) < Precision::Confusion();
}

bool GeomAPI_Pnt::isLess(const std::shared_ptr<GeomAPI_Pnt>& theOther,
                         const double theTolerance) const
{
  if (MY_PNT->X() + theTolerance < theOther->x())
    return true;
  else if (MY_PNT->X() < theOther->x() + theTolerance) {
    if (MY_PNT->Y() + theTolerance < theOther->y())
      return true;
    else if (MY_PNT->Y() < theOther->y() + theTolerance &&
             MY_PNT->Z() + theTolerance < theOther->z())
      return true;
  }
  return false;
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Pnt::to2D(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
  const std::shared_ptr<GeomAPI_Dir>& theDirX, const std::shared_ptr<GeomAPI_Dir>& theDirY)
{
  gp_Pnt anOriginPnt(theOrigin->x(), theOrigin->y(), theOrigin->z());
  gp_Vec aVec(anOriginPnt, impl<gp_Pnt>());

  double aX = aVec.X() * theDirX->x() + aVec.Y() * theDirX->y() + aVec.Z() * theDirX->z();
  double aY = aVec.X() * theDirY->x() + aVec.Y() * theDirY->y() + aVec.Z() * theDirY->z();
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aX, aY));
}


void GeomAPI_Pnt::translate(const std::shared_ptr<GeomAPI_Dir>& theDir, double theDist)
{
  gp_Vec aVec(theDir->impl<gp_Dir>());
  aVec.Normalize();
  aVec.Multiply(theDist);
  MY_PNT->Translate(aVec);
}

std::shared_ptr<GeomAPI_Pnt2d> GeomAPI_Pnt::to2D(const std::shared_ptr<GeomAPI_Pln>& thePln) const
{
  double aA, aB, aC, aD;
  thePln->coefficients(aA, aB, aC, aD);
  gp_Pln aPln(aA, aB, aC, aD);

  gp_Pnt2d aRes = ProjLib::Project(aPln, *MY_PNT);
  return std::shared_ptr<GeomAPI_Pnt2d>(new GeomAPI_Pnt2d(aRes.X(), aRes.Y()));
}

void GeomAPI_Pnt::rotate(const std::shared_ptr<GeomAPI_Ax1>& theAxis, const double theAngle)
{
  MY_PNT->Rotate(theAxis->impl<gp_Ax1>(), theAngle / 180.0 * M_PI);
}
