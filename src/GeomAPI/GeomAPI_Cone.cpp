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

#include <GeomAPI_Cone.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Dir.h>

#include <gp_Cone.hxx>
#include <Precision.hxx>

#define MY_CONE implPtr<gp_Cone>()

static gp_Cone* newCone(const gp_Pnt& theApex, const gp_Dir& theAxis,
                        const double theSemiAngle, const double theRadius = 0.0)
{
  return new gp_Cone(gp_Ax3(theApex, theAxis), theSemiAngle, theRadius);
}

//=================================================================================================
GeomAPI_Cone::GeomAPI_Cone(const std::shared_ptr<GeomAPI_Pnt>& theApex,
                           const std::shared_ptr<GeomAPI_Dir>& theAxis,
                           const double theSemiAngle)
  : GeomAPI_Interface(newCone(theApex->impl<gp_Pnt>(), theAxis->impl<gp_Dir>(), theSemiAngle)),
    myRadius1(Precision::Infinite()),
    myRadius2(Precision::Infinite())
{
}

//=================================================================================================
GeomAPI_Cone::GeomAPI_Cone(const std::shared_ptr<GeomAPI_Pnt>& theLocation,
                           const std::shared_ptr<GeomAPI_Dir>& theAxis,
                           const double theSemiAngle,
                           const double theRadius)
  : GeomAPI_Interface(
      newCone(theLocation->impl<gp_Pnt>(), theAxis->impl<gp_Dir>(), theSemiAngle, theRadius)),
    myRadius1(theRadius),
    myRadius2(Precision::Infinite())
{
}

//=================================================================================================
GeomAPI_Cone::GeomAPI_Cone(const std::shared_ptr<GeomAPI_Pnt>& theLocation,
                           const std::shared_ptr<GeomAPI_Dir>& theAxis,
                           const double theSemiAngle,
                           const double theRadius1,
                           const double theRadius2)
  : myRadius1(theRadius1),
    myRadius2(theRadius2)
{
  gp_Pnt aLoc = theLocation->impl<gp_Pnt>();
  gp_Dir aDir = theAxis->impl<gp_Dir>();
  if (theRadius1 > theRadius2) {
    aLoc.ChangeCoord() += aDir.XYZ() * (theRadius1 - theRadius2) / Tan(theSemiAngle);
    aDir.Reverse();
    myRadius1 = theRadius2;
    myRadius2 = theRadius1;
  }

  setImpl(newCone(aLoc, aDir, theSemiAngle, myRadius1));
}

//=================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAPI_Cone::apex() const
{
  const gp_Pnt& anApex = MY_CONE->Apex();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(anApex.X(), anApex.Y(), anApex.Z()));
}

//=================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAPI_Cone::location() const
{
  const gp_Pnt& aLoc = MY_CONE->Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
}

//=================================================================================================
std::shared_ptr<GeomAPI_Dir> GeomAPI_Cone::axis() const
{
  const gp_Dir& anAxis = MY_CONE->Position().Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(anAxis.X(), anAxis.Y(), anAxis.Z()));
}

//=================================================================================================
double GeomAPI_Cone::semiAngle() const
{
  return MY_CONE->SemiAngle();
}

//=================================================================================================
double GeomAPI_Cone::radius1() const
{
  return myRadius1;
}

//=================================================================================================
double GeomAPI_Cone::radius2() const
{
  return myRadius2;
}

//=================================================================================================
double GeomAPI_Cone::height() const
{
  if (Precision::IsInfinite(myRadius1) || Precision::IsInfinite(myRadius2))
    return Precision::Infinite();

  return Abs((myRadius2 - myRadius1) / Tan(semiAngle()));
}

//=================================================================================================
bool GeomAPI_Cone::isSemiInfinite() const
{
  return (!Precision::IsInfinite(myRadius1) && Precision::IsInfinite(myRadius2)) ||
         (Precision::IsInfinite(myRadius1) && !Precision::IsInfinite(myRadius2));
}

//=================================================================================================
bool GeomAPI_Cone::isInfinite() const
{
  return Precision::IsInfinite(myRadius1) && Precision::IsInfinite(myRadius2);
}
