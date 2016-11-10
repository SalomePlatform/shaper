// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_BoxPoints.cpp
// Created:     17 Mar 2016
// Author:      Clarisse Genrault (CEA)

#include <GeomAlgoAPI_BoxPoints.h>

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>

#include <iostream>

//=================================================================================================
GeomAlgoAPI_BoxPoints::GeomAlgoAPI_BoxPoints(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                                             std::shared_ptr<GeomAPI_Pnt> theSecondPoint)
:GeomAlgoAPI_Box()
{
  myFirstPoint = theFirstPoint;
  mySecondPoint = theSecondPoint;
}

//=================================================================================================
bool GeomAlgoAPI_BoxPoints::check()
{
  // The distance between myFirstPoint and mySecondPoint must not be null.
  if (myFirstPoint->distance(mySecondPoint) < Precision::Confusion())
    return false;
  return true;
}

//=================================================================================================
void GeomAlgoAPI_BoxPoints::build()
{
  myCreatedFaces.clear();

  const gp_Pnt& aFirstPoint = myFirstPoint->impl<gp_Pnt>();
  const gp_Pnt& aSecondPoint = mySecondPoint->impl<gp_Pnt>();

  // Construct the box
  BRepPrimAPI_MakeBox *aBoxMaker = new  BRepPrimAPI_MakeBox(aFirstPoint, aSecondPoint);
  aBoxMaker->Build();

  // Test the algorithm
  if(!aBoxMaker->IsDone()) {
    myError = "Box builder with two points  :: algorithm failed.";
    return;
  }

  TopoDS_Shape aResult = aBoxMaker->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);

  // Tests on the shape
  if (!aShape.get() || aShape->isNull()) {
    myError = "Box builder with two points  :: resulting shape is null.";
    return;
  }

  setImpl(aBoxMaker);

  setDone(true);
}
