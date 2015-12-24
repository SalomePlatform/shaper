// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Rotation.cpp
// Created:     12 May 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_Rotation.h"

#include <BRepBuilderAPI_Transform.hxx>

//=================================================================================================
GeomAlgoAPI_Rotation::GeomAlgoAPI_Rotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                           double                         theAngle)
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
