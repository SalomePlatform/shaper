// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_FaceBuilder.cpp
// Created:     23 Apr 2014
// Author:      Mikhail PONIKAROV

#include <GeomAlgoAPI_FaceBuilder.h>
#include <gp_Pln.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>

std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_FaceBuilder::square(
    std::shared_ptr<GeomAPI_Pnt> theCenter, std::shared_ptr<GeomAPI_Dir> theNormal,
    const double theSize)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();
  gp_Pln aPlane(aCenter, aDir);
  // half of the size in each direction from the center
  BRepBuilderAPI_MakeFace aFaceBuilder(aPlane, -theSize / 2., theSize / 2., -theSize / 2.,
                                       theSize / 2.);
  std::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  aRes->setImpl(new TopoDS_Shape(aFaceBuilder.Face()));
  return aRes;
}

std::shared_ptr<GeomAPI_Pln> GeomAlgoAPI_FaceBuilder::plane(
    std::shared_ptr<GeomAPI_Shape> theFace)
{
  std::shared_ptr<GeomAPI_Pln> aResult;
  if (!theFace)
    return aResult;  // bad shape
  TopoDS_Shape aShape = theFace->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return aResult;  // null shape
  if (aShape.ShapeType() != TopAbs_FACE)
    return aResult;  // not face
  TopoDS_Face aFace = TopoDS::Face(aShape);
  if (aFace.IsNull())
    return aResult;  // not face
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if (aSurf.IsNull())
    return aResult;  // no surface
  Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aSurf);
  if (aPlane.IsNull())
    return aResult;  // not planar
  double aA, aB, aC, aD;
  aPlane->Coefficients(aA, aB, aC, aD);
  aResult = std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(aA, aB, aC, aD));
  return aResult;
}


std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_FaceBuilder::
  planarFace(std::shared_ptr<GeomAPI_Pln> thePlane,
             double theX, double theY,
             double theWidth, double theHeight)
{
  double aA, aB, aC, aD;
  thePlane->coefficients(aA, aB, aC, aD);
  gp_Pln aPlane(aA, aB, aC, aD);

  // half of the size in each direction from the center
  BRepBuilderAPI_MakeFace aFaceBuilder(aPlane, theX, theX + theWidth, 
                                       theY, theY + theHeight);
  std::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  aRes->setImpl(new TopoDS_Shape(aFaceBuilder.Face()));
  return aRes;
}