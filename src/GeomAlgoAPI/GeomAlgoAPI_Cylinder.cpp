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

#include <GeomAlgoAPI_Cylinder.h>

#include <gp_Ax2.hxx>
#include <gp_Dir.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Shape.hxx>

#include <BRepPrimAPI_MakeCylinder.hxx>

//=================================================================================================
GeomAlgoAPI_Cylinder::GeomAlgoAPI_Cylinder(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                           const double theRadius,
                                           const double theHeight)
{
  withAngle = false;
  myAxis = theAxis;
  myRadius = theRadius;
  myHeight = theHeight;
}

//=================================================================================================
GeomAlgoAPI_Cylinder::GeomAlgoAPI_Cylinder(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                           const double theRadius,
                                           const double theHeight,
                                           const double theAngle)
{
  withAngle = true;
  myAxis = theAxis;
  myRadius = theRadius;
  myHeight = theHeight;
  myAngle = theAngle;
}

//=================================================================================================
bool GeomAlgoAPI_Cylinder::check()
{
  if (!myAxis) {
    myError = "Cylinder builder :: axis is not valid.";
    return false;
  }
  if (myRadius < Precision::Confusion()) {
    myError = "Cylinder builder :: radius is negative or null.";
    return false;
  }
  if (myHeight < Precision::Confusion()) {
    myError = "Cylinder builder :: height is negative or null.";
    return false;
  }
  if (withAngle) {
    if (myAngle < Precision::Angular() * 180./M_PI) {
      myError = "Cylinder builder :: angle is negative or null.";
      return false;
    }
    if (myAngle > 360.) {
      myError = "Cylinder builder :: angle greater than 360 degrees.";
      return false;
    }
  }
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Cylinder::build()
{
  myCreatedFaces.clear();

  const gp_Ax2& anAxis = myAxis->impl<gp_Ax2>();

  // Construct the cylinder
  BRepPrimAPI_MakeCylinder *aCylinderMaker;

  if (withAngle) {
    aCylinderMaker =
      new BRepPrimAPI_MakeCylinder(anAxis, myRadius, myHeight, myAngle * M_PI / 180.);
  } else {
    aCylinderMaker = new BRepPrimAPI_MakeCylinder(anAxis, myRadius, myHeight);
  }

  aCylinderMaker->Build();

  if (!aCylinderMaker->IsDone()) {
    return;
  }

  TopoDS_Shape aResult = aCylinderMaker->Shape();
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);

  // Test on the shapes
  if (!aShape.get() || aShape->isNull()) {
    myError = "Cylinder builder :: resulting shape is null.";
    return;
  }

  setImpl(aCylinderMaker);

  setDone(true);
}
