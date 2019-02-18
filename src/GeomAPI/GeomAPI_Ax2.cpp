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

#include <GeomAPI_Ax2.h>

#include <gp_Ax2.hxx>

#define MY_AX1 implPtr<gp_Ax2>()

//=================================================================================================
GeomAPI_Ax2::GeomAPI_Ax2()
: GeomAPI_Interface(new gp_Ax2())
{
}

//=================================================================================================
GeomAPI_Ax2::GeomAPI_Ax2(std::shared_ptr<GeomAPI_Pnt> theOrigin,
                         std::shared_ptr<GeomAPI_Dir> theN,
                         std::shared_ptr<GeomAPI_Dir> theVX)
: GeomAPI_Interface(new gp_Ax2(theOrigin->impl<gp_Pnt>(),
                               theN->impl<gp_Dir>(),
                               theVX->impl<gp_Dir>()))
{
}

//=================================================================================================
GeomAPI_Ax2::GeomAPI_Ax2(std::shared_ptr<GeomAPI_Pnt> theOrigin,
                         std::shared_ptr<GeomAPI_Dir> theDir)
: GeomAPI_Interface(new gp_Ax2(theOrigin->impl<gp_Pnt>(),
                               theDir->impl<gp_Dir>()))
{
}

//=================================================================================================
void GeomAPI_Ax2::setOrigin(const std::shared_ptr<GeomAPI_Pnt>& theOrigin)
{
  MY_AX1->SetLocation(theOrigin->impl<gp_Pnt>());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAPI_Ax2::origin() const
{
  gp_Pnt aPnt = MY_AX1->Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPnt.X(),aPnt.Y(),aPnt.Z()));
}

//=================================================================================================
void GeomAPI_Ax2::setDir(const std::shared_ptr<GeomAPI_Dir>& theDir)
{
  MY_AX1->SetDirection(theDir->impl<gp_Dir>());
}

//=================================================================================================
std::shared_ptr<GeomAPI_Dir> GeomAPI_Ax2::dir() const
{
  gp_Dir aDir = MY_AX1->Direction();
  return std::shared_ptr<GeomAPI_Dir>(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
}
