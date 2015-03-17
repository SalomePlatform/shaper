// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Curve.cpp
// Created:     04 Sep 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Curve.h>
#include<GeomAPI_Pnt.h>

#include <TopoDS_Shape.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <GeomAdaptor_Curve.hxx>

#define MY_CURVE (*(static_cast<Handle_Geom_Curve*>(myImpl)))

GeomAPI_Curve::GeomAPI_Curve()
    : GeomAPI_Interface(new Handle_Geom_Curve()), myStart(0), myEnd(1)
{
}

GeomAPI_Curve::GeomAPI_Curve(const std::shared_ptr<GeomAPI_Shape>& theShape)
  : GeomAPI_Interface(new Handle_Geom_Curve()) // initially it is null
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  TopoDS_Edge anEdge = TopoDS::Edge(aShape);
  if (!anEdge.IsNull()) {
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, myStart, myEnd);
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

std::shared_ptr<GeomAPI_Pnt> GeomAPI_Curve::getPoint(double theParam)
{
  GeomAdaptor_Curve aAdaptor(MY_CURVE, myStart, myEnd);
  gp_Pnt aPnt = aAdaptor.Value(theParam);
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
}
