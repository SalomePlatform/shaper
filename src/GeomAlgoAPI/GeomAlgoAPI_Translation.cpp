// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Translation.cpp
// Created:     8 June 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_Translation.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <gp_Ax1.hxx>

//=================================================================================================
GeomAlgoAPI_Translation::GeomAlgoAPI_Translation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                                 std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                                 double                         theDistance)
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

  // Transform the shape with copying it.
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, *aTrsf, true);
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
