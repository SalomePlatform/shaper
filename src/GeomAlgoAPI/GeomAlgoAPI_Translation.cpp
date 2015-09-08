// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Translation.cpp
// Created:     8 June 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Translation.h>

#include <GeomAlgoAPI_ShapeTools.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <gp_Ax1.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
GeomAlgoAPI_Translation::GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                                 std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                                 double                         theDistance)
: myDone(false)
{
  build(theSourceShape, theAxis, theDistance);
}

//=================================================================================================
void GeomAlgoAPI_Translation::build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
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

  gp_Trsf* aTrsf = new gp_Trsf();
  aTrsf->SetTranslation(gp_Vec(anAxis.Direction()) * theDistance);
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
const bool GeomAlgoAPI_Translation::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
const bool GeomAlgoAPI_Translation::hasVolume() const
{
  bool hasVolume(false);
  if(isValid() && (GeomAlgoAPI_ShapeTools::volume(myShape) > Precision::Confusion())) {
    hasVolume = true;
  }
  return hasVolume;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Translation::shape() const
{
  return myShape;
}

//=================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_Translation::mapOfShapes() const
{
  return myMap;
}

//=================================================================================================
std::shared_ptr<GeomAlgoAPI_MakeShape> GeomAlgoAPI_Translation::makeShape() const
{
  return myMkShape;
}

//=================================================================================================
std::shared_ptr<GeomAPI_Trsf> GeomAlgoAPI_Translation::transformation() const
{
  return myTrsf;
}
