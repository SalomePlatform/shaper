// File:        GeomAlgoAPI_EdgeBuilder.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include <GeomAlgoAPI_EdgeBuilder.h>
#include <gp_Pln.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>

boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_EdgeBuilder::line(
  boost::shared_ptr<GeomAPI_Pnt> theStart, boost::shared_ptr<GeomAPI_Pnt> theEnd)
{
  const gp_Pnt& aStart = theStart->impl<gp_Pnt>();
  const gp_Pnt& anEnd = theEnd->impl<gp_Pnt>();

  if (aStart.IsEqual(anEnd, Precision::Confusion()))
    return boost::shared_ptr<GeomAPI_Shape>();
  if (Abs(aStart.SquareDistance(anEnd)) > 1.e+100)
    return boost::shared_ptr<GeomAPI_Shape>();
  BRepBuilderAPI_MakeEdge anEdgeBuilder(aStart, anEnd);
  boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}
