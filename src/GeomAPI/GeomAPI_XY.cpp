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

#include<GeomAPI_XY.h>

#include<gp_XY.hxx>

#define MY_XY implPtr<gp_XY>()

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

