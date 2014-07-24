// File:        GeomAPI_Edge.cpp
// Created:     24 Jul 2014
// Author:      Artem ZHIDKOV

#include<GeomAPI_Edge.h>

#include <TopoDS_Shape.hxx>
#include <BRep_TEdge.hxx>
#include <BRep_CurveRepresentation.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Line.hxx>
#include <Geom_Circle.hxx>

GeomAPI_Edge::GeomAPI_Edge()
  : GeomAPI_Shape()
{}

bool GeomAPI_Edge::isLine() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  Handle(BRep_TEdge) anEdge = Handle(BRep_TEdge)::DownCast(aShape.TShape());
  if (anEdge->Curves().Extent() != 1)
    return false; // too many curves in the edge
  Handle(Geom_Curve) aCurve = anEdge->Curves().First()->Curve3D();
  if (aCurve->IsKind(STANDARD_TYPE(Geom_Line)))
    return true;
  return false;
}

bool GeomAPI_Edge::isCircle() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  Handle(BRep_TEdge) anEdge = Handle(BRep_TEdge)::DownCast(aShape.TShape());
  if (anEdge->Curves().Extent() != 1)
    return false; // too many curves in the edge
  Handle(Geom_Curve) aCurve = anEdge->Curves().First()->Curve3D();
  if (aCurve->IsKind(STANDARD_TYPE(Geom_Circle)) && aCurve->IsClosed())
    return true;
  return false;
}

bool GeomAPI_Edge::isArc() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Edge*>(this)->impl<TopoDS_Shape>();
  Handle(BRep_TEdge) anEdge = Handle(BRep_TEdge)::DownCast(aShape.TShape());
  if (anEdge->Curves().Extent() != 1)
    return false; // too many curves in the edge
  Handle(Geom_Curve) aCurve = anEdge->Curves().First()->Curve3D();
  if (aCurve->IsKind(STANDARD_TYPE(Geom_Circle)) && !aCurve->IsClosed())
    return true;
  return false;
}
