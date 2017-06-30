// Copyright (C) 2017  CEA/DEN, EDF R&D
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

// File:        GeomAPI_Ellipse.cpp
// Created:     25 April 2017
// Author:      Vitaly Smetannikov

#include "GeomAPI_Ellipse.h"
#include "GeomAPI_Ax2.h"
#include "GeomAPI_Pnt.h"

#include <gp_Elips.hxx>

#define MY_ELIPS implPtr<gp_Elips>()

GeomAPI_Ellipse::GeomAPI_Ellipse(const std::shared_ptr<GeomAPI_Ax2>& theAx2,
                                 double theMajorRadius, double theMinorRadius)
: GeomAPI_Interface(new gp_Elips(theAx2->impl<gp_Ax2>(), theMajorRadius, theMinorRadius))
{
}

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Ellipse::center() const
{
  const gp_Pnt& aCenter = MY_ELIPS->Location();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCenter.X(), aCenter.Y(), aCenter.Z()));
}

GeomPointPtr GeomAPI_Ellipse::firstFocus() const
{
  const gp_Pnt& aFirst = MY_ELIPS->Focus1();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aFirst.X(), aFirst.Y(), aFirst.Z()));
}

GeomPointPtr GeomAPI_Ellipse::secondFocus() const
{
  const gp_Pnt& aSecond = MY_ELIPS->Focus2();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aSecond.X(), aSecond.Y(), aSecond.Z()));
}

double GeomAPI_Ellipse::minorRadius() const
{
  return MY_ELIPS->MinorRadius();
}

double GeomAPI_Ellipse::majorRadius() const
{
  return MY_ELIPS->MajorRadius();
}
