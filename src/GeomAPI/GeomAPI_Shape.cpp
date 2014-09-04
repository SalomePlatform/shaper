// File:        GeomAPI_Shape.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Shape.h>

#include <TopoDS_Shape.hxx>

#define MY_SHAPE static_cast<TopoDS_Shape*>(myImpl)

GeomAPI_Shape::GeomAPI_Shape()
    : GeomAPI_Interface(new TopoDS_Shape())
{
}

bool GeomAPI_Shape::isNull()
{
  return MY_SHAPE->IsNull() == Standard_True;
}

bool GeomAPI_Shape::isVertex() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX;
}

bool GeomAPI_Shape::isEdge() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return aShape.ShapeType() == TopAbs_EDGE;
}
