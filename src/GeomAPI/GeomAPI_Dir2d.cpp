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

#include <GeomAPI_Dir2d.h>
#include <GeomAPI_XY.h>

#include <gp_Dir2d.hxx>

#define MY_DIR implPtr<gp_Dir2d>()

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

void GeomAPI_Dir2d::reverse()
{
  MY_DIR->Reverse();
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
