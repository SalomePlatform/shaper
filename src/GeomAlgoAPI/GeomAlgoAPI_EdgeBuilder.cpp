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

#include <gp_Ax2.hxx>
#include <gp_Circ.hxx>

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::line(
    std::shared_ptr<GeomAPI_Pnt> theStart, std::shared_ptr<GeomAPI_Pnt> theEnd)
{
  const gp_Pnt& aStart = theStart->impl<gp_Pnt>();
  const gp_Pnt& anEnd = theEnd->impl<gp_Pnt>();

  if (aStart.IsEqual(anEnd, Precision::Confusion()))
    return std::shared_ptr<GeomAPI_Edge>();
  if (Abs(aStart.SquareDistance(anEnd)) > 1.e+100)
    return std::shared_ptr<GeomAPI_Edge>();
  BRepBuilderAPI_MakeEdge anEdgeBuilder(aStart, anEnd);
  std::shared_ptr<GeomAPI_Edge> aRes(new GeomAPI_Edge);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::lineCircle(
    std::shared_ptr<GeomAPI_Pnt> theCenter, std::shared_ptr<GeomAPI_Dir> theNormal,
    double theRadius)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();

  gp_Circ aCircle(gp_Ax2(aCenter, aDir), theRadius);

  BRepBuilderAPI_MakeEdge anEdgeBuilder(aCircle);
  std::shared_ptr<GeomAPI_Edge> aRes(new GeomAPI_Edge);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}

std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_EdgeBuilder::lineCircleArc(
    std::shared_ptr<GeomAPI_Pnt> theCenter, std::shared_ptr<GeomAPI_Pnt> theStartPoint,
    std::shared_ptr<GeomAPI_Pnt> theEndPoint, std::shared_ptr<GeomAPI_Dir> theNormal)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();

  double aRadius = theCenter->distance(theStartPoint);
  gp_Circ aCircle(gp_Ax2(aCenter, aDir), aRadius);

  const gp_Pnt& aStart = theStartPoint->impl<gp_Pnt>();
  const gp_Pnt& anEnd = theEndPoint->impl<gp_Pnt>();

  BRepBuilderAPI_MakeEdge anEdgeBuilder;
  if (aStart.IsEqual(anEnd, Precision::Confusion())
      || gp_Pnt(0, 0, 0).IsEqual(anEnd, Precision::Confusion()))
    anEdgeBuilder = BRepBuilderAPI_MakeEdge(aCircle);
  else
    anEdgeBuilder = BRepBuilderAPI_MakeEdge(aCircle, aStart, anEnd);

  std::shared_ptr<GeomAPI_Edge> aRes(new GeomAPI_Edge);
  anEdgeBuilder.Build();

  if (anEdgeBuilder.IsDone())
    aRes->setImpl(new TopoDS_Shape(anEdgeBuilder.Edge()));
  else
    aRes = std::shared_ptr<GeomAPI_Edge>();
  return aRes;
}
