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
  headError = "Box builder with dimensions";
}

//=================================================================================================
GeomAlgoAPI_Box::GeomAlgoAPI_Box(std::shared_ptr<GeomAPI_Pnt> theFirstPoint,
                                 std::shared_ptr<GeomAPI_Pnt> theSecondPoint)
{
  myFirstPoint = theFirstPoint;
  mySecondPoint = theSecondPoint;
  myMethodType = MethodType::BOX_POINTS;
  headError = "Box builder with two points";
}

//=================================================================================================
GeomAlgoAPI_Box::GeomAlgoAPI_Box(const double theOx, const double theOy, const double theOz,
                                 const double theDx, const double theDy, const double theDz)
{
  myOx = theOx;
  myOy = theOy;
  myOz = theOz;
  myDx = theDx;
  myDy = theDy;
  myDz = theDz;
  myMethodType = MethodType::BOX_POINT_DIMS;
  headError = "Box builder with coordinates and dimensions";
}

//=================================================================================================
bool GeomAlgoAPI_Box::check()
{
  if (myMethodType == MethodType::BOX_DIM || myMethodType == MethodType::BOX_POINT_DIMS) {
    if (myDx < Precision::Confusion()) {
      myError = headError + " :: Dx is null or negative.";
      return false;
    } else if (myDy < Precision::Confusion()) {
      myError = headError + " :: Dy is null or negative.";
      return false;
    } else if (myDz < Precision::Confusion()) {
      myError = headError + " :: Dz is null or negative.";
      return false;
    }
  } else if (myMethodType == MethodType::BOX_POINTS) {
    if (!myFirstPoint.get()) {
      myError = headError + " :: the first point is not valid.";
      return false;
    }
    if (!mySecondPoint.get()) {
      myError = headError + " :: the second point is not valid.";
      return false;
    }
    if (myFirstPoint->distance(mySecondPoint) < Precision::Confusion()) {
      myError = headError + " :: the distance between the two points is null.";
      return false;
    }
    double aDiffX = myFirstPoint->x() - mySecondPoint->x();
    double aDiffY = myFirstPoint->y() - mySecondPoint->y();
    double aDiffZ = myFirstPoint->z() - mySecondPoint->z();
    if (fabs(aDiffX)  < Precision::Confusion() ||
        fabs(aDiffY)  < Precision::Confusion() ||
        fabs(aDiffZ)  < Precision::Confusion()) {
      myError =
        headError + " :: the points belong both to one of the OXY, OYZ or OZX planes.";
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
  } else if (myMethodType == MethodType::BOX_POINT_DIMS) {
    buildWithPointAndDims();
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
    myError = headError + " :: algorithm failed.";
    return;
  }

  TopoDS_Shape aResult = aBoxMaker->Shape();
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);

  // Test on the shapes
  if (!aShape.get() || aShape->isNull()) {
    myError = headError + " :: resulting shape is null.";
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
    myError = headError + " :: algorithm failed.";
    return;
  }

  TopoDS_Shape aResult = aBoxMaker->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);

  // Tests on the shape
  if (!aShape.get() || aShape->isNull()) {
    myError = headError + " :: resulting shape is null.";
    return;
  }

  setImpl(aBoxMaker);

  setDone(true);
}

//=================================================================================================
void GeomAlgoAPI_Box::buildWithPointAndDims()
{
  // Construct points from cordinates and dimensions to use the method with two points
  myFirstPoint =
    std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(myOx - myDx, myOy - myDy, myOz - myDz));
  mySecondPoint =
    std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(myOx + myDx, myOy + myDy, myOz + myDz));

  buildWithPoints();
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
