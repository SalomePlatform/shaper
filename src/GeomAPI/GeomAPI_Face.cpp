// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
#include "GeomAPI_Curve.h"
#include "GeomAPI_Cylinder.h"
#include "GeomAPI_Cone.h"
#include "GeomAPI_Torus.h"

#include <Bnd_Box2d.hxx>
#include <BndLib_Add2dCurve.hxx>
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
#include <Geom_OffsetSurface.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>
#include <Geom_SurfaceOfLinearExtrusion.hxx>
#include <Geom_SurfaceOfRevolution.hxx>
#include <Geom_SweptSurface.hxx>
#include <Geom_ToroidalSurface.hxx>
#include <GeomAdaptor_HSurface.hxx>
#include <GeomAPI_ExtremaCurveCurve.hxx>
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

static void optimalBounds(const TopoDS_Face& theFace, double& theUMin, double& theUMax,
                                                      double& theVMin, double& theVMax);


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
  if (anOwnSurf == anOtherSurf)
    return true;

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
    if (anOwnCyl && anOtherCyl)
      return anOwnCyl->isCoincident(anOtherCyl);

    // compare two swept surfaces of the same type
    if ((anOwnSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion)) &&
         anOtherSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfLinearExtrusion))) ||
        (anOwnSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)) &&
         anOtherSurf->IsKind(STANDARD_TYPE(Geom_SurfaceOfRevolution)))) {
      Handle(Geom_SweptSurface) anOwnSwept = Handle(Geom_SweptSurface)::DownCast(anOwnSurf);
      Handle(Geom_SweptSurface) anOtherSwept = Handle(Geom_SweptSurface)::DownCast(anOtherSurf);

      const gp_Dir& anOwnDir = anOwnSwept->Direction();
      const gp_Dir& anOtherDir = anOtherSwept->Direction();

      if (anOwnDir.IsParallel(anOtherDir, Precision::Angular())) {
        Handle(Geom_Curve) anOwnCurve = anOwnSwept->BasisCurve();
        Handle(Geom_Curve) anOtherCurve = anOtherSwept->BasisCurve();
        GeomAPI_ExtremaCurveCurve anExtrema(anOwnCurve, anOtherCurve);
        return anExtrema.Extrema().IsParallel() &&
               anExtrema.TotalLowerDistance() < Precision::Confusion();
      }
    }
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
  GeomLib_IsPlanarSurface isPlanarSurf(aSurf);
  gp_Pln aPln;
  bool isPlanar = false;
  if (isPlanarSurf.IsPlanar()) {
    aPln = isPlanarSurf.Plan();
    isPlanar = true;
  }
  else if (aSurf->IsKind(STANDARD_TYPE(Geom_OffsetSurface))) {
    Handle(Geom_OffsetSurface) anOffsetSurf = Handle(Geom_OffsetSurface)::DownCast(aSurf);
    Handle(Geom_Surface) aBasisSurf = anOffsetSurf->BasisSurface();
    if (aBasisSurf->IsKind(STANDARD_TYPE(Geom_Plane))) {
      aPln = Handle(Geom_Plane)::DownCast(aBasisSurf)->Pln();
      gp_Vec aTranslation(aPln.Axis().Direction().XYZ() * anOffsetSurf->Offset());
      aPln.Translate(aTranslation);
      isPlanar = true;
    }
  }

  if (isPlanar) {
    double aA, aB, aC, aD;
    aPln.Coefficients(aA, aB, aC, aD);
    if (aFace.Orientation() == TopAbs_REVERSED) {
      aA = -aA;
      aB = -aB;
      aC = -aC;
      aD = -aD;
    }
    aResult = std::shared_ptr<GeomAPI_Pln>(new GeomAPI_Pln(aA, aB, aC, aD));
  }
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

  double aUMin, aUMax, aVMin, aVMax;
  optimalBounds(aFace, aUMin, aUMax, aVMin, aVMax);

  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if (aSurf.IsNull())
    return anInnerPoint;

  gp_Pnt aPnt = aSurf->Value((aUMin + aUMax) * 0.5, (aVMin + aVMax) * 0.5);
  anInnerPoint = GeomPointPtr(new GeomAPI_Pnt(aPnt.X(), aPnt.Y(), aPnt.Z()));
  return anInnerPoint;
}


