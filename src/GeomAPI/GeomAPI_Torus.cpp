// Copyright (C) 2018-2021  CEA/DEN, EDF R&D
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

#include <GeomAPI_Torus.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

#include <gp_Torus.hxx>

#define MY_TORUS implPtr<gp_Torus>()

static gp_Torus* newTorus(const gp_Pnt& theCenter, const gp_Dir& theDir,
                          const double theMajorRadius, const double theMinorRadius)
{
  return new gp_Torus(gp_Ax3(theCenter, theDir), theMajorRadius, theMinorRadius);
}

//=================================================================================================
GeomAPI_Torus::GeomAPI_Torus(const std::shared_ptr<GeomAPI_Pnt>& theCenter,
                             const std::shared_ptr<GeomAPI_Dir>& theDir,
                             const double theMajorRadius,
                             const double theMinorRadius)
  : GeomAPI_Interface(
      newTorus(theCenter->impl<gp_Pnt>(), theDir->impl<gp_Dir>(), theMajorRadius, theMinorRadius))
{
}

//=================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAPI_Torus::center() const
{
  const gp_Pnt& aCenter = MY_TORUS->Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCenter.X(), aCenter.Y(), aCenter.Z()));
}

//=================================================================================================
std::shared_ptr<GeomAPI_Dir> GeomAPI_Torus::direction() const
{
  const gp_Dir& aDir = MY_TORUS->Axis().Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}

//=================================================================================================
double GeomAPI_Torus::majorRadius() const
{
  return MY_TORUS->MajorRadius();
}

//=================================================================================================
double GeomAPI_Torus::minorRadius() const
{
  return MY_TORUS->MinorRadius();
}
