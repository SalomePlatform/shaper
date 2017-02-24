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
  myMethodType = MethodType::BOX_DIM;
}

//=================================================================================================
GeomAlgoAPI_Box::GeomAlgoAPI_Box(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                                 std::shared_ptr<GeomAPI_Pnt> theSecondPoint)
{
  myFirstPoint = theFirstPoint;
  mySecondPoint = theSecondPoint;
  myMethodType = MethodType::BOX_POINTS;
}

//=================================================================================================
bool GeomAlgoAPI_Box::check()
{
  if (myMethodType == MethodType::BOX_DIM) {
    if (myDx < Precision::Confusion()) {
      myError = "Box builder with dimensions :: Dx is null or negative.";
      return false;
    } else if (myDy < Precision::Confusion()) {
      myError = "Box builder with dimensions :: Dy is null or negative.";
      return false;
    } else if (myDz < Precision::Confusion()) {
      myError = "Box builder with dimensions :: Dz is null or negative.";
      return false;
    }
  } else if (myMethodType == MethodType::BOX_POINTS) {
    if (!myFirstPoint.get()) {
      myError = "Box builder with points :: the first point is not valid.";
      return false;
    }
    if (!mySecondPoint.get()) {
      myError = "Box builder with points :: the second point is not valid.";
      return false;
    }
    if (myFirstPoint->distance(mySecondPoint) < Precision::Confusion()) {
      myError = "Box builder with points :: the distance between the two points is null.";
      return false;
    }
    double aDiffX = myFirstPoint->x() - mySecondPoint->x();
    double aDiffY = myFirstPoint->y() - mySecondPoint->y();
    double aDiffZ = myFirstPoint->z() - mySecondPoint->z();
    if (fabs(aDiffX)  < Precision::Confusion() ||
        fabs(aDiffY)  < Precision::Confusion() ||
        fabs(aDiffZ)  < Precision::Confusion()) {
      myError =
        "Box builder with points :: the points belong both to one of the OXY, OYZ or OZX planes.";
      return false;
    }
  } else {
    myError = "Box builder :: Method not implemented.";
    return false;
  }
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Box::build()
{
  if (myMethodType == MethodType::BOX_DIM) {
    buildWithDimensions();
  } else if (myMethodType == MethodType::BOX_POINTS) {
    buildWithPoints();
  } else {
    myError = "Box builder :: Method not implemented.";
    return;
  }
}

//=================================================================================================
void GeomAlgoAPI_Box::buildWithDimensions()
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
void GeomAlgoAPI_Box::buildWithPoints()
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
