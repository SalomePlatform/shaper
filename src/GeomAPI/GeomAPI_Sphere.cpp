// Copyright (C) 2018-20xx  CEA/DEN, EDF R&D
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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#include <GeomAPI_Sphere.h>
#include <GeomAPI_Pnt.h>

#include <gp_Sphere.hxx>

#define MY_SPHERE implPtr<gp_Sphere>()

static gp_Sphere* newSphere(const gp_Pnt& theCenter, const double theRadius)
{
  return new gp_Sphere(gp_Ax3(theCenter, gp::DZ()), theRadius);
}

//=================================================================================================
GeomAPI_Sphere::GeomAPI_Sphere(const std::shared_ptr<GeomAPI_Pnt>& theCenter, double theRadius)
  : GeomAPI_Interface(newSphere(theCenter->impl<gp_Pnt>(), theRadius))
{
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Pnt> GeomAPI_Sphere::center() const
{
  const gp_Pnt& aCenter = MY_SPHERE->Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCenter.X(), aCenter.Y(), aCenter.Z()));
}

//=================================================================================================
double GeomAPI_Sphere::radius() const
{
  return MY_SPHERE->Radius();
}
