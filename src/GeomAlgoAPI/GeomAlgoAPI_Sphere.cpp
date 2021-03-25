// Copyright (C) 2017-2021  CEA/DEN, EDF R&D
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

// File:        GeomAlgoAPI_Sphere.h
// Created:     16 Mar 2017
// Author:      Clarisse Genrault (CEA)

#include <GeomAlgoAPI_Sphere.h>

#include <BRepPrimAPI_MakeSphere.hxx>

//=================================================================================================
GeomAlgoAPI_Sphere::GeomAlgoAPI_Sphere(std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                                       const double theRadius)
{
  myCenterPoint = theCenterPoint;
  myRadius = theRadius;
}

//=================================================================================================
bool GeomAlgoAPI_Sphere::check()
{
  if (!myCenterPoint) {
    myError = "Sphere builder :: center is not valid.";
    return false;
  }
  if (myRadius < Precision::Confusion()) {
    myError = "Sphere builder :: radius is negative or null.";
    return false;
  }
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Sphere::build()
{
  myCreatedFaces.clear();

  const gp_Pnt& aCenterPoint = myCenterPoint->impl<gp_Pnt>();

  // Construct the sphere
  BRepPrimAPI_MakeSphere *aSphereMaker = new BRepPrimAPI_MakeSphere(aCenterPoint, myRadius);

  aSphereMaker->Build();

  if (!aSphereMaker->IsDone()) {
    return;
  }

  TopoDS_Shape aResult = aSphereMaker->Shape();
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);

  // Test on the shapes
  if (!aShape.get() || aShape->isNull()) {
    myError = "Sphere builder :: resulting shape is null.";
    return;
  }

  setImpl(aSphereMaker);

  setDone(true);
}
