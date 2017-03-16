// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Sphere.h
// Created:     16 Mar 2017
// Author:      Clarisse Genrault (CEA)

#include <GeomAlgoAPI_Sphere.h>

#include <BRepPrimAPI_MakeSphere.hxx>

//=================================================================================================
GeomAlgoAPI_Sphere::GeomAlgoAPI_Sphere()
{
}

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
