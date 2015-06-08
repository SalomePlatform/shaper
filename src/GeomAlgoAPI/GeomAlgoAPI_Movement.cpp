// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Movement.cpp
// Created:     8 June 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Movement.h>

#include <GeomAlgoAPI_ShapeProps.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <gp_Ax1.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
GeomAlgoAPI_Movement::GeomAlgoAPI_Movement(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                           double                         theDistance)
: myDone(false),
  myShape(new GeomAPI_Shape()),
  myMap(new GeomAPI_DataMapOfShapeShape()),
  myMkShape(new GeomAlgoAPI_MakeShape())
{
  build(theSourceShape, theAxis, theDistance);
}

//=================================================================================================
void GeomAlgoAPI_Movement::build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                 std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                 double                         theDistance)
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
  aTrsf.SetTranslation(gp_Vec(anAxis.Direction()) * theDistance);

  // Transform the shape with copying it.
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, aTrsf, true);
  if(!aBuilder) {
    return;
  }

  myDone = aBuilder->IsDone() == Standard_True;

  if(!myDone) {
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();
  // Fill data map to keep correct orientation of sub-shapes.
  for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next()) {
    std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
    aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
    myMap->bind(aCurrentShape, aCurrentShape);
  }

  myShape->setImpl(new TopoDS_Shape(aResult));
  myMkShape->setImpl(aBuilder);
}

//=================================================================================================
const bool GeomAlgoAPI_Movement::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
const bool GeomAlgoAPI_Movement::hasVolume() const
{
  bool hasVolume(false);
  if(isValid() && (GeomAlgoAPI_ShapeProps::volume(myShape) > Precision::Confusion())) {
    hasVolume = true;
  }
  return hasVolume;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Movement::shape() const
{
  return myShape;
}

//=================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_Movement::mapOfShapes() const
{
  return myMap;
}

//=================================================================================================
std::shared_ptr<GeomAlgoAPI_MakeShape> GeomAlgoAPI_Movement::makeShape() const
{
  return myMkShape;
}
