// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Shape.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include "GeomAPI_Shape.h"

#include <BRep_Tool.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepTools.hxx>
#include <Bnd_Box.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

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

bool GeomAPI_Shape::isCompSolid() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();
  return !aShape.IsNull() && aShape.ShapeType() == TopAbs_COMPSOLID;
}

bool GeomAPI_Shape::isPlanar() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Shape*>(this)->impl<TopoDS_Shape>();

  if(aShape.IsNull()) {
    return false;
  }

  TopAbs_ShapeEnum aShapeType = aShape.ShapeType();

  if(aShapeType == TopAbs_VERTEX) {
    return true;
  } else if(aShapeType == TopAbs_EDGE || aShapeType ==  TopAbs_WIRE || aShapeType == TopAbs_SHELL) {
    BRepBuilderAPI_FindPlane aFindPlane(aShape);
    return aFindPlane.Found() == Standard_True;
  } else if(aShapeType == TopAbs_FACE) {
    const Handle(Geom_Surface)& aSurface = BRep_Tool::Surface(TopoDS::Face(aShape));
    Handle(Standard_Type) aType = aSurface->DynamicType();

    if(aType == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
      Handle(Geom_RectangularTrimmedSurface) aTrimSurface = Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
      aType = aTrimSurface->BasisSurface()->DynamicType();
    }
    return (aType == STANDARD_TYPE(Geom_Plane));
  } else {
    return false;
  }
}

GeomAPI_Shape::ShapeType GeomAPI_Shape::shapeType() const
{
  const TopoDS_Shape& aShape = impl<TopoDS_Shape>();

  ShapeType aST = GeomAPI_Shape::SHAPE;

  switch(aShape.ShapeType()) {
  case TopAbs_COMPOUND:
    aST = GeomAPI_Shape::COMPOUND;
    break;
  case TopAbs_COMPSOLID:
    aST = GeomAPI_Shape::COMPSOLID;
    break;
  case TopAbs_SOLID:
    aST = GeomAPI_Shape::SOLID;
    break;
  case TopAbs_SHELL:
    aST = GeomAPI_Shape::SHELL;
    break;
  case TopAbs_FACE:
    aST = GeomAPI_Shape::FACE;
    break;
  case TopAbs_WIRE:
    aST = GeomAPI_Shape::WIRE;
    break;
  case TopAbs_EDGE:
    aST = GeomAPI_Shape::EDGE;
    break;
  case TopAbs_VERTEX:
    aST = GeomAPI_Shape::VERTEX;
    break;
  case TopAbs_SHAPE:
    aST = GeomAPI_Shape::SHAPE;
    break;
  }

  return aST;
}

std::string GeomAPI_Shape::shapeTypeStr() const
{
  ShapeType aShapeType = shapeType();
  std::string aShapeTypeStr;

  switch(aShapeType) {
    case COMPOUND: {
      aShapeTypeStr = "Compound";
      break;
    }
    case COMPSOLID: {
      aShapeTypeStr = "CompSolid";
      break;
    }
    case SOLID: {
      aShapeTypeStr = "Solid";
      break;
    }
    case SHELL: {
      aShapeTypeStr = "Shell";
      break;
    }
    case FACE: {
      aShapeTypeStr = "Face";
      break;
    }
    case WIRE: {
      aShapeTypeStr = "Wire";
      break;
    }
    case EDGE: {
      aShapeTypeStr = "Edge";
      break;
    }
    case VERTEX: {
      aShapeTypeStr = "Vertex";
      break;
    }
    case SHAPE: {
      aShapeTypeStr = "Shape";
      break;
    }
  }

  return aShapeTypeStr;
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
