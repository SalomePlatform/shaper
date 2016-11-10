// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Box.cpp
// Created:     10 Mar 2016
// Author:      Clarisse Genrault (CEA)

#include <GeomAlgoAPI_Box.h>

#include <BRepPrimAPI_MakeBox.hxx>
#include <TopoDS_Shape.hxx>

//=================================================================================================
GeomAlgoAPI_Box::GeomAlgoAPI_Box()
{
}

//=================================================================================================
GeomAlgoAPI_Box::GeomAlgoAPI_Box(const double theDx, const double theDy, const double theDz)
{
  myDx = theDx;
  myDy = theDy;
  myDz = theDz;
}

//=================================================================================================
bool GeomAlgoAPI_Box::check()
{
  if (myDx < Precision::Confusion()) {
    myError = "Box builder with dimensions  :: Dx is null.";
    return false;
  } else if (myDy < Precision::Confusion()) {
    myError = "Box builder with dimensions  :: Dy is null.";
    return false;
  } else if (myDz < Precision::Confusion()) {
    myError = "Box builder with dimensions  :: Dz is null.";
    return false;
  }
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Box::build()
{
  myCreatedFaces.clear();

  // Construct the box
  BRepPrimAPI_MakeBox *aBoxMaker = new BRepPrimAPI_MakeBox(myDx, myDy, myDz);
  aBoxMaker->Build();

  // Test the algorithm
  if (!aBoxMaker->IsDone()) {
    myError = "Box builder with dimensions  :: algorithm failed.";
    return;
  }

  TopoDS_Shape aResult = aBoxMaker->Shape();
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);

  // Test on the shapes
  if (!aShape.get() || aShape->isNull()) {
    myError = "Box builder with dimensions  :: resulting shape is null.";
    return;
  }

  setImpl(aBoxMaker);

  setDone(true);
}

//=================================================================================================
void GeomAlgoAPI_Box::prepareNamingFaces()
{
  BRepPrimAPI_MakeBox aBoxMaker = impl<BRepPrimAPI_MakeBox>();
  std::shared_ptr<GeomAPI_Shape> aShapeFront(new GeomAPI_Shape);
  aShapeFront->setImpl(new TopoDS_Shape(aBoxMaker.FrontFace()));
  myCreatedFaces["Front"] = aShapeFront;
  std::shared_ptr<GeomAPI_Shape> aShapeBack(new GeomAPI_Shape);
  aShapeBack->setImpl(new TopoDS_Shape(aBoxMaker.BackFace()));
  myCreatedFaces["Back"] = aShapeBack;
  std::shared_ptr<GeomAPI_Shape> aShapeTop(new GeomAPI_Shape);
  aShapeTop->setImpl(new TopoDS_Shape(aBoxMaker.TopFace()));
  myCreatedFaces["Top"] = aShapeTop;
  std::shared_ptr<GeomAPI_Shape> aShapeBottom(new GeomAPI_Shape);
  aShapeBottom->setImpl(new TopoDS_Shape(aBoxMaker.BottomFace()));
  myCreatedFaces["Bottom"] = aShapeBottom;
  std::shared_ptr<GeomAPI_Shape> aShapeLeft(new GeomAPI_Shape);
  aShapeLeft->setImpl(new TopoDS_Shape(aBoxMaker.LeftFace()));
  myCreatedFaces["Left"] = aShapeLeft;
  std::shared_ptr<GeomAPI_Shape> aShapeRight(new GeomAPI_Shape);
  aShapeRight->setImpl(new TopoDS_Shape(aBoxMaker.RightFace()));
  myCreatedFaces["Right"] = aShapeRight;
}
