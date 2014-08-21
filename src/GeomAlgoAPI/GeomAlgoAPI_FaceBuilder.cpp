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

boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_FaceBuilder::square(
    boost::shared_ptr<GeomAPI_Pnt> theCenter, boost::shared_ptr<GeomAPI_Dir> theNormal,
    const double theSize)
{
  const gp_Pnt& aCenter = theCenter->impl<gp_Pnt>();
  const gp_Dir& aDir = theNormal->impl<gp_Dir>();
  gp_Pln aPlane(aCenter, aDir);
  // half of the size in each direction from the center
  BRepBuilderAPI_MakeFace aFaceBuilder(aPlane, -theSize / 2., theSize / 2., -theSize / 2.,
                                       theSize / 2.);
  boost::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  aRes->setImpl(new TopoDS_Shape(aFaceBuilder.Face()));
  return aRes;
}

boost::shared_ptr<GeomAPI_Pln> GeomAlgoAPI_FaceBuilder::plane(
    boost::shared_ptr<GeomAPI_Shape> theFace)
{
  boost::shared_ptr<GeomAPI_Pln> aResult;
  if (!theFace)
    return aResult;  // bad shape
  TopoDS_Shape aShape = theFace->impl<TopoDS_Shape>();
  if (aShape.IsNull())
    return aResult;  // null shape
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
  aResult = boost::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(aA, aB, aC, aD));
  return aResult;
}
