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
                                           double                         theAngle)
: myDone(false),
  myShape(new GeomAPI_Shape())
{
  build(theSourceShape, theAxis, theAngle);
}

//=================================================================================================
void GeomAlgoAPI_Rotation::build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                 std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                 double                         theAngle)
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

  // Transform the shape with copying it.
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, aTrsf, true);
  if(!aBuilder) {
    return;
  }

  setImpl(aBuilder);
  myDone = aBuilder->IsDone() == Standard_True;

  if(!myDone) {
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();
  // Fill data map to keep correct orientation of sub-shapes.
  for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next()) {
    std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
    aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
    myMap.bind(aCurrentShape, aCurrentShape);
  }

  myShape->setImpl(new TopoDS_Shape(aResult));
  myMkShape = new GeomAlgoAPI_MakeShape(aBuilder);
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
void GeomAlgoAPI_Rotation::mapOfShapes(GeomAPI_DataMapOfShapeShape& theMap) const
{
  theMap = myMap;
}

//=================================================================================================
GeomAlgoAPI_MakeShape* GeomAlgoAPI_Rotation::makeShape() const
{
  return myMkShape;
}

//=================================================================================================
GeomAlgoAPI_Rotation::~GeomAlgoAPI_Rotation()
{
  if (myImpl) {
    myMap.clear();
  }
}
