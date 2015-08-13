// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Transform.cpp
// Created:     29 July 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_Transform.h>

#include <GeomAlgoAPI_ShapeTools.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
GeomAlgoAPI_Transform::GeomAlgoAPI_Transform(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                             std::shared_ptr<GeomAPI_Trsf>  theTrsf)
: myDone(false),
  myTrsf(theTrsf),
  myShape(new GeomAPI_Shape()),
  myMap(new GeomAPI_DataMapOfShapeShape()),
  myMkShape(new GeomAlgoAPI_MakeShape())
{
  build(theSourceShape, theTrsf);
}

//=================================================================================================
void GeomAlgoAPI_Transform::build(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                  std::shared_ptr<GeomAPI_Trsf>  theTrsf)
{
  if(!theSourceShape || !theTrsf) {
    return;
  }

  const TopoDS_Shape& aSourceShape = theSourceShape->impl<TopoDS_Shape>();
  const gp_Trsf& aTrsf = theTrsf->impl<gp_Trsf>();

  if(aSourceShape.IsNull()) {
    return;
  }

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

  myMkShape->setImpl(aBuilder);
  myShape->setImpl(new TopoDS_Shape(aResult));
}

//=================================================================================================
const bool GeomAlgoAPI_Transform::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
const bool GeomAlgoAPI_Transform::hasVolume() const
{
  bool hasVolume(false);
  if(isValid() && (GeomAlgoAPI_ShapeTools::volume(myShape) > Precision::Confusion())) {
    hasVolume = true;
  }
  return hasVolume;
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Transform::shape() const
{
  return myShape;
}

//=================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_Transform::mapOfShapes() const
{
  return myMap;
}

//=================================================================================================
std::shared_ptr<GeomAlgoAPI_MakeShape> GeomAlgoAPI_Transform::makeShape() const
{
  return myMkShape;
}

//=================================================================================================
std::shared_ptr<GeomAPI_Trsf> GeomAlgoAPI_Transform::transformation() const
{
  return myTrsf;
}
