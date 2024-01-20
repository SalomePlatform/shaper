// Copyright (C) 2014-2024  CEA, EDF
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

#include<GeomAPI_Pnt2d.h>
#include<GeomAPI_XY.h>
#include<GeomAPI_XYZ.h>
#include<GeomAPI_Pnt.h>
#include<GeomAPI_Dir.h>

#include<gp_Pnt2d.hxx>

#include <Precision.hxx>

#define MY_PNT2D implPtr<gp_Pnt2d>()

GeomAPI_Pnt2d::GeomAPI_Pnt2d(const double theX, const double theY)
    : GeomAPI_Interface(new gp_Pnt2d(theX, theY))
{
}

GeomAPI_Pnt2d::GeomAPI_Pnt2d(const std::shared_ptr<GeomAPI_XY>& theCoords)
    : GeomAPI_Interface(new gp_Pnt2d(theCoords->x(), theCoords->y()))
{
}

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

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Pnt2d::to3D(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                                                   const std::shared_ptr<GeomAPI_Dir>& theDirX,
                                                   const std::shared_ptr<GeomAPI_Dir>& theDirY)
{
  std::shared_ptr<GeomAPI_XYZ> aSum = theOrigin->xyz()->added(theDirX->xyz()->multiplied(x()))
      ->added(theDirY->xyz()->multiplied(y()));

  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aSum));
}

const std::shared_ptr<GeomAPI_XY> GeomAPI_Pnt2d::xy()
{
  return std::shared_ptr<GeomAPI_XY>(new GeomAPI_XY(MY_PNT2D->X(), MY_PNT2D->Y()));
}

double GeomAPI_Pnt2d::distance(const std::shared_ptr<GeomAPI_Pnt2d>& theOther) const
{
  return MY_PNT2D->Distance(theOther->impl<gp_Pnt2d>());
}

bool GeomAPI_Pnt2d::isEqual(const std::shared_ptr<GeomAPI_Pnt2d>& theOther) const
{
  return distance(theOther) < Precision::Confusion();
}
