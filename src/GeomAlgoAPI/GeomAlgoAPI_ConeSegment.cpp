// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ConeSegment.cpp
// Created:     24 Nov 2016
// Author:      Clarisse Genrault (CEA)

#include <GeomAlgoAPI_ConeSegment.h>

#include <gp_Dir.hxx>
#include <gp_Ax1.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS_Edge.hxx>

#include <BRepPrimAPI_MakeRevol.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <Precision.hxx>

#include <math.h>

//=================================================================================================
GeomAlgoAPI_ConeSegment::GeomAlgoAPI_ConeSegment()
{
}

//=================================================================================================

GeomAlgoAPI_ConeSegment::GeomAlgoAPI_ConeSegment(const double theRMin1,
                                                 const double theRMax1,
                                                 const double theRMin2,
                                                 const double theRMax2,
                                                 const double theZ,
                                                 const double theStartPhi,
                                                 const double theDeltaPhi)
{
  myRMin1 = theRMin1;
  myRMax1 = theRMax1;
  myRMin2 = theRMin2;
  myRMax2 = theRMax2;
  myZ = theZ;
  myStartPhi = theStartPhi;
  myDeltaPhi = theDeltaPhi;
}

//=================================================================================================
bool GeomAlgoAPI_ConeSegment::check()
{
  if (myRMin1 < 0.)
  {
    myError = "Cone Segment builder :: rmin1 is negative.";
    return false;
  }
  if (myRMin2 < 0.)
  {
    myError = "Cone Segment builder :: rmin2 is negative.";
    return false;
  }
  if ((myRMax1-myRMin1) < Precision::Confusion())
  {
    myError = "Cone Segment builder :: rmin1 is larger than rmax1.";
    return false;
  }
  if ((myRMax2-myRMin2) < Precision::Confusion())
  {
    myError = "Cone Segment builder :: rmin2 is larger than rmax2.";
    return false;
  }
  if (myZ < Precision::Confusion())
  {
    myError = "Cone Segment builder :: z is negative or null.";
    return false;
  }
  if (myDeltaPhi < Precision::Angular() * 180./M_PI)
  {
    myError = "Cone Segment builder :: deltaphi is negative or null.";
    return false;
  }
  if (myDeltaPhi > 360)
  {
    myError = "Cone Segment builder :: deltaphi is larger than 360 degree.";
    return false;
  }
  return true;
}

//=================================================================================================
void GeomAlgoAPI_ConeSegment::build()
{
  myCreatedFaces.clear();

  const double aStartPhiRad = myStartPhi * M_PI/180.;
  BRepBuilderAPI_MakeWire aWireBuilder;

  // Define the section from the 4 vertices
  gp_Pnt aPointOuterBase(myRMax1 * cos(aStartPhiRad), myRMax1 * sin(aStartPhiRad), -myZ/2.);
  gp_Pnt aPointInnerBase(myRMin1 * cos(aStartPhiRad), myRMin1 * sin(aStartPhiRad), -myZ/2.);
  gp_Pnt aPointOuterTop(myRMax2 * cos(aStartPhiRad), myRMax2 * sin(aStartPhiRad), myZ/2.);
  gp_Pnt aPointInnerTop(myRMin2 * cos(aStartPhiRad), myRMin2 * sin(aStartPhiRad), myZ/2.);

  if ((myRMax1 - myRMin1) >= Precision::Confusion()){
    BRepBuilderAPI_MakeEdge anEdgeBuilderBase(aPointOuterBase, aPointInnerBase);
    anEdgeBuilderBase.Build();
    aWireBuilder.Add(anEdgeBuilderBase.Edge());
  }

  BRepBuilderAPI_MakeEdge anEdgeBuilderOuter(aPointOuterBase, aPointOuterTop);
  anEdgeBuilderOuter.Build();
  aWireBuilder.Add(anEdgeBuilderOuter.Edge());
  if ((myRMax2 - myRMin2) >= Precision::Confusion()){
    BRepBuilderAPI_MakeEdge anEdgeBuilderTop(aPointOuterTop, aPointInnerTop);
    anEdgeBuilderTop.Build();
    aWireBuilder.Add(anEdgeBuilderTop.Edge());
  }

  BRepBuilderAPI_MakeEdge anEdgeBuilderInner(aPointInnerTop, aPointInnerBase);
  anEdgeBuilderInner.Build();
  aWireBuilder.Add(anEdgeBuilderInner.Edge());

  aWireBuilder.Build();
  BRepBuilderAPI_MakeFace aFaceBuilder(aWireBuilder.Wire());
  aFaceBuilder.Build();

  if (!aFaceBuilder.IsDone()){
    myError = "Cone Segment builder :: section is not valid";
    return;
  }

  // Perform a revolution based on the section to build the solid
  gp_Dir aZDir(0., 0., 1.);
  gp_Pnt anOrigin(0., 0., 0.);
  gp_Ax1 aZAxis(anOrigin, aZDir);
  BRepPrimAPI_MakeRevol* aRevolBuilder =
    new BRepPrimAPI_MakeRevol(aFaceBuilder.Face(), aZAxis, myDeltaPhi * M_PI/180., Standard_True);
  if(!aRevolBuilder) {
    return;
    myError = "Cone Segment builder :: section revolution did not succeed";
  }
  if(!aRevolBuilder->IsDone()) {
    myError = "Cone Segment builder :: section revolution did not succeed";
    return;
  }

  setImpl(aRevolBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  std::shared_ptr<GeomAPI_Shape> aResultShape =
    std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
  aResultShape->setImpl(new TopoDS_Shape(aRevolBuilder->Shape()));

  // Test on the shapes
  if (!(aResultShape).get() || aResultShape->isNull()) {
    myError = "Cone Segment builder  :: resulting shape is null.";
    return;
  }

  setShape(aResultShape);
  setDone(true);
}
