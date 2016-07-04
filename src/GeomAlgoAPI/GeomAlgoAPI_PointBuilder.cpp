// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_PointBuilder.cpp
// Created:     02 Jun 2014
// Author:      Mikhail PONIKAROV

#include "GeomAlgoAPI_PointBuilder.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Vertex.h>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Vertex.hxx>

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> GeomAlgoAPI_PointBuilder::vertex(const std::shared_ptr<GeomAPI_Pnt> thePoint)
{
  const gp_Pnt& aPnt = thePoint->impl<gp_Pnt>();
  BRepBuilderAPI_MakeVertex aMaker(aPnt);
  TopoDS_Vertex aVertex = aMaker.Vertex();
  std::shared_ptr<GeomAPI_Vertex> aRes(new GeomAPI_Vertex);
  aRes->setImpl(new TopoDS_Shape(aVertex));
  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> GeomAlgoAPI_PointBuilder::vertex(const double theX,
                                                                 const double theY,
                                                                 const double theZ)
{
  const gp_Pnt aPnt(theX, theY, theZ);
  BRepBuilderAPI_MakeVertex aMaker(aPnt);
  TopoDS_Vertex aVertex = aMaker.Vertex();
  std::shared_ptr<GeomAPI_Vertex> aRes(new GeomAPI_Vertex);
  aRes->setImpl(new TopoDS_Shape(aVertex));
  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAlgoAPI_PointBuilder::point(const std::shared_ptr<GeomAPI_Shape> theVertex)
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

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> GeomAlgoAPI_PointBuilder::vertexOnEdge(const std::shared_ptr<GeomAPI_Edge> theEdge,
                                                                       const double theValue,
                                                                       const bool theIsPercent,
                                                                       const bool theIsReverse)
{
  if(!theEdge.get()) {
    return NULL;
  }

  double aValue = theValue;
  if(theIsPercent) {
    aValue = theEdge->length() / 100.0 * aValue;
  }

  const TopoDS_Edge& anEdge = TopoDS::Edge(theEdge->impl<TopoDS_Shape>());
  Standard_Real aUFirst, aULast;
  Handle(Geom_Curve) anEdgeCurve = BRep_Tool::Curve(anEdge, aUFirst, aULast);

  std::shared_ptr<GeomAPI_Vertex> aVertex;
  if(!anEdgeCurve.IsNull() ) {
    Handle(Geom_Curve) aReOrientedCurve = anEdgeCurve;

    if(theIsReverse) {
      aReOrientedCurve = anEdgeCurve->Reversed();
      aUFirst = anEdgeCurve->ReversedParameter(aULast);
    }

    // Get the point by length
    GeomAdaptor_Curve anAdapCurve = GeomAdaptor_Curve(aReOrientedCurve);
    GCPnts_AbscissaPoint anAbsPnt(anAdapCurve, aValue, aUFirst);
    Standard_Real aParam = anAbsPnt.Parameter();
    gp_Pnt aPnt = anAdapCurve.Value(aParam);
    BRepBuilderAPI_MakeVertex aMkVertex(aPnt);
    const TopoDS_Vertex& aShape = aMkVertex.Vertex();
    aVertex.reset(new GeomAPI_Vertex());
    aVertex->setImpl(new TopoDS_Vertex(aShape));
  }

  return aVertex;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> GeomAlgoAPI_PointBuilder::vertexByProjection(
    const std::shared_ptr<GeomAPI_Vertex> theVertex,
    const std::shared_ptr<GeomAPI_Face> thePlane)
{
  if(!theVertex.get() || !thePlane.get() || !thePlane->isPlanar()) {
    return NULL;
  }

  std::shared_ptr<GeomAPI_Pnt> aGeomPnt = theVertex->point();
  gp_Pnt aPnt = aGeomPnt->impl<gp_Pnt>();

  std::shared_ptr<GeomAPI_Pln> aGeomPln = thePlane->getPlane();
  gp_Pln aPln = aGeomPln->impl<gp_Pln>();

  gp_Dir aPntAxis = aPnt.XYZ() - aPln.Location().XYZ();
  gp_Dir aPlnNorm = aPln.Axis().Direction();

  if(aPntAxis * aPlnNorm > 0) {
    aPlnNorm.Reverse();
  }

  double aDistance = aPln.Distance(aPnt);
  aPnt.Translate(gp_Vec(aPlnNorm) * aDistance);

  return std::shared_ptr<GeomAPI_Vertex>(new GeomAPI_Vertex(aPnt.X(), aPnt.Y(), aPnt.Z()));
}
