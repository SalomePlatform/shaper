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

#include <GeomAPI_Box.h>
#include <GeomAPI_Ax3.h>

#include <Precision.hxx>

struct Box
{
  std::shared_ptr<GeomAPI_Ax3> myCoordSystem;
  double myWidth;
  double myDepth;
  double myHeight;

  Box(const std::shared_ptr<GeomAPI_Ax3>& theCoordSystem,
      const double theWidth,
      const double theDepth,
      const double theHeight)
    : myCoordSystem(theCoordSystem),
      myWidth(theWidth),
      myDepth(theDepth),
      myHeight(theHeight)
  {}
};

#define MY_BOX implPtr<Box>()

//=================================================================================================
GeomAPI_Box::GeomAPI_Box()
  : GeomAPI_Interface(new Box(std::shared_ptr<GeomAPI_Ax3>(new GeomAPI_Ax3), 0., 0., 0.))
{
}

//=================================================================================================
GeomAPI_Box::GeomAPI_Box(const std::shared_ptr<GeomAPI_Ax3>& theCorner,
                         const double theWidth,
                         const double theDepth,
                         const double theHeight)
  : GeomAPI_Interface(new Box(theCorner, theWidth, theDepth, theHeight))
{
}

//=================================================================================================
std::shared_ptr<GeomAPI_Ax3> GeomAPI_Box::axes() const
{
  return MY_BOX->myCoordSystem;
}

//=================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAPI_Box::corner() const
{
  return axes()->origin();
}

//=================================================================================================
double GeomAPI_Box::width() const
{
  return MY_BOX->myWidth;
}

//=================================================================================================
double GeomAPI_Box::depth() const
{
  return MY_BOX->myDepth;
}

//=================================================================================================
double GeomAPI_Box::height() const
{
  return MY_BOX->myHeight;
}

//=================================================================================================
bool GeomAPI_Box::isAxesAligned() const
{
  return Abs(MY_BOX->myCoordSystem->dirX()->x() - 1.0) < Precision::Angular() &&
         Abs(MY_BOX->myCoordSystem->normal()->z() - 1.0) < Precision::Angular();
}
