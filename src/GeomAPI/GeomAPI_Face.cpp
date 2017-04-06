// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Face.cpp
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#include "GeomAPI_Face.h"

#include "GeomAPI_Dir.h"
#include "GeomAPI_Pln.h"
#include "GeomAPI_Pnt.h"

#include <BOPTools_AlgoTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <IntTools_Context.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

GeomAPI_Face::GeomAPI_Face()
  : GeomAPI_Shape()
{
}

GeomAPI_Face::GeomAPI_Face(const std::shared_ptr<GeomAPI_Shape>& theShape)
{
  if (!theShape->isNull() && theShape->isFace()) {
    setImpl(new TopoDS_Shape(theShape->impl<TopoDS_Shape>()));
  }
}

bool GeomAPI_Face::isEqual(std::shared_ptr<GeomAPI_Shape> theFace) const
{
  if (!theFace.get())
    return false;

  if (!theFace->isFace())
    return false;

  const TopoDS_Shape& aMyShape = const_cast<GeomAPI_Face*>(this)->impl<TopoDS_Shape>();
  const TopoDS_Shape& aInShape = theFace->impl<TopoDS_Shape>();

  TopoDS_Face aMyFace = TopoDS::Face(aMyShape);
  TopoDS_Face aInFace = TopoDS::Face(aInShape);

  Handle(Geom_Surface) aMySurf = BRep_Tool::Surface(aMyFace);
  Handle(Geom_Surface) aInSurf = BRep_Tool::Surface(aInFace);

  // Check that surfaces a the same type
  if (aMySurf->DynamicType() != aInSurf->DynamicType())
    return false;

  // Get parameters of surfaces
  double aMyUMin, aMyUMax, aMyVMin, aMyVMax;
  aMySurf->Bounds(aMyUMin, aMyUMax, aMyVMin, aMyVMax);
  double aInUMin, aInUMax, aInVMin, aInVMax;
  aInSurf->Bounds(aInUMin, aInUMax, aInVMin, aInVMax);

  // Check that parameters are the same
  if (fabs(aMyUMin - aInUMin) > Precision::PConfusion() ||
      fabs(aMyUMax - aInUMax) > Precision::PConfusion() ||
      fabs(aMyVMin - aInVMin) > Precision::PConfusion() ||
      fabs(aMyVMax - aInVMax) > Precision::PConfusion())
    return false;

  Handle(IntTools_Context) aContext = new IntTools_Context();
  // Double check needed bacause BOPTools_AlgoTools::CheckSameGeom not very smart.
  Standard_Boolean aRes = BOPTools_AlgoTools::CheckSameGeom(aMyFace, aInFace, aContext)
    && BOPTools_AlgoTools::CheckSameGeom(aInFace, aMyFace, aContext);

  return aRes == Standard_True;
}

bool GeomAPI_Face::isCylindrical() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Face*>(this)->impl<TopoDS_Shape>();
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aShape));
  Handle(Geom_RectangularTrimmedSurface) aTrimmed =
    Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf);
  if (!aTrimmed.IsNull())
    aSurf = aTrimmed->BasisSurface();
  return aSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) == Standard_True;
}

std::shared_ptr<GeomAPI_Pln> GeomAPI_Face::getPlane() const
{
  std::shared_ptr<GeomAPI_Pln> aResult;
  TopoDS_Shape aShape = this->impl<TopoDS_Shape>();
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
  GeomLib_IsPlanarSurface isPlanar(aSurf);
  if(!isPlanar.IsPlanar()) {
    return aResult;
  }
  gp_Pln aPln = isPlanar.Plan();
  double aA, aB, aC, aD;
  aPln.Coefficients(aA, aB, aC, aD);
  if (aFace.Orientation() == TopAbs_REVERSED) {
    aA = -aA;
    aB = -aB;
    aC = -aC;
    aD = -aD;
  }
  aResult = std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(aA, aB, aC, aD));
  return aResult;
}
