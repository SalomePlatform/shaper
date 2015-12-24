// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Transform.cpp
// Created:     29 July 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_Transform.h"

#include <GeomAlgoAPI_ShapeTools.h>

#include <BRepBuilderAPI_Transform.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
GeomAlgoAPI_Transform::GeomAlgoAPI_Transform(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                             std::shared_ptr<GeomAPI_Trsf>  theTrsf)
: myTrsf(theTrsf)
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
  this->setImpl(aBuilder);
  this->setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if(aBuilder->IsDone() != Standard_True) {
    return;
  }
  TopoDS_Shape aResult = aBuilder->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}

//=================================================================================================
std::shared_ptr<GeomAPI_Trsf> GeomAlgoAPI_Transform::transformation() const
{
  return myTrsf;
}
