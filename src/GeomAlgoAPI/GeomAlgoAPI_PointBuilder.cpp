// File:        GeomAlgoAPI_PointBuilder.cpp
// Created:     02 Jun 2014
// Author:      Mikhail PONIKAROV



#include <GeomAlgoAPI_PointBuilder.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <TopoDS_Vertex.hxx>

boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_PointBuilder::point(
    boost::shared_ptr<GeomAPI_Pnt> thePoint)
{
  const gp_Pnt& aPnt = thePoint->impl<gp_Pnt>();
  BRepBuilderAPI_MakeVertex aMaker(aPnt);
  TopoDS_Vertex aVertex = aMaker.Vertex();
  boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  aRes->setImpl(new TopoDS_Shape(aVertex));
  return aRes;
}
