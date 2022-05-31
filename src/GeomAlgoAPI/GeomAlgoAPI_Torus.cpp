// Copyright (C) 2017-2022  CEA/DEN, EDF R&D
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

// File:        GeomAlgoAPI_Torus.cpp
// Created:     20 Mar 2017
// Author:      Clarisse Genrault (CEA)

#include <GeomAlgoAPI_Torus.h>

#include <gp_Ax2.hxx>

#include <BRepPrimAPI_MakeTorus.hxx>

//=================================================================================================
GeomAlgoAPI_Torus::GeomAlgoAPI_Torus(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                     const double theRadius,
                                     const double theRingRadius)
{
  myAxis = theAxis;
  myRadius = theRadius;
  myRingRadius = theRingRadius;
}

//=================================================================================================
bool GeomAlgoAPI_Torus::check()
{
  if (!myAxis) {
    myError = "Torus builder :: axis is not valid.";
    return false;
  } else if (myRadius < Precision::Confusion()) {
    myError = "Torus builder :: radius is negative or null.";
    return false;
  } else if (myRingRadius < Precision::Confusion()) {
    myError = "Torus builder :: ring radius is negative or null.";
    return false;
  } else if (myRadius < myRingRadius) {
    myError = "Torus builder :: ring radius is greater than the radius.";
    return false;
  }
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Torus::build()
{
  myCreatedFaces.clear();

  const gp_Ax2& anAxis = myAxis->impl<gp_Ax2>();

  // Construct the torus
  BRepPrimAPI_MakeTorus *aTorusMaker =
    new BRepPrimAPI_MakeTorus(anAxis, myRadius, myRingRadius);

  aTorusMaker->Build();

  if (!aTorusMaker->IsDone()) {
    return;
  }

  TopoDS_Shape aResult = aTorusMaker->Shape();
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);

  // Test on the shapes
  if (!aShape.get() || aShape->isNull()) {
    myError = "Torus builder :: resulting shape is null.";
    return;
  }

  setImpl(aTorusMaker);

  setDone(true);
}
