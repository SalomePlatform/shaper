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

#include <gp_Circ.hxx>

#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>

#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepPrimAPI_MakeSphere.hxx>

//=================================================================================================
GeomAlgoAPI_Sphere::GeomAlgoAPI_Sphere(std::shared_ptr<GeomAPI_Pnt> theCenterPoint,
                                       const double theRadius)
{
  isRootGeo = false;
  myCenterPoint = theCenterPoint;
  myRadius = theRadius;
}

//=================================================================================================
GeomAlgoAPI_Sphere::GeomAlgoAPI_Sphere(const double theRMin, const double theRMax,
                                       const double thePhiMin, const double thePhiMax,
                                       const double theThetaMin, const double theThetaMax)
{
  isRootGeo = true;
  myRMin = theRMin;
  myRMax = theRMax;
  myPhiMin = thePhiMin;
  myPhiMax = thePhiMax;
  myThetaMin = theThetaMin;
  myThetaMax = theThetaMax;
}

//=================================================================================================
bool GeomAlgoAPI_Sphere::check()
{
  if (isRootGeo) {
    if ((myRMin-myRMax) > Precision::Confusion()) {
      myError = "Sphere builder :: RMin is larger than RMax.";
      return false;
    }
  } else {
    if (!myCenterPoint) {
      myError = "Sphere builder :: center is not valid.";
      return false;
    }
    if (myRadius < Precision::Confusion()) {
      myError = "Sphere builder :: radius is negative or null.";
      return false;
    }
  }
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Sphere::build()
{
  myCreatedFaces.clear();
  if (isRootGeo) {
    buildRootSphere();
  } else {
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
}

//=================================================================================================
void GeomAlgoAPI_Sphere::buildRootSphere()
{
  myCreatedFaces.clear();

  const double aStartPhiRad = myPhiMin * M_PI / 180.;
  BRepBuilderAPI_MakeWire aWireBuilder;

  gp_Pnt anOrigin(0., 0., 0.);
  gp_Dir aNormal(-sin(aStartPhiRad), cos(aStartPhiRad), 0.);


  // The section is defined by 4 points and up to 4 wires. In the rmin = 0 case, two of the points
  // will actually be the origin
  double aX = myRMax*sin(myThetaMin * M_PI/180.);
  double aZ = myRMax*cos(myThetaMin * M_PI/180.);
  gp_Pnt aTopOuterStart(aX*cos(aStartPhiRad), aX*sin(aStartPhiRad), aZ);

  aX = myRMax*sin((myThetaMin+myThetaMax) * M_PI/180.);
  aZ = myRMax*cos((myThetaMin+myThetaMax) * M_PI/180.);
  gp_Pnt aBaseOuterEnd(aX*cos(aStartPhiRad), aX*sin(aStartPhiRad), aZ);

  aX = myRMin*sin(myThetaMin * M_PI/180.);
  aZ = myRMin*cos(myThetaMin * M_PI/180.);
  gp_Pnt aTopInnerStart(aX*cos(aStartPhiRad), aX*sin(aStartPhiRad), aZ);

  aX = myRMin*sin((myThetaMin+myThetaMax) * M_PI/180.);
  aZ = myRMin*cos((myThetaMin+myThetaMax) * M_PI/180.);
  gp_Pnt aBaseInnerEnd(aX*cos(aStartPhiRad), aX*sin(aStartPhiRad), aZ);

  // There will always be the rmax arc since rmax can't be zero
  gp_Circ anOuterCircle(gp_Ax2(anOrigin, aNormal), myRMax);
  BRepBuilderAPI_MakeEdge anArcOuterBuilder;
  if (aTopOuterStart.IsEqual(aBaseOuterEnd, Precision::Confusion()))
    anArcOuterBuilder = BRepBuilderAPI_MakeEdge(anOuterCircle);
  else
    anArcOuterBuilder = BRepBuilderAPI_MakeEdge(anOuterCircle, aTopOuterStart, aBaseOuterEnd);
  anArcOuterBuilder.Build();

  // Two cases : either we need four edges (one being an arc with curvature radius rmin) or we
  // need three (if rmin=0).
  // In the later case the top and bottom edges intersect at the origin
  // Add the edges to the wire in consecutive order (very important for the face to make sense
  // topologically)
  if(myRMin >= Precision::Confusion()){
    gp_Circ anInnerCircle(gp_Ax2(anOrigin, aNormal), myRMin);
    BRepBuilderAPI_MakeEdge anArcInnerBuilder;
    if (aTopInnerStart.IsEqual(aBaseInnerEnd, Precision::Confusion()))
      anArcInnerBuilder = BRepBuilderAPI_MakeEdge(anInnerCircle);
    else
      anArcInnerBuilder = BRepBuilderAPI_MakeEdge(anInnerCircle, aTopInnerStart, aBaseInnerEnd);
    anArcInnerBuilder.Build();

    BRepBuilderAPI_MakeEdge anEdgeStartBuilder(aTopInnerStart, aTopOuterStart);
    anEdgeStartBuilder.Build();
    BRepBuilderAPI_MakeEdge anEdgeEndBuilder(aBaseInnerEnd, aBaseOuterEnd);
    anEdgeEndBuilder.Build();

    aWireBuilder.Add(anEdgeStartBuilder.Edge());
    aWireBuilder.Add(anArcOuterBuilder.Edge());
    aWireBuilder.Add(anEdgeEndBuilder.Edge());
    aWireBuilder.Add(anArcInnerBuilder.Edge());
  }
  else{
    BRepBuilderAPI_MakeEdge anEdgeStartBuilder(anOrigin, aTopOuterStart);
    anEdgeStartBuilder.Build();
    BRepBuilderAPI_MakeEdge anEdgeEndBuilder(anOrigin, aBaseOuterEnd);
    anEdgeEndBuilder.Build();

    aWireBuilder.Add(anArcOuterBuilder.Edge());
    aWireBuilder.Add(anEdgeStartBuilder.Edge());
    aWireBuilder.Add(anEdgeEndBuilder.Edge());
  }

  // Make a face from the wire
  aWireBuilder.Build();
  BRepBuilderAPI_MakeFace aFaceBuilder(aWireBuilder.Wire());
  aFaceBuilder.Build();

  if (!aFaceBuilder.IsDone()){
    myError = "GDML sphere builder :: section is not valid";
    return;
  }

  // Mathematical tool objects needed by the revolution builder
  gp_Dir aZDir(0., 0., 1.);
  gp_Ax1 aZAxis(anOrigin, aZDir);

  // Build the solid using the section face we've created and a revolution builder
  BRepPrimAPI_MakeRevol* aRevolBuilder =
    new BRepPrimAPI_MakeRevol(aFaceBuilder.Face(), aZAxis, myPhiMax * M_PI / 180., Standard_True);
  if(!aRevolBuilder) {
    return;
    myError = "GDML Sphere builder :: section revolution did not succeed";
  }
  if(!aRevolBuilder->IsDone()) {
    myError = "GDML Sphere builder :: section revolution did not succeed";
    return;
  }

  // Get the shape, verify it, build a GeomAPI_Shape.
  std::shared_ptr<GeomAPI_Shape> aResultShape =
    std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
  aResultShape->setImpl(new TopoDS_Shape(aRevolBuilder->Shape()));
  setShape(aResultShape);

  // Test on the shapes
  if (!(aResultShape).get() || aResultShape->isNull()) {
    myError = "GDML Sphere builder  :: resulting shape is null.";
    return;
  }

  setImpl(aRevolBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  setDone(true);
}
