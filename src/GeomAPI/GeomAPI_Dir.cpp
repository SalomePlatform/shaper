// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include <GeomAPI_Dir.h>
#include <GeomAPI_XYZ.h>

#include <gp_Dir.hxx>

#define MY_DIR implPtr<gp_Dir>()

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

void GeomAPI_Dir::reverse()
{
  MY_DIR->Reverse();
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

double GeomAPI_Dir::angle(const std::shared_ptr<GeomAPI_Dir>& theArg) const
{
  return MY_DIR->Angle(theArg->impl<gp_Dir>());
}

bool GeomAPI_Dir::isParallel(const std::shared_ptr<GeomAPI_Dir> theDir,
                             const double theTolerance) const
{
  return MY_DIR->IsParallel(theDir->impl<gp_Dir>(), theTolerance) == Standard_True;
}
