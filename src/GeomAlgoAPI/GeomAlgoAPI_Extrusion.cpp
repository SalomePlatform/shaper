// File:        GeomAlgoAPI_Extrusion.cpp
// Created:     06 Jun 2014
// Author:      Artem ZHIDKOV

#include <GeomAlgoAPI_Extrusion.h>

#include <gp_Pln.hxx>

#include <BRepPrimAPI_MakePrism.hxx>
#include <Geom_Plane.hxx>
#include <Geom_Surface.hxx>
#include <TopExp_Explorer.hxx>

#include <Precision.hxx>
const double tolerance = Precision::Angular();

boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Extrusion::makeExtrusion(
    boost::shared_ptr<GeomAPI_Shape> theShape, boost::shared_ptr<GeomAPI_Dir> theDir,
    double theSize)
{
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  gp_Vec aDir(theDir->impl<gp_Dir>().XYZ() * theSize);

  TopoDS_Shape aPrism = BRepPrimAPI_MakePrism(aShape, aDir);
  if (aPrism.IsNull())
    return boost::shared_ptr<GeomAPI_Shape>();

  boost::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape());
  aResult->setImpl(new TopoDS_Shape(aPrism));
  return aResult;
}

boost::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Extrusion::makeExtrusion(
    boost::shared_ptr<GeomAPI_Shape> theShape, double theSize)
{
  bool isFirstNorm = true;
  gp_Dir aShapeNormal;

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  TopExp_Explorer aFaceExp(aShape, TopAbs_FACE);
  for (; aFaceExp.More(); aFaceExp.Next()) {
    const TopoDS_Face& aFace = (const TopoDS_Face&) aFaceExp.Current();
    Handle(BRep_TFace) aBFace = Handle(BRep_TFace)::DownCast(aFace.TShape());
    if (aBFace.IsNull())
      return boost::shared_ptr<GeomAPI_Shape>();

    Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(aBFace->Surface());
    if (aPlane.IsNull())  // non-planar shapes is not supported for extrusion yet
      continue;

    const gp_Dir& aNormal = aPlane->Pln().Axis().Direction();
    if (isFirstNorm) {
      aShapeNormal = aNormal;
      isFirstNorm = false;
    } else if (!aShapeNormal.IsEqual(aNormal, tolerance))  // non-planar shapes is not supported for extrusion yet
      return boost::shared_ptr<GeomAPI_Shape>();
  }

  boost::shared_ptr<GeomAPI_Dir> aDir(
      new GeomAPI_Dir(aShapeNormal.X(), aShapeNormal.Y(), aShapeNormal.Z()));

  return GeomAlgoAPI_Extrusion::makeExtrusion(theShape, aDir, theSize);
}
