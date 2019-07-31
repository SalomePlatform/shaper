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

#include "GeomAPI_Face.h"

#include "GeomAPI_Dir.h"
#include "GeomAPI_Pln.h"
#include "GeomAPI_Pnt.h"
#include "GeomAPI_Sphere.h"
#include "GeomAPI_Cylinder.h"
#include "GeomAPI_Cone.h"
#include "GeomAPI_Torus.h"

#include <BOPTools_AlgoTools.hxx>
#include <BRep_Tool.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepGProp_Face.hxx>
#include <BRepTools.hxx>
#include <BRepTopAdaptor_TopolTool.hxx>
#include <Geom_Surface.hxx>
#include <Geom_SphericalSurface.hxx>
#include <Geom_ConicalSurface.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <IntPatch_ImpImpIntersection.hxx>
#include <IntTools_Context.hxx>
#include <Standard_Type.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Face.hxx>

#include <gp_Sphere.hxx>
#include <gp_Cylinder.hxx>
#include <gp_Cone.hxx>
#include <gp_Torus.hxx>


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
  // Double check needed because BOPTools_AlgoTools::AreFacesSameDomain not very smart.
  Standard_Boolean aRes = BOPTools_AlgoTools::AreFacesSameDomain(aMyFace, aInFace, aContext)
    && BOPTools_AlgoTools::AreFacesSameDomain(aInFace, aMyFace, aContext);

  return aRes == Standard_True;
}

static Handle(Geom_Surface) baseSurface(const TopoDS_Face& theFace)
{
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(theFace);
  while (aSurf->IsKind(STANDARD_TYPE(Geom_RectangularTrimmedSurface))) {
    Handle(Geom_RectangularTrimmedSurface) rts =
        Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurf);
    aSurf = rts->BasisSurface();
  }
  return aSurf;
}

bool GeomAPI_Face::isSameGeometry(const std::shared_ptr<GeomAPI_Shape> theShape) const
{
  if (!theShape->isFace())
    return false;
  if (isSame(theShape))
    return true;

  GeomFacePtr anOther = theShape->face();
  if (isPlanar() && anOther->isPlanar()) {
    GeomPlanePtr anOwnPlane = getPlane();
    GeomPlanePtr anOtherPlane = anOther->getPlane();
    return anOwnPlane->isCoincident(anOtherPlane);
  }

  TopoDS_Face anOwnFace = TopoDS::Face(impl<TopoDS_Shape>());
  TopoDS_Face anOtherFace = TopoDS::Face(theShape->impl<TopoDS_Shape>());

  Handle(Geom_Surface) anOwnSurf = baseSurface(anOwnFace);
  Handle(Geom_Surface) anOtherSurf = baseSurface(anOtherFace);

  // case of two elementary surfaces
  if (anOwnSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)) &&
      anOtherSurf->IsKind(STANDARD_TYPE(Geom_ElementarySurface)))
  {
    Handle(GeomAdaptor_HSurface) aGA1 = new GeomAdaptor_HSurface(anOwnSurf);
    Handle(GeomAdaptor_HSurface) aGA2 = new GeomAdaptor_HSurface(anOtherSurf);

    Handle(BRepTopAdaptor_TopolTool) aTT1 = new BRepTopAdaptor_TopolTool();
    Handle(BRepTopAdaptor_TopolTool) aTT2 = new BRepTopAdaptor_TopolTool();

    try {
      IntPatch_ImpImpIntersection anIIInt(aGA1, aTT1, aGA2, aTT2,
                                          Precision::Confusion(),
                                          Precision::Confusion());
      if (!anIIInt.IsDone() || anIIInt.IsEmpty())
        return false;

      return anIIInt.TangentFaces();
    }
    catch (Standard_Failure const&) {
      return false;
    }
  }

  // case of two cylindrical surfaces, at least one of which is a swept surface
  // swept surfaces: SurfaceOfLinearExtrusion, SurfaceOfRevolution
  if ((anOwnSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) ||
       anOwnSurf->IsKind(STANDARD_TYPE(Geom_SweptSurface))) &&
      (anOtherSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface)) ||
       anOtherSurf->IsKind(STANDARD_TYPE(Geom_SweptSurface))))
  {
    GeomCylinderPtr anOwnCyl = getCylinder();
    GeomCylinderPtr anOtherCyl = anOther->getCylinder();
    return anOwnCyl && anOtherCyl && anOwnCyl->isCoincident(anOtherCyl);
  }

  return false;
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

