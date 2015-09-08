// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Rotation.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Rotation.h>

#include <GeomAlgoAPI_ShapeTools.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
GeomAlgoAPI_Rotation::GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                           double                         theAngle)
: myDone(false)
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

  gp_Trsf* aTrsf = new gp_Trsf();
  aTrsf->SetRotation(anAxis, theAngle / 180.0 * M_PI);
  myTrsf.reset(new GeomAPI_Trsf(aTrsf));

  // Transform the shape with copying it.
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, *aTrsf, true);
  if(!aBuilder) {
    return;
  }
  myMkShape.reset(new GeomAlgoAPI_MakeShape(aBuilder));

  myDone = aBuilder->IsDone() == Standard_True;

  if(!myDone) {
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();
  // Fill data map to keep correct orientation of sub-shapes.
  myMap.reset(new GeomAPI_DataMapOfShapeShape());
  for(TopExp_Explorer anExp(aResult, TopAbs_FACE); anExp.More(); anExp.Next()) {
    std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
    aCurrentShape->setImpl(new TopoDS_Shape(anExp.Current()));
    myMap->bind(aCurrentShape, aCurrentShape);
  }

  myShape.reset(new GeomAPI_Shape());
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
  if(isValid() && (GeomAlgoAPI_ShapeTools::volume(myShape) > Precision::Confusion())) {
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

//=================================================================================================
std::shared_ptr<GeomAPI_Trsf> GeomAlgoAPI_Rotation::transformation() const
{
  return myTrsf;
}
