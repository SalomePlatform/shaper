// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Curve.cpp
// Created:     04 Sep 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Curve.h>

#include <TopoDS_Shape.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>

#define MY_CURVE (*(static_cast<Handle_Geom_Curve*>(myImpl)))

GeomAPI_Curve::GeomAPI_Curve()
    : GeomAPI_Interface(new Handle_Geom_Curve())
{
}

GeomAPI_Curve::GeomAPI_Curve(const std::shared_ptr<GeomAPI_Shape>& theShape)
  : GeomAPI_Interface(new Handle_Geom_Curve()) // initially it is null
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  TopoDS_Edge anEdge = TopoDS::Edge(aShape);
  if (!anEdge.IsNull()) {
    Standard_Real aStart, anEnd;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aStart, anEnd);
    if (!aCurve.IsNull()) {
      setImpl(new Handle(Geom_Curve)(aCurve));
    }
  }
}

bool GeomAPI_Curve::isNull() const
{
  return MY_CURVE.IsNull() == Standard_True;
}

bool GeomAPI_Curve::isLine() const
{
  return !isNull() && MY_CURVE->DynamicType() == STANDARD_TYPE(Geom_Line);
}

bool GeomAPI_Curve::isCircle() const
{
  return !isNull() && MY_CURVE->DynamicType() == STANDARD_TYPE(Geom_Circle);
}
