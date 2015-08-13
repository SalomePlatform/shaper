// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Shape.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include<GeomAPI_Shape.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Iterator.hxx>
#include <BRepBndLib.hxx>
#include <Bnd_Box.hxx>
#include <BRepTools.hxx>

#include <sstream>

#define MY_SHAPE implPtr<TopoDS_Shape>()

GeomAPI_Shape::GeomAPI_Shape()
    : GeomAPI_Interface(new TopoDS_Shape())
{
}

bool GeomAPI_Shape::isNull() const
{
  return MY_SHAPE->IsNull() == Standard_True;
}

bool GeomAPI_Shape::isEqual(const std::shared_ptr<GeomAPI_Shape> theShape) const
{
  if (!theShape.get())
    return false;
  if (isNull())
    return theShape->isNull();
  if (theShape->isNull())
    return false;

  return MY_SHAPE->IsEqual(theShape->impl<TopoDS_Shape>()) == Standard_True;
}

bool GeomAPI_Shape::isVertex() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX;
}

bool GeomAPI_Shape::isEdge() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_EDGE;
}

bool GeomAPI_Shape::isFace() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_FACE;
}

bool GeomAPI_Shape::isCompound() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_COMPOUND;
}

bool GeomAPI_Shape::isCompoundOfSolids() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull() || aShape.ShapeType() != TopAbs_COMPOUND)
    return false;
  bool isAtLeastOne = false;
  for(TopoDS_Iterator aSubs(aShape); aSubs.More(); aSubs.Next()) {
    if (aSubs.Value().IsNull() || aSubs.Value().ShapeType() != TopAbs_SOLID)
      return false;
    isAtLeastOne = true;
  }
  return isAtLeastOne;
}

bool GeomAPI_Shape::isSolid() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_SOLID;
}

GeomAPI_Shape::ShapeType GeomAPI_Shape::shapeType() const
{
  const TopoDS_Shape& aShape = impl<TopoDS_Shape>();
  return (ShapeType)aShape.ShapeType();
}

bool GeomAPI_Shape::computeSize(double& theXmin, double& theYmin, double& theZmin,
                                double& theXmax, double& theYmax, double& theZmax) const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return false;
  Bnd_Box aBndBox;
  BRepBndLib::Add(aShape, aBndBox);
  aBndBox.Get(theXmin, theYmin, theZmin, theXmax, theYmax, theZmax);
  return true;
}

std::string GeomAPI_Shape::getShapeStream() const
{
  std::ostringstream aStream;
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  BRepTools::Write(aShape, aStream);
  return aStream.str();
}
