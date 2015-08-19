// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAPI_Face.cpp
// Created:     2 Dec 2014
// Author:      Artem ZHIDKOV

#include <GeomAPI_Face.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <Geom_Surface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

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
  if (!theFace->isFace())
    return false;

  const TopoDS_Shape& aMyShape = const_cast<GeomAPI_Face*>(this)->impl<TopoDS_Shape>();
  const TopoDS_Shape& aInShape = theFace->impl<TopoDS_Shape>();

  Handle(Geom_Surface) aMySurf = BRep_Tool::Surface(TopoDS::Face(aMyShape));
  Handle(Geom_Surface) aInSurf = BRep_Tool::Surface(TopoDS::Face(aInShape));

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

  return true;
}

bool GeomAPI_Face::isPlanar() const
{
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Face*>(this)->impl<TopoDS_Shape>();
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aShape));
  Handle(Geom_RectangularTrimmedSurface) aTrimmed = 
    Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf);
  if (!aTrimmed.IsNull())
    aSurf = aTrimmed->BasisSurface();
  GeomLib_IsPlanarSurface isPlanar(aSurf);
  return isPlanar.IsPlanar() == Standard_True;
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
  const TopoDS_Shape& aShape = const_cast<GeomAPI_Face*>(this)->impl<TopoDS_Shape>();
  BRepAdaptor_Surface aSurfAdapt(TopoDS::Face(aShape));

  if (aSurfAdapt.GetType() != GeomAbs_Plane)
    return std::shared_ptr<GeomAPI_Pln>();

  // Obtain central point
  double aUMin, aUMax, aVMin, aVMax;
  aUMin = aSurfAdapt.FirstUParameter();
  aUMax = aSurfAdapt.LastUParameter();
  aVMin = aSurfAdapt.FirstVParameter();
  aVMax = aSurfAdapt.LastVParameter();
  gp_Pnt aCentralPnt;
  gp_Vec aDU, aDV;
  aSurfAdapt.D1((aUMin+aUMax)*0.5, (aVMin+aVMax)*0.5, aCentralPnt, aDU, aDV);
  std::shared_ptr<GeomAPI_Pnt> aCenter(
      new GeomAPI_Pnt(aCentralPnt.X(), aCentralPnt.Y(), aCentralPnt.Z()));

  // Obtain plane direction
  gp_XYZ aNormalVec = aDU.XYZ().Crossed(aDV.XYZ());
  if (aNormalVec.SquareModulus() < Precision::Confusion() * Precision::Confusion())
    return std::shared_ptr<GeomAPI_Pln>();
  std::shared_ptr<GeomAPI_Dir> aNormal(
      new GeomAPI_Dir(aNormalVec.X(), aNormalVec.Y(), aNormalVec.Z()));

  std::shared_ptr<GeomAPI_Pln> aResult(new GeomAPI_Pln(aCenter, aNormal));
  return aResult;
}
