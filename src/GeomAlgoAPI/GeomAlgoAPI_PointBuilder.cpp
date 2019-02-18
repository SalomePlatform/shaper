// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "GeomAlgoAPI_PointBuilder.h"

#include <GeomAPI_Edge.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Lin.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_Vertex.h>

#include <BRep_Tool.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <GCPnts_AbscissaPoint.hxx>
#include <Geom_Line.hxx>
#include <GeomAdaptor_Curve.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <gp_Pln.hxx>
#include <gp_Pnt.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Vertex.hxx>

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex>
  GeomAlgoAPI_PointBuilder::vertex(const std::shared_ptr<GeomAPI_Pnt> thePoint)
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
std::shared_ptr<GeomAPI_Pnt>
  GeomAlgoAPI_PointBuilder::point(const std::shared_ptr<GeomAPI_Shape> theVertex)
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
std::shared_ptr<GeomAPI_Vertex> GeomAlgoAPI_PointBuilder::vertexOnEdge(
                                            const std::shared_ptr<GeomAPI_Edge> theEdge,
                                            const double theValue,
                                            const bool theIsPercent,
                                            const bool theIsReverse)
{
  std::shared_ptr<GeomAPI_Vertex> aVertex;

  if(!theEdge.get()) {
    return aVertex;
  }

  double aValue = theValue;
  if(theIsPercent) {
    aValue = theEdge->length() / 100.0 * aValue;
  }

  const TopoDS_Edge& anEdge = TopoDS::Edge(theEdge->impl<TopoDS_Shape>());
  Standard_Real aUFirst, aULast;
  Handle(Geom_Curve) anEdgeCurve = BRep_Tool::Curve(anEdge, aUFirst, aULast);

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
  const std::shared_ptr<GeomAPI_Edge> theEdge)
{
  std::shared_ptr<GeomAPI_Vertex> aVertex;

  if (!theVertex.get() || !theEdge.get()) {
    return aVertex;
  }

  std::shared_ptr<GeomAPI_Pnt> aProjPnt = theVertex->point();
  gp_Pnt aPnt(aProjPnt->x(), aProjPnt->y(), aProjPnt->z());

  TopoDS_Edge anEdge = TopoDS::Edge(theEdge->impl<TopoDS_Shape>());
  double aFirstOnCurve, aLastOnCurve;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirstOnCurve, aLastOnCurve);

  if (aCurve.IsNull()) {
    return aVertex;
  }

  GeomAPI_ProjectPointOnCurve aProjection(aPnt, aCurve);

  if (aProjection.NbPoints() != 0) {
    gp_Pnt aNearestPoint = aProjection.NearestPoint();
    aVertex.reset(new GeomAPI_Vertex(aNearestPoint.X(), aNearestPoint.Y(), aNearestPoint.Z()));
  }

  return aVertex;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> GeomAlgoAPI_PointBuilder::vertexByProjection(
    const std::shared_ptr<GeomAPI_Vertex> theVertex,
    const std::shared_ptr<GeomAPI_Face> theFace)
{
  std::shared_ptr<GeomAPI_Vertex> aVertex;

  if (theVertex.get() && theFace.get() && theFace->isPlanar()) {
    std::shared_ptr<GeomAPI_Pnt> aProjPnt = theVertex->point();
    std::shared_ptr<GeomAPI_Pln> aProjPln = theFace->getPlane();

    std::shared_ptr<GeomAPI_Pnt> aPnt = aProjPln->project(aProjPnt);

    if (aPnt.get())
      aVertex.reset(new GeomAPI_Vertex(aPnt->x(), aPnt->y(), aPnt->z()));
  }

  return aVertex;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Vertex> GeomAlgoAPI_PointBuilder::vertexByIntersection(
    const std::shared_ptr<GeomAPI_Edge> theEdge1,
    const std::shared_ptr<GeomAPI_Edge> theEdge2)
{
  std::shared_ptr<GeomAPI_Vertex> aVertex;

  if (theEdge1.get() && theEdge2.get() && theEdge1->isLine() && theEdge2->isLine()) {
    std::shared_ptr<GeomAPI_Lin> aLin1 = theEdge1->line();
    std::shared_ptr<GeomAPI_Lin> aLin2 = theEdge2->line();

    std::shared_ptr<GeomAPI_Pnt> aPnt = aLin1->intersect(aLin2);

    if (aPnt.get())
      aVertex.reset(new GeomAPI_Vertex(aPnt->x(), aPnt->y(), aPnt->z()));
  }

  return aVertex;
}
