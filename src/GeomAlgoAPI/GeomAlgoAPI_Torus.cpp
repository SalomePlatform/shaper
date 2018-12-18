// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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