std::shared_ptr<GeomAPI_Sphere> GeomAPI_Face::getSphere() const
{
  GeomSpherePtr aSphere;

  const TopoDS_Face& aFace = TopoDS::Face(impl<TopoDS_Shape>());
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if (aSurf->IsKind(STANDARD_TYPE(Geom_SphericalSurface))) {
    gp_Sphere aSph = Handle(Geom_SphericalSurface)::DownCast(aSurf)->Sphere();
    const gp_Pnt& aCenter = aSph.Location();
    double aRadius = aSph.Radius();

    GeomPointPtr aCenterPnt(new GeomAPI_Pnt(aCenter.X(), aCenter.Y(), aCenter.Z()));
    aSphere = GeomSpherePtr(new GeomAPI_Sphere(aCenterPnt, aRadius));
  }
  return aSphere;
}

std::shared_ptr<GeomAPI_Cylinder> GeomAPI_Face::getCylinder() const
{
  GeomCylinderPtr aCylinder;

  const TopoDS_Face& aFace = TopoDS::Face(impl<TopoDS_Shape>());
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if (aSurf->IsKind(STANDARD_TYPE(Geom_CylindricalSurface))) {
    gp_Cylinder aCyl = Handle(Geom_CylindricalSurface)::DownCast(aSurf)->Cylinder();
    gp_Pnt aLoc = aCyl.Location();
    const gp_Dir& aDir = aCyl.Position().Direction();
    double aRadius = aCyl.Radius();

    double aUMin, aUMax, aVMin, aVMax;
    BRepTools::UVBounds(aFace, aUMin, aUMax, aVMin, aVMax);
    double aHeight = aVMax - aVMin;

    aLoc.ChangeCoord() += aDir.XYZ() * aVMin;
    GeomPointPtr aLocation(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
    GeomDirPtr aDirection(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
    aCylinder = GeomCylinderPtr(new GeomAPI_Cylinder(aLocation, aDirection, aRadius, aHeight));
  }
  return aCylinder;
}

std::shared_ptr<GeomAPI_Cone> GeomAPI_Face::getCone() const
{
  GeomConePtr aCone;

  const TopoDS_Face& aFace = TopoDS::Face(impl<TopoDS_Shape>());
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if (aSurf->IsKind(STANDARD_TYPE(Geom_ConicalSurface))) {
    gp_Cone aCon = Handle(Geom_ConicalSurface)::DownCast(aSurf)->Cone();
    gp_Pnt aLoc = aCon.Location();
    gp_Dir aDir = aCon.Position().Direction();

    double aUMin, aUMax, aVMin, aVMax;
    BRepTools::UVBounds(aFace, aUMin, aUMax, aVMin, aVMax);

    double aSemiAngle = Abs(aCon.SemiAngle());
    double aRadius1 = Abs(aCon.RefRadius() + aVMin * Sin(aCon.SemiAngle()));
    double aRadius2 = Abs(aCon.RefRadius() + aVMax * Sin(aCon.SemiAngle()));

    aLoc.ChangeCoord() += aDir.XYZ() * aVMin * Cos(aCon.SemiAngle());

    GeomPointPtr aLocation(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
    GeomDirPtr aDirection(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
    aCone = GeomConePtr(new GeomAPI_Cone(aLocation, aDirection, aSemiAngle, aRadius1, aRadius2));
  }
  return aCone;
}

std::shared_ptr<GeomAPI_Torus> GeomAPI_Face::getTorus() const
{
  GeomTorusPtr aTorus;

  const TopoDS_Face& aFace = TopoDS::Face(impl<TopoDS_Shape>());
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if (aSurf->IsKind(STANDARD_TYPE(Geom_ToroidalSurface))) {
    gp_Torus aTor = Handle(Geom_ToroidalSurface)::DownCast(aSurf)->Torus();
    const gp_Pnt& aLoc = aTor.Location();
    const gp_Dir& aDir = aTor.Position().Direction();
    double aMajorRadius = aTor.MajorRadius();
    double aMinorRadius = aTor.MinorRadius();

    GeomPointPtr aCenter(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
    GeomDirPtr aDirection(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));
    aTorus = GeomTorusPtr(new GeomAPI_Torus(aCenter, aDirection, aMajorRadius, aMinorRadius));
  }
  return aTorus;
}

GeomPointPtr GeomAPI_Face::middlePoint() const
{
  GeomPointPtr anInnerPoint;

  const TopoDS_Face& aFace = impl<TopoDS_Face>();
  if (aFace.IsNull())
    return anInnerPoint;

  BRepGProp_Face aProp(aFace);
  double aUMin, aUMax, aVMin, aVMax;
  aProp.Bounds(aUMin, aUMax, aVMin, aVMax);

  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if (aSurf.IsNull())
    return anInnerPoint;

  gp_Pnt aPnt = aSurf->Value((aUMin + aUMax) * 0.5, (aVMin + aVMax) * 0.5);
  anInnerPoint = GeomPointPtr(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
  return anInnerPoint;
}
