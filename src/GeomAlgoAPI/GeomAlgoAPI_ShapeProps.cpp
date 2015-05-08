// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeProps.cpp
// Created:     8 May 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_ShapeProps.h>

#include <BRepGProp.hxx>
#include <GProp_GProps.hxx>
#include <TopoDS_Shape.hxx>


double GeomAlgoAPI_ShapeProps::volume(std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  TopoDS_Shape aShape = theShape->impl<TopoDS_Shape>();
  BRepGProp::VolumeProperties(aShape, aGProps);
  return aGProps.Mass();
}