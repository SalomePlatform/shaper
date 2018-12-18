// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Cone.cpp
// Created:     20 Mar 2017
// Author:      Clarisse Genrault (CEA)

#include <GeomAlgoAPI_Cone.h>

#include <gp_Ax2.hxx>

#include <BRepPrimAPI_MakeCone.hxx>

//=================================================================================================
GeomAlgoAPI_Cone::GeomAlgoAPI_Cone(std::shared_ptr<GeomAPI_Ax2> theAxis,
                                   const double theBaseRadius,
                                   const double theTopRadius,
                                   const double theHeight)
{
  myAxis = theAxis;
  myBaseRadius = theBaseRadius;
  myTopRadius = theTopRadius;
  myHeight = theHeight;
}

//=================================================================================================
bool GeomAlgoAPI_Cone::check()
{
  if (!myAxis) {
    myError = "Cone builder :: axis is not valid.";
    return false;
  } else if (myBaseRadius < Precision::Confusion() && myTopRadius < Precision::Confusion()) {
    myError = "Cone builder :: base radius and top radius are negative or null.";
    return false;
  } else if (myBaseRadius < 0.) {
    myError = "Cone builder :: base radius is negative.";
    return false;
  } else if (myTopRadius < 0.) {
    myError = "Cone builder :: top radius is negative.";
    return false;
  } else if (fabs(myBaseRadius-myTopRadius) < Precision::Confusion()) {
    myError = "Cone builder :: base radius and top radius are too close.";
    return false;
  } else if (myHeight < Precision::Confusion()) {
    myError = "Cone builder :: height is negative or null.";
    return false;
  }
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Cone::build()
{
  myCreatedFaces.clear();

  const gp_Ax2& anAxis = myAxis->impl<gp_Ax2>();

  // Construct the torus
  BRepPrimAPI_MakeCone *aConeMaker =
    new BRepPrimAPI_MakeCone(anAxis, myBaseRadius, myTopRadius, myHeight);

  aConeMaker->Build();

  if (!aConeMaker->IsDone()) {
    return;
  }

  TopoDS_Shape aResult = aConeMaker->Shape();
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);

  // Test on the shapes
  if (!aShape.get() || aShape->isNull()) {
    myError = "Torus builder :: resulting shape is null.";
    return;
  }

  setImpl(aConeMaker);

  setDone(true);
}