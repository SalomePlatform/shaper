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

#include <GeomAPI_Cylinder.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

#include <gp_Cylinder.hxx>
#include <Precision.hxx>

#define MY_CYL implPtr<gp_Cylinder>()

static gp_Cylinder* newCyl(const gp_Pnt& theLocation, const gp_Dir& theAxis, const double theRadius)
{
  return new gp_Cylinder(gp_Ax3(theLocation, theAxis), theRadius);
}

//=================================================================================================
GeomAPI_Cylinder::GeomAPI_Cylinder(const std::shared_ptr<GeomAPI_Pnt>& theLocation,
                                   const std::shared_ptr<GeomAPI_Dir>& theAxis,
                                   const double theRadius)
  : GeomAPI_Interface(newCyl(theLocation->impl<gp_Pnt>(), theAxis->impl<gp_Dir>(), theRadius)),
    myHeight(Precision::Infinite())
{
}

//=================================================================================================
GeomAPI_Cylinder::GeomAPI_Cylinder(const std::shared_ptr<GeomAPI_Pnt>& theLocation,
                                   const std::shared_ptr<GeomAPI_Dir>& theAxis,
                                   const double theRadius,
                                   const double theHeight)
  : GeomAPI_Interface(newCyl(theLocation->impl<gp_Pnt>(), theAxis->impl<gp_Dir>(), theRadius)),
    myHeight(theHeight)
{
}

//=================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAPI_Cylinder::location() const
{
  const gp_Pnt& aCenter = MY_CYL->Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCenter.X(), aCenter.Y(), aCenter.Z()));
}

//=================================================================================================
std::shared_ptr<GeomAPI_Dir> GeomAPI_Cylinder::axis() const
{
  const gp_Dir& anAxis = MY_CYL->Position().Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(anAxis.X(), anAxis.Y(), anAxis.Z()));
}

//=================================================================================================
double GeomAPI_Cylinder::radius() const
{
  return MY_CYL->Radius();
}

//=================================================================================================
double GeomAPI_Cylinder::height() const
{
  return myHeight;
}

//=================================================================================================
bool GeomAPI_Cylinder::isInfinite() const
{
  return Precision::IsInfinite(myHeight);
}

//=================================================================================================
bool GeomAPI_Cylinder::isCoincident(const GeomCylinderPtr theCylinder, const double theTolerance)
{
  if (!theCylinder)
    return false;

  gp_Cylinder* anOther = theCylinder->implPtr<gp_Cylinder>();
  if (fabs(MY_CYL->Radius() - anOther->Radius()) < theTolerance) {
    gp_Dir aDir1 = MY_CYL->Position().Direction();
    gp_Dir aDir2 = anOther->Position().Direction();
    if (aDir1.IsParallel(aDir2, Precision::Angular())) {
      gp_Pnt aLoc1 = MY_CYL->Location();
      gp_Pnt aLoc2 = anOther->Location();
      gp_Vec aVec12(aLoc1, aLoc2);
      if (aVec12.SquareMagnitude() < theTolerance * theTolerance ||
          aVec12.IsParallel(aDir1, Precision::Angular())) {
        return true;
      }
    }
  }
  return false;
}
