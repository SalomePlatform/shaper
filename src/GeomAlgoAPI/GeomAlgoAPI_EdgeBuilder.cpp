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

boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_EdgeBuilder::lineCircle(
    boost::shared_ptr<GeomAPI_Pnt> theCenter,
    boost::shared_ptr<GeomAPI_Dir> theNormal, double theRadius)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();

  gp_Circ aCircle(gp_Ax2(aCenter, aDir), theRadius);

  BRepBuilderAPI_MakeEdge anEdgeBuilder(aCircle);
  boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}

boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_EdgeBuilder::lineCircleArc(
    boost::shared_ptr<GeomAPI_Pnt> theCenter,
    boost::shared_ptr<GeomAPI_Pnt> theStartPoint,
    boost::shared_ptr<GeomAPI_Pnt> theEndPoint,
    boost::shared_ptr<GeomAPI_Dir> theNormal)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();

  double aRadius = theCenter->distance(theStartPoint);
  gp_Circ aCircle(gp_Ax2(aCenter, aDir), aRadius);

  const gp_Pnt& aStart = theStartPoint->impl<gp_Pnt>();
  const gp_Pnt& anEnd = theEndPoint->impl<gp_Pnt>();

  BRepBuilderAPI_MakeEdge anEdgeBuilder;
  if (aStart.IsEqual(anEnd, Precision::Confusion()) ||
      gp_Pnt(0, 0, 0).IsEqual(anEnd, Precision::Confusion()))
    anEdgeBuilder = BRepBuilderAPI_MakeEdge(aCircle);
  else
    anEdgeBuilder = BRepBuilderAPI_MakeEdge(aCircle, aStart, anEnd);

  boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  TopoDS_Edge anEdge = anEdgeBuilder.Edge();
  aRes->setImpl(new TopoDS_Shape(anEdge));
  return aRes;
}
