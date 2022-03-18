// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <GeomAlgoAPI_Tube.h>

#include <BRepAlgo_FaceRestrictor.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_Transform.hxx>

#include <BRepPrimAPI_MakePrism.hxx>

#include <gp_Circ.hxx>

#include <TopoDS_Wire.hxx>
#include <TopoDS_Face.hxx>

//=================================================================================================
GeomAlgoAPI_Tube::GeomAlgoAPI_Tube()
{
}

//=================================================================================================
GeomAlgoAPI_Tube::GeomAlgoAPI_Tube(const double theRMin, const double theRMax, const double theZ)
{
  myRMin = theRMin;
  myRMax = theRMax;
  myZ = theZ;
}

//=================================================================================================
bool GeomAlgoAPI_Tube::check()
{
  if ((myRMax -myRMin) < Precision::Confusion()) {
    myError = "Tube builder :: rmin is greater then or equal to rmax.";
    return false;
  } else if (myRMin < 0.) {
    myError = "Tube builder :: rmin is negative.";
    return false;
  } else if (myZ < Precision::Confusion()) {
    myError = "Tube builder :: z is negative or null.";
    return false;
  }

  return true;
}

//=================================================================================================
void GeomAlgoAPI_Tube::build()
{
  buildTube();
}

//=================================================================================================
void GeomAlgoAPI_Tube::buildTube()
{
  myCreatedFaces.clear();

  // Construct the inner and outer circles
  gp_Pnt anOrigin(0., 0., 0.);
  gp_Dir aNormal(0., 0., 1.);
  gp_Circ anInnerCircle(gp_Ax2(anOrigin, aNormal), myRMin);
  gp_Circ anOuterCircle(gp_Ax2(anOrigin, aNormal), myRMax);

  // Construct the inner wire
  BRepBuilderAPI_MakeEdge anInnerCircleBuilder(anInnerCircle);
  anInnerCircleBuilder.Build();
  BRepBuilderAPI_MakeWire anInnerWireBuilder;
  anInnerWireBuilder.Add(anInnerCircleBuilder.Edge());
  anInnerWireBuilder.Build();
  TopoDS_Wire anInnerWire(anInnerWireBuilder.Wire());

  // Construct the outer wire
  BRepBuilderAPI_MakeEdge anOuterCircleBuilder(anOuterCircle);
  anOuterCircleBuilder.Build();
  BRepBuilderAPI_MakeWire anOuterWireBuilder;
  anOuterWireBuilder.Add(anOuterCircleBuilder.Edge());
  anOuterWireBuilder.Build();
  TopoDS_Wire anOuterWire(anOuterWireBuilder.Wire());

  // Construct the face withe the outer wire
  BRepBuilderAPI_MakeFace aFaceBuilder(anOuterWire);
  aFaceBuilder.Build();
  TopoDS_Face aFace(aFaceBuilder.Face());

  // Construct the hole face
  BRepAlgo_FaceRestrictor aFaceRestrictor;
  aFaceRestrictor.Init(aFace, Standard_False, Standard_True);
  aFaceRestrictor.Add(anInnerWire);
  aFaceRestrictor.Add(anOuterWire);
  aFaceRestrictor.Perform();
  aFace = TopoDS_Face(aFaceRestrictor.Current());

  // Construct the tube
  gp_Vec aVec(aNormal);
  gp_Trsf aTrsf;
  aTrsf.SetTranslation(aVec * -myZ/2);
  BRepBuilderAPI_Transform *aTranformBuilder = new BRepBuilderAPI_Transform(aFace, aTrsf);
  if (!aTranformBuilder || !aTranformBuilder->IsDone()) {
   myError = "Tube builder :: algorithm failed";
   return;
  }
  TopoDS_Shape aMovedBase = aTranformBuilder->Shape();
  BRepPrimAPI_MakePrism *aPrismBuilder = new BRepPrimAPI_MakePrism(aMovedBase, aVec * myZ);

  std::shared_ptr<GeomAPI_Shape> aShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aPrismBuilder->Shape()));
  setShape(aShape);

  setImpl(aPrismBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  setDone(true);
}
