// Copyright (C) 2014-201x CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Scale.cpp
// Created:     23 Jan 2017
// Author:      Clarisse Genrault (CEA)

#include "GeomAlgoAPI_Scale.h"

#include <BRepBuilderAPI_Transform.hxx>
#include <Precision.hxx>

//=================================================================================================
GeomAlgoAPI_Scale::GeomAlgoAPI_Scale(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                     std::shared_ptr<GeomAPI_Pnt>   theCenterPoint,
                                     double theScaleFactor)
{
  mySourceShape = theSourceShape;
  myCenterPoint = theCenterPoint;
  myScaleFactor = theScaleFactor;
}

//=================================================================================================
bool GeomAlgoAPI_Scale::check()
{
  if (!mySourceShape) {
    myError = "Scale builder :: source shape is invalid.";
    return false;
  }
  if (!myCenterPoint) {
    myError = "Scale builder :: center point is invalid.";
    return false;
  }
  if (fabs(myScaleFactor) < Precision::Confusion()) {
    myError = "Scale builder :: the scale factor is null.";
    return false;
  }
  return true;
}

//=================================================================================================
void GeomAlgoAPI_Scale::build()
{
  const gp_Pnt& aCenterPoint = myCenterPoint->impl<gp_Pnt>();
  gp_Trsf* aTrsf = new gp_Trsf();
  aTrsf->SetScale(aCenterPoint, myScaleFactor);

  const TopoDS_Shape& aSourceShape = mySourceShape->impl<TopoDS_Shape>();

  if(aSourceShape.IsNull()) {
    myError = "Scale builder :: source shape does not contain any actual shape.";
    return;
  }

  // Transform the shape while copying it.
  BRepBuilderAPI_Transform* aBuilder = new BRepBuilderAPI_Transform(aSourceShape, *aTrsf, true);
  if(!aBuilder) {
    myError = "Scale builder :: transform initialization failed.";
    return;
  }

  setImpl(aBuilder);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if(!aBuilder->IsDone()) {
    myError = "Scale builder :: algorithm failed.";
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  setShape(aShape);
  setDone(true);
}