// ==================     Auxiliary functions     ========================

void optimalBounds(const TopoDS_Face& theFace, const TopoDS_Edge& theEdge, Bnd_Box2d& theBndBox)
{
  Standard_Real aFirst, aLast;
  const Handle(Geom2d_Curve) aC2D = BRep_Tool::CurveOnSurface(theEdge, theFace, aFirst, aLast);
  if (aC2D.IsNull())
    return;

  Standard_Real aXmin = 0.0, aYmin = 0.0, aXmax = 0.0, aYmax = 0.0;
  Standard_Real aUmin, aUmax, aVmin, aVmax;
  Bnd_Box2d aBoxC, aBoxS;
  BndLib_Add2dCurve::AddOptimal(aC2D, aFirst, aLast, 0., aBoxC);
  if (aBoxC.IsVoid())
    return;

  aBoxC.Get(aXmin, aYmin, aXmax, aYmax);

  TopLoc_Location aLoc;
  Handle(Geom_Surface) aS = BRep_Tool::Surface(theFace, aLoc);
  aS->Bounds(aUmin, aUmax, aVmin, aVmax);

  if (aS->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface))
  {
    const Handle(Geom_RectangularTrimmedSurface) aSt =
        Handle(Geom_RectangularTrimmedSurface)::DownCast(aS);
    aS = aSt->BasisSurface();
  }

  //
  if (!aS->IsUPeriodic())
  {
    Standard_Boolean isUPeriodic = Standard_False;

    // Additional verification for U-periodicity for B-spline surfaces.
    // 1. Verify that the surface is U-closed (if such flag is false). Verification uses 2 points.
    // 2. Verify periodicity of surface inside UV-bounds of the edge. It uses 3 or 6 points.
    if (aS->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface) &&
      (aXmin < aUmin || aXmax > aUmax))
    {
      Standard_Real aTol2 = 100 * Precision::SquareConfusion();
      isUPeriodic = Standard_True;
      gp_Pnt P1, P2;
      // 1. Verify that the surface is U-closed
      if (!aS->IsUClosed())
      {
        Standard_Real aVStep = aVmax - aVmin;
        for (Standard_Real aV = aVmin; aV <= aVmax; aV += aVStep)
        {
          P1 = aS->Value(aUmin, aV);
          P2 = aS->Value(aUmax, aV);
          if (P1.SquareDistance(P2) > aTol2)
          {
            isUPeriodic = Standard_False;
            break;
          }
        }
      }
      // 2. Verify periodicity of surface inside UV-bounds of the edge
      if (isUPeriodic) // the flag still not changed
      {
        Standard_Real aV = (aVmin + aVmax) * 0.5;
        Standard_Real aU[6]; // values of U lying out of surface boundaries
        Standard_Real aUpp[6]; // corresponding U-values plus/minus period
        Standard_Integer aNbPnt = 0;
        if (aXmin < aUmin)
        {
          aU[0] = aXmin;
          aU[1] = (aXmin + aUmin) * 0.5;
          aU[2] = aUmin;
          aUpp[0] = aU[0] + aUmax - aUmin;
          aUpp[1] = aU[1] + aUmax - aUmin;
          aUpp[2] = aU[2] + aUmax - aUmin;
          aNbPnt += 3;
        }
        if (aXmax > aUmax)
        {
          aU[aNbPnt] = aUmax;
          aU[aNbPnt + 1] = (aXmax + aUmax) * 0.5;
          aU[aNbPnt + 2] = aXmax;
          aUpp[aNbPnt] = aU[aNbPnt] - aUmax + aUmin;
          aUpp[aNbPnt + 1] = aU[aNbPnt + 1] - aUmax + aUmin;
          aUpp[aNbPnt + 2] = aU[aNbPnt + 2] - aUmax + aUmin;
          aNbPnt += 3;
        }
        for (Standard_Integer anInd = 0; anInd < aNbPnt; anInd++)
        {
          P1 = aS->Value(aU[anInd], aV);
          P2 = aS->Value(aUpp[anInd], aV);
          if (P1.SquareDistance(P2) > aTol2)
          {
            isUPeriodic = Standard_False;
            break;
          }
        }
      }
    }

    if (!isUPeriodic)
    {
      if ((aXmin < aUmin) && (aUmin < aXmax))
      {
        aXmin = aUmin;
      }
      if ((aXmin < aUmax) && (aUmax < aXmax))
      {
        aXmax = aUmax;
      }
    }
  }

  if (!aS->IsVPeriodic())
  {
    Standard_Boolean isVPeriodic = Standard_False;

    // Additional verification for V-periodicity for B-spline surfaces.
    // 1. Verify that the surface is V-closed (if such flag is false). Verification uses 2 points.
    // 2. Verify periodicity of surface inside UV-bounds of the edge. It uses 3 or 6 points.
    if (aS->DynamicType() == STANDARD_TYPE(Geom_BSplineSurface) &&
      (aYmin < aVmin || aYmax > aVmax))
    {
      Standard_Real aTol2 = 100 * Precision::SquareConfusion();
      isVPeriodic = Standard_True;
      gp_Pnt P1, P2;
      // 1. Verify that the surface is V-closed
      if (!aS->IsVClosed())
      {
        Standard_Real aUStep = aUmax - aUmin;
        for (Standard_Real aU = aUmin; aU <= aUmax; aU += aUStep)
        {
          P1 = aS->Value(aU, aVmin);
          P2 = aS->Value(aU, aVmax);
          if (P1.SquareDistance(P2) > aTol2)
          {
            isVPeriodic = Standard_False;
            break;
          }
        }
      }
      // 2. Verify periodicity of surface inside UV-bounds of the edge
      if (isVPeriodic) // the flag still not changed
      {
        Standard_Real aU = (aUmin + aUmax) * 0.5;
        Standard_Real aV[6]; // values of V lying out of surface boundaries
        Standard_Real aVpp[6]; // corresponding V-values plus/minus period
        Standard_Integer aNbPnt = 0;
        if (aYmin < aVmin)
        {
          aV[0] = aYmin;
          aV[1] = (aYmin + aVmin) * 0.5;
          aV[2] = aVmin;
          aVpp[0] = aV[0] + aVmax - aVmin;
          aVpp[1] = aV[1] + aVmax - aVmin;
          aVpp[2] = aV[2] + aVmax - aVmin;
          aNbPnt += 3;
        }
        if (aYmax > aVmax)
        {
          aV[aNbPnt] = aVmax;
          aV[aNbPnt + 1] = (aYmax + aVmax) * 0.5;
          aV[aNbPnt + 2] = aYmax;
          aVpp[aNbPnt] = aV[aNbPnt] - aVmax + aVmin;
          aVpp[aNbPnt + 1] = aV[aNbPnt + 1] - aVmax + aVmin;
          aVpp[aNbPnt + 2] = aV[aNbPnt + 2] - aVmax + aVmin;
          aNbPnt += 3;
        }
        for (Standard_Integer anInd = 0; anInd < aNbPnt; anInd++)
        {
          P1 = aS->Value(aU, aV[anInd]);
          P2 = aS->Value(aU, aVpp[anInd]);
          if (P1.SquareDistance(P2) > aTol2)
          {
            isVPeriodic = Standard_False;
            break;
          }
        }
      }
    }

    if (!isVPeriodic)
    {
      if ((aYmin < aVmin) && (aVmin < aYmax))
      {
        aYmin = aVmin;
      }
      if ((aYmin < aVmax) && (aVmax < aYmax))
      {
        aYmax = aVmax;
      }
    }
  }

  aBoxS.Update(aXmin, aYmin, aXmax, aYmax);

  theBndBox.Add(aBoxS);
}

void optimalBounds(const TopoDS_Face& theFace, double& theUMin, double& theUMax,
                                               double& theVMin, double& theVMax)
{
  Bnd_Box2d aBB;

  for (TopExp_Explorer anExp(theFace, TopAbs_EDGE); anExp.More(); anExp.Next())
    optimalBounds(theFace, TopoDS::Edge(anExp.Current()), aBB);

  aBB.Get(theUMin, theVMin, theUMax, theVMax);
}
