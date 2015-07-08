// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Rotation.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Rotation.h>

#include <GeomAlgoAPI_ShapeProps.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
GeomAlgoAPI_Rotation::GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                           double                         theAngle,
                                           bool theSimpleTransform)
: myDone(false),
  myShape(new GeomAPI_Shape()),
  myMap(new GeomAPI_DataMapOfShapeShape()),
  myMkShape(new GeomAlgoAPI_MakeShape())
{
  build(theSourceShape, theAxis, theAngle, theSimpleTransform);
}

//=================================================================================================
void GeomAlgoAPI_Rotation::build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                 std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                 double                         theAngle,
                                 bool theSimpleTransform)
{
  if(!theSourceShape || !theAxis) {
    return;
  }

  const TopoDS_Shape& aSourceShape = theSourceShape->impl<TopoDS_Shape>();
  const gp_Ax1& anAxis = theAxis->impl<gp_Ax1>();

  if(aSourceShape.IsNull()) {
    return;
  }

  gp_Trsf aTrsf;
  aTrsf.SetRotation(anAxis, theAngle / 180.0 * M_PI);

  TopoDS_Shape aResult;
  // Transform the shape with copying it.
  if (theSimpleTransform) {
    TopLoc_Location aDelta(aTrsf);
    aResult = aSourceShape.Moved(aDelta);
    myDone = true; // is OK for sure
  } else {
    BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, aTrsf, true);
    if(!aBuilder) {
      return;
    }

    myDone = aBuilder->IsDone() == Standard_True;

    if(!myDone) {
      return;
    }

    aResult = aBuilder->Shape();
    // Fill data map to keep correct orientation of sub-shapes.
    for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next()) {
      std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
      aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
      myMap->bind(aCurrentShape, aCurrentShape);
    }
    myMkShape->setImpl(aBuilder);
  }

  myShape->setImpl(new TopoDS_Shape(aResult));
}

//=================================================================================================
const bool GeomAlgoAPI_Rotation::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
const bool GeomAlgoAPI_Rotation::hasVolume() const
{
  bool hasVolume(false);
  if(isValid() && (GeomAlgoAPI_ShapeProps::volume(myShape) > Precision::Confusion())) {
    hasVolume = true;
  }
  return hasVolume;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Rotation::shape() const
{
  return myShape;
}

//=================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_Rotation::mapOfShapes() const
{
  return myMap;
}

//=================================================================================================
std::shared_ptr<GeomAlgoAPI_MakeShape> GeomAlgoAPI_Rotation::makeShape() const
{
  return myMkShape;
}
