// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_PointBuilder.cpp
// Created:     02 Jun 2014
// Author:      Mikhail PONIKAROV

#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Vertex.h>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <gp_Pnt.hxx>

std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_PointBuilder::point(
    std::shared_ptr<GeomAPI_Pnt> thePoint)
{
  const gp_Pnt& aPnt = thePoint->impl<gp_Pnt>();
  BRepBuilderAPI_MakeVertex aMaker(aPnt);
  TopoDS_Vertex aVertex = aMaker.Vertex();
  std::shared_ptr<GeomAPI_Vertex> aRes(new GeomAPI_Vertex);
  aRes->setImpl(new TopoDS_Shape(aVertex));
  return aRes;
}


std::shared_ptr<GeomAPI_Pnt> GeomAlgoAPI_PointBuilder::point(std::shared_ptr<GeomAPI_Shape> theVertex)
{
  TopoDS_Shape aShape = theVertex->impl<TopoDS_Shape>();
  if ((!aShape.IsNull()) && (aShape.ShapeType() == TopAbs_VERTEX)) {
    TopoDS_Vertex aVertex = TopoDS::Vertex(aShape);
    gp_Pnt aPoint = BRep_Tool::Pnt(aVertex);
    std::shared_ptr<GeomAPI_Pnt> aPnt(new GeomAPI_Pnt(aPoint.X(), aPoint.Y(), aPoint.Z()));
    return aPnt;
  }
  return std::shared_ptr<GeomAPI_Pnt>();
}
