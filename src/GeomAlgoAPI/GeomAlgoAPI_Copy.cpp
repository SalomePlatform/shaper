// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Copy.cpp
// Created:     06 Sept 2016
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_Copy.h"


#include <BRepBuilderAPI_Copy.hxx>

//=================================================================================================
GeomAlgoAPI_Copy::GeomAlgoAPI_Copy(const std::shared_ptr<GeomAPI_Shape> theShape,
                                   const bool theCopyGeom,
                                   const bool theCopyMesh)
{
  build(theShape, theCopyGeom, theCopyMesh);
}


//=================================================================================================
void GeomAlgoAPI_Copy::build(const std::shared_ptr<GeomAPI_Shape> theShape,
                             const bool theCopyGeom,
                             const bool theCopyMesh)
{
  if(!theShape.get()) {
    return;
  }

  // Getting shape.
  const TopoDS_Shape& aBaseShape = theShape->impl<TopoDS_Shape>();

  // Creating copy.
  BRepBuilderAPI_Copy* aBuilder = new BRepBuilderAPI_Copy(aBaseShape, theCopyGeom, theCopyMesh);
  this->setImpl(aBuilder);
  this->setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  TopoDS_Shape aResult = aBuilder->Shape();

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
