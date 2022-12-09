// Copyright (C) 2007-2022  CEA/DEN, EDF R&D, OPEN CASCADE
//
// Copyright (C) 2003-2007  OPEN CASCADE, EADS/CCR, LIP6, CEA/DEN,
// CEDRAT, EDF R&D, LEG, PRINCIPIA R&D, BUREAU VERITAS
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
//  File    : GEOMAlgo_AlgoTools.cxx
//  Created :
//  Author  : Peter KURNEV

#include <GEOMAlgo_AlgoTools.hxx>

#include <Basics_OCCTVersion.hxx>

#include <gp_Pnt.hxx>
#include <gp_Pnt2d.hxx>
#include <gp_Dir2d.hxx>
#include <gp_Ax2.hxx>
#include <Bnd_Box.hxx>

#include <BRepAdaptor_Curve2d.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_TrimmedCurve.hxx>
#include <Geom2d_Line.hxx>
#include <Geom2d_TrimmedCurve.hxx>

#include <Geom2dHatch_Intersector.hxx>
#include <Geom2dHatch_Hatcher.hxx>

#include <Geom2dAdaptor_Curve.hxx>
#include <HatchGen_Domain.hxx>

#include <GeomLib.hxx>
#include <Geom_Curve.hxx>
#include <Geom_Surface.hxx>

#include <GeomAdaptor_Surface.hxx>

#include <GeomAPI_ProjectPointOnSurf.hxx>
#include <GeomAPI_ProjectPointOnCurve.hxx>

#include <GProp_GProps.hxx>

#include <Poly_Triangulation.hxx>

#include <TopAbs_Orientation.hxx>

#include <TopLoc_Location.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Compound.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepLib_MakeVertex.hxx>

#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepBndLib.hxx>
#include <BRepMesh_IncrementalMesh.hxx>
#include <BRepGProp.hxx>

#include <IntTools_Tools.hxx>

#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>
#include <TColgp_SequenceOfPnt2d.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <IntTools_Tools.hxx>

#include <BOPTools_AlgoTools3D.hxx>
#include <BOPTools_AlgoTools2D.hxx>

#include <ShapeUpgrade_ShapeDivideArea.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>

#include <GEOMAlgo_PassKeyShape.hxx>

#include <algorithm>

static
  void GetCount(const TopoDS_Shape& aS,
                Standard_Integer& iCnt);
static
  void CopySource(const TopoDS_Shape& aS,
    TopTools_IndexedDataMapOfShapeShape& aMapSS,
    TopoDS_Shape& aSC);

static Standard_Boolean comp(const std::pair<TopoDS_Shape, Standard_Real>& theA,
                             const std::pair<TopoDS_Shape, Standard_Real>& theB);

static Standard_Boolean IsUiso (const TopoDS_Edge& theEdge,
                                const TopoDS_Face& theFace);

static void CorrectShell (const TopoDS_Shape& theShell,
                          const TopoDS_Face&  theFace);

static gp_Pnt GetMidPnt2d(const TopoDS_Face&     theFace,
                          const Standard_Boolean theIsNaturalRestrictions);

static void ModifyFacesForGlobalResult(const TopoDS_Face&     theInputFace,
                                       const Standard_Real    theAverageArea,
                                       const Standard_Boolean theIsToAddFaces,
                                       Standard_Integer&      theNbExtremalFaces,
                                       TopTools_MapOfShape&   theExtremalFaces,
                                       const std::vector<std::pair<TopoDS_Shape, Standard_Real>> theFacesAndAreas,
                                       const TopTools_DataMapOfShapeReal& theFaceAreaMap,
                                       const TopTools_IndexedDataMapOfShapeListOfShape& theEFmap,
                                       TopoDS_Shape&          theRes,
                                       TopoDS_Shape&          theGlobalRes,
                                       TopTools_MapOfShape&   theRemovedFaces);

//=======================================================================
//function : CopyShape
//purpose  :
//=======================================================================
void GEOMAlgo_AlgoTools::CopyShape(const TopoDS_Shape& aS,
       TopoDS_Shape& aSC)
{
  TopTools_IndexedDataMapOfShapeShape aMapSS;
  //
  CopySource(aS, aMapSS, aSC);
}
//=======================================================================
//function : CopyShape
//purpose  :
//=======================================================================
void GEOMAlgo_AlgoTools::CopyShape(const TopoDS_Shape& aS,
       TopoDS_Shape& aSC,
       TopTools_IndexedDataMapOfShapeShape& aMapSS)
{
  CopySource(aS, aMapSS, aSC);
}
//=======================================================================
//function : CopySource
//purpose  :
//=======================================================================
void CopySource(const TopoDS_Shape& aS,
                TopTools_IndexedDataMapOfShapeShape& aMapSS,
                TopoDS_Shape& aSC)
{
  Standard_Boolean bFree;
  TopAbs_ShapeEnum aT;
  TopoDS_Iterator aIt;
  TopoDS_Shape aSF;
  BRep_Builder BB;
  //
  aT=aS.ShapeType();
  //
  if (aMapSS.Contains(aS)) {
    aSC=aMapSS.ChangeFromKey(aS);
    aSC.Orientation(aS.Orientation());
    return;
  }
  else {
    aSC=aS.EmptyCopied();
    aMapSS.Add(aS, aSC);
  }
  //
  bFree=aSC.Free();
  aSC.Free(Standard_True);
  aSF=aS;
  if (aT==TopAbs_EDGE){
    TopAbs_Orientation aOr;
    //
    aOr=aS.Orientation();
    if(aOr==TopAbs_INTERNAL) {
      aSF.Orientation(TopAbs_FORWARD);
    }
  }
  aIt.Initialize(aSF);
  for (; aIt.More();  aIt.Next()) {
    TopoDS_Shape aSCx;
    //
    const TopoDS_Shape& aSx=aIt.Value();
    //
    CopySource (aSx, aMapSS, aSCx);
    //
    aSCx.Orientation(aSx.Orientation());
    BB.Add(aSC, aSCx);
  }
  aSC.Free(bFree);
}
//=======================================================================
//function : FaceNormal
//purpose  : 
//=======================================================================
void GEOMAlgo_AlgoTools::FaceNormal (const TopoDS_Face& aF,
         const Standard_Real U,
         const Standard_Real V,
         gp_Vec& aN)
{
  gp_Pnt aPnt ;
  gp_Vec aD1U, aD1V;
  Handle(Geom_Surface) aSurface;

  aSurface=BRep_Tool::Surface(aF);
  aSurface->D1 (U, V, aPnt, aD1U, aD1V);
  aN=aD1U.Crossed(aD1V);
  aN.Normalize();  
  if (aF.Orientation() == TopAbs_REVERSED){
    aN.Reverse();
  }
  return;
}
//=======================================================================
//function : BuildPCurveForEdgeOnFace
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_AlgoTools::BuildPCurveForEdgeOnFace
  (const TopoDS_Edge& aEold,
   const TopoDS_Edge& aEnew,
   const TopoDS_Face& aF,
   const Handle(IntTools_Context)& aCtx)
{
  Standard_Boolean bIsClosed, bUClosed, bHasOld;
  Standard_Integer iRet, aNbPoints;
  Standard_Real aTS, aTS1, aTS2, aT, aT1, aT2, aScPr, aTol;
  Standard_Real aU, aV, aUS1, aVS1, aUS2, aVS2;
  gp_Pnt aP;
  gp_Pnt2d aP2DS1, aP2DS2, aP2D;
  gp_Vec2d aV2DS1, aV2DS2;
  Handle(Geom2d_Curve) aC2D, aC2DS1, aC2DS2;
  Handle(Geom_Surface) aS;
  TopoDS_Edge aES;
  //
  iRet=0;
  //
  bHasOld=BOPTools_AlgoTools2D::HasCurveOnSurface(aEnew, aF, aC2D, aT1, aT2, aTol);
  if (bHasOld) {
    return iRet;
  }
  //
  // Try to copy PCurve from old edge to the new one.
  iRet = BOPTools_AlgoTools2D::AttachExistingPCurve(aEold, aEnew, aF, aCtx);

  if (iRet) {
    // Do PCurve using projection algorithm.
    iRet = 0;
  } else {
    // The PCurve is attached successfully.
    return iRet;
  }
  //
  BOPTools_AlgoTools2D::BuildPCurveForEdgeOnFace(aEnew, aF);
  aC2D=BRep_Tool::CurveOnSurface(aEnew, aF, aT1, aT2);
  if (aC2D.IsNull()){
    iRet=1;
    return iRet;
  }
  //
  bIsClosed=BRep_Tool::IsClosed(aEold, aF);
  if (!bIsClosed) {
    return iRet;
  }
  //
  aTol=1.e-7;
  //
  // 1. bUClosed - direction of closeness
  //
  aES=aEold;
  aES.Orientation(TopAbs_FORWARD);
  aC2DS1=BRep_Tool::CurveOnSurface(aES, aF, aTS1, aTS2);
  //
  aES.Orientation(TopAbs_REVERSED);
  aC2DS2=BRep_Tool::CurveOnSurface(aES, aF, aTS1, aTS2);
  //
  aTS=IntTools_Tools::IntermediatePoint(aTS1, aTS2);
  //
  aC2DS1->D1(aTS, aP2DS1, aV2DS1);
  aC2DS2->D1(aTS, aP2DS2, aV2DS2);
  //
  gp_Vec2d aV2DS12(aP2DS1, aP2DS2);
  gp_Dir2d aD2DS12(aV2DS12);
  const gp_Dir2d& aD2DX=gp::DX2d();
  //
  aScPr=aD2DS12*aD2DX;
  bUClosed=Standard_True;
  if (fabs(aScPr) < aTol) {
    bUClosed=!bUClosed;
  }
  //
  // 2. aP2D - point on curve aC2D, that corresponds to aP2DS1
  aP2DS1.Coord(aUS1, aVS1);
  aP2DS2.Coord(aUS2, aVS2);
  //
  aS=BRep_Tool::Surface(aF);
  aS->D0(aUS1, aVS1, aP);
  //
  GeomAPI_ProjectPointOnCurve& aProjPC=aCtx->ProjPC(aEnew);
  //
  aProjPC.Perform(aP);
  aNbPoints=aProjPC.NbPoints();
  if (!aNbPoints) {
    iRet=2;
    return iRet;
  }
  //
  aT=aProjPC.LowerDistanceParameter();

  //
  // 3. Build the second 2D curve
  Standard_Boolean bRevOrder;
  gp_Vec2d aV2DT, aV2D;
  Handle(Geom2d_Curve) aC2Dnew;
  Handle(Geom2d_TrimmedCurve) aC2DTnew;
  BRep_Builder aBB;
  //
  aC2D->D1(aT, aP2D, aV2D);
  aP2D.Coord(aU, aV);
  //
  aC2Dnew=Handle(Geom2d_Curve)::DownCast(aC2D->Copy());
  aC2DTnew = new Geom2d_TrimmedCurve(aC2Dnew, aT1, aT2);
  //
  aV2DT=aV2DS12;
  if (!bUClosed) {    // V Closed
    if (fabs(aV-aVS2)<aTol) {
      aV2DT.Reverse();
    }
  }
  else {   // U Closed
    if (fabs(aU-aUS2)<aTol) {
      aV2DT.Reverse();
    }
  }
  //
  aC2DTnew->Translate(aV2DT);
  //
  // 4 Order the 2D curves
  bRevOrder=Standard_False;
  aScPr=aV2D*aV2DS1;
  if(aScPr<0.) {
    bRevOrder=!bRevOrder;
  }
  //
  // 5. Update the edge
  aTol=BRep_Tool::Tolerance(aEnew);
  if (!bRevOrder) {
    aBB.UpdateEdge(aEnew, aC2D, aC2DTnew, aF, aTol);
  }
  else {
    aBB.UpdateEdge(aEnew, aC2DTnew, aC2D , aF, aTol);
  }
  //
  return iRet;
}
//////////////////////////////////////////////////////////////////////////
//=======================================================================
// function: MakeContainer
// purpose:
//=======================================================================
void GEOMAlgo_AlgoTools::MakeContainer(const TopAbs_ShapeEnum theType,
           TopoDS_Shape& theC)
{
  BRep_Builder aBB;
  //
  switch(theType) {
    case TopAbs_COMPOUND:{
      TopoDS_Compound aC;
      aBB.MakeCompound(aC);
      theC=aC;
    }
      break;
      //
    case TopAbs_COMPSOLID:{
      TopoDS_CompSolid aCS;
      aBB.MakeCompSolid(aCS);
      theC=aCS;
    }
      break;
      //
    case TopAbs_SOLID:{
      TopoDS_Solid aSolid;
      aBB.MakeSolid(aSolid);
      theC=aSolid;
    }
      break;
      //
      //
    case TopAbs_SHELL:{
      TopoDS_Shell aShell;
      aBB.MakeShell(aShell);
      theC=aShell;
    }
      break;
      //
    case TopAbs_WIRE: {
      TopoDS_Wire aWire;
      aBB.MakeWire(aWire);
      theC=aWire;
    }
      break;
      //
    default:
      break;
  }
}
//=======================================================================
//function : IsUPeriodic
//purpose  :
//=======================================================================
Standard_Boolean GEOMAlgo_AlgoTools::IsUPeriodic(const  Handle(Geom_Surface) &aS)
{
  Standard_Boolean bRet;
  GeomAbs_SurfaceType aType;
  GeomAdaptor_Surface aGAS;
  //
  aGAS.Load(aS);
  aType=aGAS.GetType();
  bRet=(aType==GeomAbs_Cylinder||
        aType==GeomAbs_Cone ||
        aType==GeomAbs_Sphere);
  //
  return bRet;
}

//=======================================================================
//function : RefinePCurveForEdgeOnFace
//purpose  :
//=======================================================================
void GEOMAlgo_AlgoTools::RefinePCurveForEdgeOnFace(const TopoDS_Edge& aE,
         const TopoDS_Face& aF,
         const Standard_Real aUMin,
         const Standard_Real aUMax)
{
  Standard_Real aT1, aT2, aTx, aUx, aTol;
  gp_Pnt2d aP2D;
  Handle(Geom_Surface) aS;
  Handle(Geom2d_Curve) aC2D;
  BRep_Builder aBB;
  //
  aC2D=BRep_Tool::CurveOnSurface(aE, aF, aT1, aT2);
  if (!aC2D.IsNull()) {
    if (BRep_Tool::IsClosed(aE, aF)) {
      return;
    }
    aTx=IntTools_Tools::IntermediatePoint(aT1, aT2);
    aC2D->D0(aTx, aP2D);
    aUx=aP2D.X();
    if (aUx < aUMin || aUx > aUMax) {
      // need to rebuild
      Handle(Geom2d_Curve) aC2Dx;
      //
      aTol=BRep_Tool::Tolerance(aE);
      aBB.UpdateEdge(aE, aC2Dx, aF, aTol);
    }
  }
}
//=======================================================================
//function :IsSplitToReverse
//purpose  : 
//=======================================================================
Standard_Boolean GEOMAlgo_AlgoTools::IsSplitToReverse
  (const TopoDS_Edge& aEF1,
   const TopoDS_Edge& aEF2,
   const Handle(IntTools_Context)& aContext)
{
  Standard_Boolean aFlag;
  Standard_Real aT1, aT2, aScPr, a, b;
  gp_Vec aV1, aV2;
  gp_Pnt aP;
  
  
  Handle(Geom_Curve)aC1=BRep_Tool::Curve(aEF1, a, b);
  aT1=IntTools_Tools::IntermediatePoint(a, b);
  aC1->D0(aT1, aP);
  aFlag=BOPTools_AlgoTools2D::EdgeTangent(aEF1, aT1, aV1);

  if(!aFlag) {
    return Standard_False;
  }

  gp_Dir aDT1(aV1);
  //
  aFlag=aContext->ProjectPointOnEdge(aP, aEF2, aT2);
  if(!aFlag) {
    return Standard_False;
  }
  //
  aFlag=BOPTools_AlgoTools2D::EdgeTangent(aEF2, aT2, aV2);
  if(!aFlag) {
    return Standard_False;
  }

  gp_Dir aDT2(aV2);

  aScPr=aDT1*aDT2;

  return (aScPr<0.);
}


//=======================================================================
//function : ProjectPointOnShape
//purpose  :
//=======================================================================
Standard_Boolean GEOMAlgo_AlgoTools::ProjectPointOnShape
  (const gp_Pnt& aP1,
   const TopoDS_Shape& aS,
   gp_Pnt& aP2,
   const Handle(IntTools_Context)& aCtx)
{
  Standard_Boolean bIsDone = Standard_False;
  Standard_Real aT2;
  TopAbs_ShapeEnum aType;
  //
  aType = aS.ShapeType();
  switch (aType)
    {
    case TopAbs_EDGE:
      {
        const TopoDS_Edge& aE2 = TopoDS::Edge(aS);
        //
        if (BRep_Tool::Degenerated(aE2)) { // jfa
          return Standard_True;
        }
        else {
          Standard_Real f, l;
          Handle(Geom_Curve) aC3D = BRep_Tool::Curve (aE2, f, l);
          if (aC3D.IsNull()) {
            return Standard_True;
          }
          bIsDone = aCtx->ProjectPointOnEdge(aP1, aE2, aT2);
        }
        if (!bIsDone) {
          return bIsDone;
        }
        //
        GEOMAlgo_AlgoTools::PointOnEdge(aE2, aT2, aP2);
      }
      break;
      //
    case TopAbs_FACE:
      {
        const TopoDS_Face& aF2 = TopoDS::Face(aS);
        GeomAPI_ProjectPointOnSurf& aProj = aCtx->ProjPS(aF2);
        //
        aProj.Perform(aP1);
        bIsDone = aProj.IsDone();
        if (!bIsDone) {
          return bIsDone;
        }
        //
        aP2 = aProj.NearestPoint();
      }
      break;
      //
    default:
      break; // Err
    }
  return bIsDone;
}

//=======================================================================
//function : PointOnEdge
//purpose  :
//=======================================================================
void GEOMAlgo_AlgoTools::PointOnEdge(const TopoDS_Edge& aE,
         gp_Pnt& aP3D)
{
  Standard_Real aTx, aT1, aT2;
  //
  BRep_Tool::Curve(aE, aT1, aT2);
  aTx=IntTools_Tools::IntermediatePoint(aT1, aT2);
  GEOMAlgo_AlgoTools::PointOnEdge(aE, aTx, aP3D);
}
//=======================================================================
//function : PointOnEdge
//purpose  :
//=======================================================================
void GEOMAlgo_AlgoTools::PointOnEdge(const TopoDS_Edge& aE,
         const Standard_Real aT,
         gp_Pnt& aP3D)
{
  Standard_Real aT1, aT2;
  Handle(Geom_Curve) aC3D;
  //
  aC3D=BRep_Tool::Curve(aE, aT1, aT2);
  aC3D->D0(aT, aP3D);
}
//=======================================================================
//function : PointOnFace
//purpose  :
//=======================================================================
void GEOMAlgo_AlgoTools::PointOnFace(const TopoDS_Face& aF,
         const Standard_Real aU,
         const Standard_Real aV,
         gp_Pnt& aP3D)
{
  Handle(Geom_Surface) aS;
  //
  aS=BRep_Tool::Surface(aF);
  aS->D0(aU, aV, aP3D);
}
//=======================================================================
//function : PointOnFace
//purpose  :
//=======================================================================
void GEOMAlgo_AlgoTools::PointOnFace(const TopoDS_Face& aF,
         gp_Pnt& aP3D)
{
  Standard_Real aU, aV, aUMin, aUMax, aVMin, aVMax;
  //
  BRepTools::UVBounds(aF, aUMin, aUMax, aVMin, aVMax);
  //
  aU=IntTools_Tools::IntermediatePoint(aUMin, aUMax);
  aV=IntTools_Tools::IntermediatePoint(aVMin, aVMax);
  //
  GEOMAlgo_AlgoTools::PointOnFace(aF, aU, aV, aP3D);
}
//=======================================================================
//function : PointOnShape
//purpose  :
//=======================================================================
void GEOMAlgo_AlgoTools::PointOnShape(const TopoDS_Shape& aS,
          gp_Pnt& aP3D)
{
  TopAbs_ShapeEnum aType;
  //
  aP3D.SetCoord(99.,99.,99.);
  aType=aS.ShapeType();
  switch(aType) {
    case TopAbs_EDGE: {
      const TopoDS_Edge& aE=TopoDS::Edge(aS);
      GEOMAlgo_AlgoTools::PointOnEdge(aE, aP3D);
      }
      break;
      //
    case TopAbs_FACE: {
      const TopoDS_Face& aF=TopoDS::Face(aS);
      GEOMAlgo_AlgoTools::PointOnFace(aF, aP3D);
      }
      break;
      //
    default:
      break; // Err
  }
}
//=======================================================================
//function : FindSDShapes
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_AlgoTools::FindSDShapes
  (const TopoDS_Shape& aE1,
   const TopTools_ListOfShape& aLE,
   const Standard_Real aTol,
   TopTools_ListOfShape& aLESD,
   const Handle(IntTools_Context)& aCtx)
{
  Standard_Boolean bIsDone;
  Standard_Real aTol2, aD2;
  gp_Pnt aP1, aP2;
  TopTools_ListIteratorOfListOfShape aIt;
  //
  aTol2=aTol*aTol;
  GEOMAlgo_AlgoTools::PointOnShape(aE1, aP1);
  //
  aIt.Initialize(aLE);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aE2=aIt.Value();
    if (aE2.IsSame(aE1)) {
       aLESD.Append(aE2);
    }
    else {
      bIsDone=GEOMAlgo_AlgoTools::ProjectPointOnShape(aP1, aE2, aP2, aCtx);
      if (!bIsDone) {
        //return 1;
        continue; // jfa BUG 20361
      }
      aD2=aP1.SquareDistance(aP2);
      if(aD2<aTol2) {
        aLESD.Append(aE2);
      }
    }
  }
  return 0;
}

//=======================================================================
//function : FindSDShapes
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_AlgoTools::FindSDShapes
  (const TopTools_ListOfShape& aLE,
   const Standard_Real aTol,
   TopTools_IndexedDataMapOfShapeListOfShape& aMEE,
   const Handle(IntTools_Context)& aCtx)
{
  Standard_Integer aNbE, aNbEProcessed, aNbESD, iErr;
  TopTools_ListOfShape aLESD;
  TopTools_ListIteratorOfListOfShape aIt, aIt1;
  TopTools_IndexedMapOfShape aMProcessed;
  TopAbs_ShapeEnum aType;
  //
  aNbE=aLE.Extent();
  if (!aNbE) {
    return 3; // Err
  }
  if (aNbE==1) {
    return 0; // Nothing to do
  }
  //
  for(;;) {
    aNbEProcessed=aMProcessed.Extent();
    if (aNbEProcessed==aNbE) {
      break;
    }
    //
    aIt.Initialize(aLE);
    for (; aIt.More(); aIt.Next()) {
      const TopoDS_Shape& aS=aIt.Value();
      //
      if (aMProcessed.Contains(aS)) {
        continue;
      }
      //
      aType=aS.ShapeType();
      if (aType==TopAbs_EDGE) {
        const TopoDS_Edge& aE=TopoDS::Edge(aS);
        if (BRep_Tool::Degenerated(aE)) {
          aMProcessed.Add(aE);
          continue;
        }
      }
      //
      aLESD.Clear();
      iErr=GEOMAlgo_AlgoTools::FindSDShapes(aS, aLE, aTol, aLESD, aCtx);
      if (iErr) {
        return 2; // Err
      }
      //
      aNbESD=aLESD.Extent();
      if (!aNbESD) {
        return 1; // Err
      }
      //
      aMEE.Add(aS, aLESD);
      //
      aIt1.Initialize(aLESD);
      for (; aIt1.More(); aIt1.Next()) {
        const TopoDS_Shape& aE1=aIt1.Value();
        aMProcessed.Add(aE1);
      }
    }
  }
  return 0;
}
//=======================================================================
//function : RefineSDShapes
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_AlgoTools::RefineSDShapes
  (GEOMAlgo_IndexedDataMapOfPassKeyShapeListOfShape& aMPKLE,
   const Standard_Real aTol,
   const Handle(IntTools_Context)& aCtx)
{
  Standard_Integer i, aNbE, iErr, j, aNbEE, aNbToAdd;
  TopTools_IndexedDataMapOfShapeListOfShape aMEE, aMSDE, aMEToAdd;
  //
  iErr=1;
  //
  aNbE=aMPKLE.Extent();
  for (i=1; i<=aNbE; ++i) {
    TopTools_ListOfShape& aLSDE=aMPKLE.ChangeFromIndex(i);
    //
    aMEE.Clear();
    iErr=GEOMAlgo_AlgoTools::FindSDShapes(aLSDE, aTol, aMEE, aCtx);
    if (iErr) {
      return iErr;
    }
    //
    aNbEE=aMEE.Extent();
    if (aNbEE==1) {
      continue;  // nothing to do
    }
    //
    for (j=1; j<=aNbEE; ++j) {
      TopTools_ListOfShape& aLEE=aMEE.ChangeFromIndex(j);
      //
      if (j==1) {
        aLSDE.Clear();
        aLSDE.Append(aLEE);
      }
      else {
        const TopoDS_Shape& aE1=aLEE.First();
        aMEToAdd.Add(aE1, aLEE);
      }
    }
  }
  //
  aNbToAdd=aMEToAdd.Extent();
  if (!aNbToAdd) {
    return aNbToAdd;
  }
  //
  for (i=1; i<=aNbToAdd; ++i) {
    GEOMAlgo_PassKeyShape aPKE1;
    //
    const TopoDS_Shape& aE1=aMEToAdd.FindKey(i);
    const TopTools_ListOfShape& aLE=aMEToAdd(i);
    //
    aPKE1.SetShapes(aE1);
    aMPKLE.Add(aPKE1, aLE);
  }
  //
  return 0;
}
//=======================================================================
//function : BuildTriangulation
//purpose  :
//=======================================================================
Standard_Boolean 
  GEOMAlgo_AlgoTools::BuildTriangulation (const TopoDS_Shape& theShape)
{
  // calculate deflection
  Standard_Real aDeviationCoefficient = 0.001;

  Bnd_Box B;
  BRepBndLib::Add(theShape, B);
  Standard_Real aXmin, aYmin, aZmin, aXmax, aYmax, aZmax;
  B.Get(aXmin, aYmin, aZmin, aXmax, aYmax, aZmax);

  Standard_Real dx = aXmax - aXmin, dy = aYmax - aYmin, dz = aZmax - aZmin;
  Standard_Real aDeflection = Max(Max(dx, dy), dz) * aDeviationCoefficient * 4;
  Standard_Real aHLRAngle = 0.349066;

  // build triangulation
  BRepMesh_IncrementalMesh Inc (theShape, aDeflection, Standard_False, aHLRAngle);

  // check triangulation
  bool isTriangulation = true;

  TopExp_Explorer exp (theShape, TopAbs_FACE);
  if (exp.More())
  {
    TopLoc_Location aTopLoc;
    Handle(Poly_Triangulation) aTRF;
    aTRF = BRep_Tool::Triangulation(TopoDS::Face(exp.Current()), aTopLoc);
    if (aTRF.IsNull()) {
      isTriangulation = false;
    }
  }
  else // no faces, try edges
  {
    TopExp_Explorer expe (theShape, TopAbs_EDGE);
    if (!expe.More()) {
      isTriangulation = false;
    }
    else {
      TopLoc_Location aLoc;
      Handle(Poly_Polygon3D) aPE = BRep_Tool::Polygon3D(TopoDS::Edge(expe.Current()), aLoc);
      if (aPE.IsNull()) {
        isTriangulation = false;
      }
    }
  }
  return isTriangulation;
}

//=======================================================================
//function : IsCompositeShape
//purpose  :
//=======================================================================
Standard_Boolean GEOMAlgo_AlgoTools::IsCompositeShape(const TopoDS_Shape& aS)
{
  Standard_Boolean bRet;
  Standard_Integer iCnt;
  TopoDS_Iterator aIt;
  //
  iCnt=0;
  GetCount(aS, iCnt);
  bRet=(iCnt>1);
  //
  return bRet;
}
//=======================================================================
//function : GetCount
//purpose  :
//=======================================================================
void GetCount(const TopoDS_Shape& aS,
              Standard_Integer& iCnt)
{
  TopoDS_Iterator aIt;
  TopAbs_ShapeEnum aTS;
  //
  aTS=aS.ShapeType();
  //
  if (aTS==TopAbs_SHAPE) {
    return;
  }
  if (aTS!=TopAbs_COMPOUND) {
    ++iCnt;
    return;
  }
  //
  aIt.Initialize(aS);
  for (; aIt.More(); aIt.Next()) {
    const TopoDS_Shape& aSx=aIt.Value();
    GetCount(aSx, iCnt);
  }
}

//=======================================================================
//function : PntInFace
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_AlgoTools::PntInFace(const TopoDS_Face& aF,
                                               gp_Pnt& theP,
                                               gp_Pnt2d& theP2D)
{
  Standard_Boolean bIsDone, bHasFirstPoint, bHasSecondPoint;
  Standard_Integer iErr, aIx, aNbDomains, i;
  Standard_Real aUMin, aUMax, aVMin, aVMax;
  Standard_Real aVx, aUx, aV1, aV2, aU1, aU2, aEpsT;
  Standard_Real aTotArcIntr, aTolTangfIntr, aTolHatch2D, aTolHatch3D;
  gp_Dir2d aD2D (0., 1.);
  gp_Pnt2d aP2D;
  gp_Pnt aPx;
  Handle(Geom2d_Curve) aC2D;
  Handle(Geom2d_TrimmedCurve) aCT2D;
  Handle(Geom2d_Line) aL2D;
  Handle(Geom_Surface) aS;
  TopAbs_Orientation aOrE;
  TopoDS_Face aFF;
  TopExp_Explorer aExp;
  //
  aTolHatch2D=1.e-8;
  aTolHatch3D=1.e-8;
  aTotArcIntr=1.e-10;
  aTolTangfIntr=1.e-10;
  //
  Geom2dHatch_Intersector aIntr(aTotArcIntr, aTolTangfIntr);
  Geom2dHatch_Hatcher aHatcher(aIntr,
          aTolHatch2D, aTolHatch3D,
          Standard_True, Standard_False);
  //
  iErr=0;
  aEpsT=1.e-12;
  //
  aFF=aF;
  aFF.Orientation (TopAbs_FORWARD);
  //
  aS=BRep_Tool::Surface(aFF);
  BRepTools::UVBounds(aFF, aUMin, aUMax, aVMin, aVMax);
  //
  // 1
  aExp.Init (aFF, TopAbs_EDGE);
  for (; aExp.More() ; aExp.Next()) {
    const TopoDS_Edge& aE=*((TopoDS_Edge*)&aExp.Current());
    aOrE=aE.Orientation();
    //
    aC2D=BRep_Tool::CurveOnSurface (aE, aFF, aU1, aU2);
    if (aC2D.IsNull() ) {
      iErr=1;
      return iErr;
    }
    if (fabs(aU1-aU2) < aEpsT) {
      iErr=2;
      return iErr;
    }
    //
    aCT2D=new Geom2d_TrimmedCurve(aC2D, aU1, aU2);
    aHatcher.AddElement(aCT2D, aOrE);
  }// for (; aExp.More() ; aExp.Next()) {
  //
  // 2
  aUx=IntTools_Tools::IntermediatePoint(aUMin, aUMax);
  aP2D.SetCoord(aUx, 0.);
  aL2D=new Geom2d_Line (aP2D, aD2D);
  Geom2dAdaptor_Curve aHCur(aL2D);
  //
  aIx=aHatcher.AddHatching(aHCur) ;
  //
  // 3.
  aHatcher.Trim();
  bIsDone=aHatcher.TrimDone(aIx);
  if (!bIsDone) {
    iErr=3;
    return iErr;
  }
  //
  aHatcher.ComputeDomains(aIx);
  bIsDone=aHatcher.IsDone(aIx);
  if (!bIsDone) {
    iErr=4;
    return iErr;
  }
  //
  // 4.
  aVx=aVMin;
  aNbDomains=aHatcher.NbDomains(aIx);
  if (!aNbDomains) {
    iErr=5;
    return iErr;
  }
  //
  i=1;
  const HatchGen_Domain& aDomain=aHatcher.Domain (aIx, i) ;
  bHasFirstPoint=aDomain.HasFirstPoint();
  if (!bHasFirstPoint) {
    iErr=5;
    return iErr;
  }
  //
  aV1=aDomain.FirstPoint().Parameter();
  //
  bHasSecondPoint=aDomain.HasSecondPoint();
  if (!bHasSecondPoint) {
    iErr=6;
    return iErr;
  }
  //
  aV2=aDomain.SecondPoint().Parameter();
  //
  aVx=IntTools_Tools::IntermediatePoint(aV1, aV2);
  //
  aS->D0(aUx, aVx, aPx);
  //
  theP2D.SetCoord(aUx, aVx);
  theP=aPx;
  //
  return iErr;
}

//=======================================================================
//function : PointCloudInFace
//purpose  :
//=======================================================================
Standard_Integer GEOMAlgo_AlgoTools::PointCloudInFace(const TopoDS_Face& theFace,
                                                      const int          theNbPnts,
                                                      TopoDS_Compound&   theCompound)
{
#if OCC_VERSION_LARGE < 0x07050304
  return -1;
#else
  ShapeUpgrade_ShapeDivideArea tool (theFace);
  tool.SetSplittingByNumber (Standard_True);
  tool.NbParts() = theNbPnts;
  tool.Perform();
  TopoDS_Shape res = tool.Result();

  BRep_Builder aBB;
  TopoDS_Compound aGlobalRes;
  aBB.MakeCompound (aGlobalRes);

  TopTools_IndexedMapOfShape aFaceMap;
  TopExp::MapShapes (res, TopAbs_FACE, aFaceMap);
  Standard_Integer aNbFaces = aFaceMap.Extent();

  TopTools_IndexedDataMapOfShapeListOfShape aEFmap;
  TopExp::MapShapesAndAncestors (res, TopAbs_EDGE, TopAbs_FACE, aEFmap);
  
  TopTools_MapOfShape aBiggestFaces, aSmallestFaces;
  Standard_Boolean aUseTriangulation = Standard_True;
  Standard_Boolean aSkipShared = Standard_False;
  if (aNbFaces != theNbPnts)
  {
    Standard_Real aTotalArea = 0.;
    std::vector<std::pair<TopoDS_Shape, Standard_Real> > aFacesAndAreas (aNbFaces);
    for (Standard_Integer ii = 1; ii <= aNbFaces; ii++)
    {
      GProp_GProps aProps;
      BRepGProp::SurfaceProperties (aFaceMap(ii), aProps, aSkipShared, aUseTriangulation);
      Standard_Real anArea = aProps.Mass();
      aTotalArea += anArea;
      std::pair<TopoDS_Shape, Standard_Real> aFaceWithArea (aFaceMap(ii), anArea);
      aFacesAndAreas[ii-1] = aFaceWithArea;
    }
    std::sort (aFacesAndAreas.begin(), aFacesAndAreas.end(), comp);

    Standard_Real anAverageArea = aTotalArea / theNbPnts;

    TopTools_DataMapOfShapeReal aFaceAreaMap;
    for (Standard_Integer ii = 0; ii < aNbFaces; ii++)
      aFaceAreaMap.Bind (aFacesAndAreas[ii].first, aFacesAndAreas[ii].second);
    
    TopTools_MapOfShape aRemovedFaces;
    
    if (aNbFaces < theNbPnts)
    {
      Standard_Integer aNbMissingFaces = theNbPnts - aNbFaces;
      for (Standard_Integer ii = aNbFaces-1; ii > aNbFaces - aNbMissingFaces - 1; ii--)
        aBiggestFaces.Add (aFacesAndAreas[ii].first);

      ModifyFacesForGlobalResult (theFace, anAverageArea,
                                  Standard_True, //to add faces
                                  aNbMissingFaces, aBiggestFaces,
                                  aFacesAndAreas, aFaceAreaMap, aEFmap,
                                  res, aGlobalRes,
                                  aRemovedFaces);
    }
    else //aNbFaces > theNbPnts
    {
      Standard_Integer aNbExcessFaces = aNbFaces - theNbPnts;
      for (Standard_Integer ii = 0; ii < aNbExcessFaces; ii++)
        aSmallestFaces.Add (aFacesAndAreas[ii].first);

      TopTools_IndexedDataMapOfShapeListOfShape aVFmap;
      TopExp::MapShapesAndAncestors (res, TopAbs_VERTEX, TopAbs_FACE, aVFmap);

      //Remove smallest faces with free boundaries
      for (Standard_Integer ii = 0; ii < aNbExcessFaces; ii++)
      {
        const TopoDS_Face& aFace = TopoDS::Face (aFacesAndAreas[ii].first);
        Standard_Boolean anIsFreeBoundFound = Standard_False;
        TopExp_Explorer anExplo (aFace, TopAbs_EDGE);
        for (; anExplo.More(); anExplo.Next())
        {
          const TopoDS_Edge& anEdge = TopoDS::Edge (anExplo.Current());
          if (!BRep_Tool::Degenerated (anEdge) &&
              aEFmap.FindFromKey(anEdge).Extent() < 2)
          {
            anIsFreeBoundFound = Standard_True;
            break;
          }
        }
        if (anIsFreeBoundFound)
        {
          Standard_Real aMaxArea = 0.;
          for (anExplo.Init(aFace, TopAbs_VERTEX); anExplo.More(); anExplo.Next())
          {
            const TopoDS_Shape& aVertex = anExplo.Current();
            const TopTools_ListOfShape& aFaceList = aVFmap.FindFromKey (aVertex);
            TopTools_ListIteratorOfListOfShape anItl (aFaceList);
            for (; anItl.More(); anItl.Next())
            {
              Standard_Real anArea = aFaceAreaMap (anItl.Value());
              if (anArea > aMaxArea)
                aMaxArea = anArea;
            }
          }
          Standard_Real anArreaOfSmallestFace = aFaceAreaMap (aFace);
          if (anArreaOfSmallestFace < aMaxArea / 16)
          {
            aBB.Remove (res, aFace);
            aRemovedFaces.Add (aFace);
          }
        }
      }

      ModifyFacesForGlobalResult (theFace, anAverageArea,
                                  Standard_False, //to decrease number of faces
                                  aNbExcessFaces, aSmallestFaces,
                                  aFacesAndAreas, aFaceAreaMap, aEFmap,
                                  res, aGlobalRes,
                                  aRemovedFaces);
    }
  }

  aBB.Add (aGlobalRes, res);

  aBB.MakeCompound (theCompound);
  for (TopExp_Explorer aGlobalExplo(aGlobalRes, TopAbs_FACE); aGlobalExplo.More(); aGlobalExplo.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face (aGlobalExplo.Current());
    Standard_Boolean anIsNaturalRestrictions = Standard_True;
    TopExp_Explorer anExplo (aFace, TopAbs_EDGE);
    for (; anExplo.More(); anExplo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (anExplo.Current());
      if (BRep_Tool::Degenerated (anEdge))
        continue;
      if (!aEFmap.Contains(anEdge) ||
          aEFmap.FindFromKey(anEdge).Extent() < 2)
      {
        anIsNaturalRestrictions = Standard_False;
        break;
      }
    }

    gp_Pnt aPnt = GetMidPnt2d (aFace, anIsNaturalRestrictions);
    TopoDS_Vertex aVertex = BRepLib_MakeVertex (aPnt);
    aBB.Add (theCompound, aVertex);
  }

  return 0;
#endif
}

Standard_Boolean comp(const std::pair<TopoDS_Shape, Standard_Real>& theA,
                      const std::pair<TopoDS_Shape, Standard_Real>& theB)
{
  return (theA.second < theB.second);
}

Standard_Boolean IsUiso (const TopoDS_Edge& theEdge,
                         const TopoDS_Face& theFace)
{
  BRepAdaptor_Curve2d aBAcurve2d (theEdge, theFace);
  gp_Pnt2d aP2d;
  gp_Vec2d aVec;
  aBAcurve2d.D1 (aBAcurve2d.FirstParameter(), aP2d, aVec);
  return (Abs(aVec.Y()) > Abs(aVec.X()));
}

void CorrectShell (const TopoDS_Shape& theShell,
                   const TopoDS_Face&  theFace)
{
  BRepAdaptor_Surface aBAsurf (theFace, Standard_False);
  GeomAbs_SurfaceType aType = aBAsurf.GetType();
  if (aType <= GeomAbs_Torus) //elementary surfaces
    return;

  TopLoc_Location anInputLoc;
  const Handle(Geom_Surface)& anInputSurf = BRep_Tool::Surface (theFace, anInputLoc);

  BRep_Builder aBB;
  
  TopoDS_Iterator anIter (theShell);
  for (; anIter.More(); anIter.Next())
  {
    const TopoDS_Face& aFace = TopoDS::Face (anIter.Value());
    TopLoc_Location aLoc;
    const Handle(Geom_Surface)& aSurf = BRep_Tool::Surface (aFace, aLoc);
    if (aSurf == anInputSurf)
      continue;

    TopExp_Explorer anExplo (aFace, TopAbs_EDGE);
    for (; anExplo.More(); anExplo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (anExplo.Current());
      Standard_Real aFirst, aLast;
      Handle(Geom2d_Curve) aPCurve = BRep_Tool::CurveOnSurface (anEdge, aFace, aFirst, aLast);
      aBB.UpdateEdge (anEdge, aPCurve, anInputSurf, anInputLoc, 0.);
    }
    Standard_Real aTol = BRep_Tool::Tolerance (aFace);
    aBB.UpdateFace (aFace, anInputSurf, anInputLoc, aTol);
  }
}

gp_Pnt GetMidPnt2d(const TopoDS_Face&     theFace,
                   const Standard_Boolean theIsNaturalRestrictions)
{
  gp_Pnt aResPnt;
  
  if (theIsNaturalRestrictions)
  {
    BRepAdaptor_Surface aBAsurf (theFace);
    Standard_Real aUmin, aUmax, aVmin, aVmax;
    aUmin = aBAsurf.FirstUParameter();
    aUmax = aBAsurf.LastUParameter();
    aVmin = aBAsurf.FirstVParameter();
    aVmax = aBAsurf.LastVParameter();
    aResPnt = aBAsurf.Value ((aUmin + aUmax)/2, (aVmin + aVmax)/2);
  }
  else
  {
    const Standard_Integer aNbSamples = 4;
    TopoDS_Wire aWire = BRepTools::OuterWire (theFace);
    TopTools_IndexedMapOfShape aEmap;
    TopExp::MapShapes (aWire, TopAbs_EDGE, aEmap);
    Standard_Integer aNbPointsOnContour = aNbSamples * aEmap.Extent();
    TColgp_Array1OfPnt anArray (1, aNbPointsOnContour);
    
    BRepTools_WireExplorer aWexp (aWire, theFace);
    Standard_Integer anInd = 0;
    for (; aWexp.More(); aWexp.Next())
    {
      const TopoDS_Edge& anEdge = aWexp.Current();
      BRepAdaptor_Curve2d aBAcurve2d (anEdge, theFace);
      Standard_Real aDelta = (aBAcurve2d.LastParameter() - aBAcurve2d.FirstParameter())/aNbSamples;
      for (Standard_Integer ii = 0; ii < aNbSamples; ii++)
      {
        Standard_Real aParam = aBAcurve2d.FirstParameter() + ii * aDelta;
        gp_Pnt2d aP2d = aBAcurve2d.Value (aParam);
        gp_Pnt aPnt (aP2d.X(), aP2d.Y(), 0.);
        anArray (++anInd) = aPnt;
      }
    }
    
    gp_Ax2 anAxis;
    Standard_Boolean anIsSingular;
    GeomLib::AxeOfInertia (anArray, anAxis, anIsSingular);
    gp_Pnt aBaryCentre = anAxis.Location();
    gp_Pnt2d aCentre2d (aBaryCentre.X(), aBaryCentre.Y());
    BRepTopAdaptor_FClass2d aClassifier (theFace, Precision::Confusion());
    BRepAdaptor_Surface aBAsurf (theFace, Standard_False);
    
    TopAbs_State aStatus = aClassifier.Perform (aCentre2d);
    gp_Pnt2d aP2d = aCentre2d;
    Standard_Integer anIndVertex = 0;
    const Standard_Integer aNbIter = 10;
    while (aStatus != TopAbs_IN && anIndVertex < aNbPointsOnContour)
    {
      gp_Pnt aVertexPnt = anArray (anIndVertex+1);
      gp_Pnt2d aVertexP2d (aVertexPnt.X(), aVertexPnt.Y());
      TColgp_SequenceOfPnt2d aPseq;
      aPseq.Append (aCentre2d);
      aPseq.Append (aVertexP2d);
      for (Standard_Integer ii = 1; ii <= aNbIter; ii++)
      {
        for (Standard_Integer jj = 1; jj < aPseq.Length(); jj++)
        {
          aP2d.SetXY ((aPseq(jj).XY() + aPseq(jj+1).XY())/2);
          aStatus = aClassifier.Perform (aP2d);
          if (aStatus == TopAbs_IN)
            break;
          else
          {
            aPseq.InsertAfter (jj, aP2d);
            jj++;
          }
        }
        if (aStatus == TopAbs_IN)
          break;
      }
      anIndVertex += aNbSamples;
    }
    aResPnt = aBAsurf.Value (aP2d.X(), aP2d.Y());
  } //case of complex boundaries

  return aResPnt;
}

void ModifyFacesForGlobalResult(const TopoDS_Face&     theInputFace,
                                const Standard_Real    theAverageArea,
                                const Standard_Boolean theIsToAddFaces,
                                Standard_Integer&      theNbExtremalFaces,
                                TopTools_MapOfShape&   theExtremalFaces,
                                const std::vector<std::pair<TopoDS_Shape, Standard_Real>> theFacesAndAreas,
                                const TopTools_DataMapOfShapeReal& theFaceAreaMap,
                                const TopTools_IndexedDataMapOfShapeListOfShape& theEFmap,
                                TopoDS_Shape&          theRes,
                                TopoDS_Shape&          theGlobalRes,
                                TopTools_MapOfShape&   theRemovedFaces)
{
  BRep_Builder aBB;
  const Standard_Integer aNbFaces = (Standard_Integer) theFacesAndAreas.size();

  const Standard_Integer aDiff = theNbExtremalFaces - theRemovedFaces.Extent();

  Standard_Integer aSum = 0;
  while (aSum < aDiff) //global loop
  {
    Standard_Integer aNbFacesDone = 0, aNbFacesInTape = 0;
    TopoDS_Face aStartFace;
    
    Standard_Integer aStartIndex = (theIsToAddFaces)? aNbFaces-1 : 0;
    Standard_Integer anEndIndex  = (theIsToAddFaces)? 0 : aNbFaces-1;
    Standard_Integer aStep = (theIsToAddFaces)? -1 : 1;
    
    for (Standard_Integer ii = aStartIndex; ii != anEndIndex; ii += aStep)
    {
      const TopoDS_Face& aFace = TopoDS::Face (theFacesAndAreas[ii].first);
      if (!theRemovedFaces.Contains(aFace))
      {
        aStartFace = aFace;
        break;
      }
    }
    if (aStartFace.IsNull())
      break;

    theRemovedFaces.Add (aStartFace);
    
    TopoDS_Edge aCommonEdge;
    TopoDS_Face aNextFace;
    Standard_Real anExtremalArea = (theIsToAddFaces)? 0. : Precision::Infinite();
    for (TopExp_Explorer anExplo(aStartFace, TopAbs_EDGE); anExplo.More(); anExplo.Next())
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (anExplo.Current());
      const TopTools_ListOfShape& aFaceList = theEFmap.FindFromKey (anEdge);
      TopTools_ListIteratorOfListOfShape anItl (aFaceList);
      for (; anItl.More(); anItl.Next())
      {
        const TopoDS_Face& aFace = TopoDS::Face (anItl.Value());
        if (aFace.IsSame (aStartFace) ||
            theRemovedFaces.Contains(aFace))
          continue;
        Standard_Real anArea = theFaceAreaMap(aFace);
        Standard_Boolean anIsToExchange = (theIsToAddFaces)? (anArea > anExtremalArea) : (anArea < anExtremalArea);
        if (anIsToExchange)
        {
          anExtremalArea = anArea;
          aCommonEdge = anEdge;
          aNextFace = aFace;
        }
      }
    }
    if (aCommonEdge.IsNull()) //all adjacent faces are already removed
    {
      theExtremalFaces.Add (theFacesAndAreas[theNbExtremalFaces].first);
      theNbExtremalFaces++;
      continue;
    }

    //Start filling the shell
    aBB.Remove (theRes, aStartFace);
    aNbFacesDone++;
    TopoDS_Shell aShell;
    aBB.MakeShell (aShell);
    Standard_Real anAreaOfTape = 0.;
    aBB.Add (aShell, aStartFace);
    aNbFacesInTape++;
    anAreaOfTape += theFaceAreaMap (aStartFace);
    
    Standard_Boolean anIsUiso = IsUiso (aCommonEdge, aStartFace);
    //Find another faces on this level
    TopoDS_Face aCurrentFace = aNextFace;
    TopoDS_Edge aCurrentEdge = aCommonEdge;
    Standard_Boolean anIsFirstDirection = Standard_True;
    aBB.Remove (theRes, aCurrentFace);
    theRemovedFaces.Add (aCurrentFace);
    if (theExtremalFaces.Contains (aCurrentFace))
    {
      aNbFacesDone++;
    }
    aBB.Add (aShell, aCurrentFace);
    aNbFacesInTape++;
    anAreaOfTape += theFaceAreaMap (aCurrentFace);
    Standard_Boolean anIsRound = Standard_False;
    for (;;) //local loop
    {
      TopoDS_Edge aNextEdge;
      for (TopExp_Explorer anExplo(aCurrentFace, TopAbs_EDGE); anExplo.More(); anExplo.Next())
      {
        const TopoDS_Edge& anEdge = TopoDS::Edge (anExplo.Current());
        if (anEdge.IsSame (aCurrentEdge))
          continue;
        const TopTools_ListOfShape& aFaceList = theEFmap.FindFromKey (anEdge);
        TopTools_ListIteratorOfListOfShape anItl (aFaceList);
        for (; anItl.More(); anItl.Next())
        {
          const TopoDS_Face& aFace = TopoDS::Face (anItl.Value());
          if (aFace.IsSame (aCurrentFace))
            continue;
          if (aFace.IsSame (aStartFace))
          {
            anIsRound = Standard_True;
            break;
          }
          if (theRemovedFaces.Contains(aFace))
            continue;
          if (anIsUiso == IsUiso (anEdge, aFace))
          {
            aNextEdge = anEdge;
            aNextFace = aFace;
            break;
          }
        }
        if (anIsRound || !aNextEdge.IsNull())
          break;
      }
      if (anIsRound) //round tape: returned to start face
        break;
      if (aNextEdge.IsNull())
      {
        if (anIsFirstDirection)
        {
          aCurrentFace = aStartFace;
          aCurrentEdge = aCommonEdge;
          anIsFirstDirection = Standard_False;
          continue;
        }
        else
          break;
      }
      
      aBB.Add (aShell, aNextFace);
      aNbFacesInTape++;
      anAreaOfTape += theFaceAreaMap (aNextFace);
      aBB.Remove (theRes, aNextFace);
      theRemovedFaces.Add (aNextFace);
      if (theExtremalFaces.Contains (aNextFace))
      {
        aNbFacesDone++;
      }
      aCurrentEdge = aNextEdge;
      aNextEdge.Nullify();
      aCurrentFace = aNextFace;
    } //end of local loop
    
    //Tape is formed
    Standard_Integer aNumberToSplit = (theIsToAddFaces)? aNbFacesInTape + aNbFacesDone : aNbFacesInTape - aNbFacesDone;
    if (!theIsToAddFaces && aNbFacesDone > 1)
    {
      Standard_Integer aRealNumberToSplit = (aNumberToSplit > 0)? aNumberToSplit : 1;
      Standard_Real anAverageAreaInTape = anAreaOfTape / aRealNumberToSplit;
      if (anAverageAreaInTape > theAverageArea)
      {
        Standard_Integer aNewNumberToSplit = RealToInt(round(anAreaOfTape / theAverageArea));
        if (aNewNumberToSplit < aNbFacesInTape)
        {
          Standard_Integer aNumberToIncrease = aNewNumberToSplit - aNumberToSplit;
          for (Standard_Integer jj = theNbExtremalFaces; jj < theNbExtremalFaces + aNumberToIncrease; jj++)
            theExtremalFaces.Add (theFacesAndAreas[jj].first);
          theNbExtremalFaces += aNumberToIncrease;
          aNumberToSplit = aNewNumberToSplit;
        }
      }
    }
    if (anIsRound && aNumberToSplit <= 1)
    {
      Standard_Integer aNumberToIncrease = 3 - aNumberToSplit;
      for (Standard_Integer jj = theNbExtremalFaces; jj < theNbExtremalFaces + aNumberToIncrease; jj++)
        theExtremalFaces.Add (theFacesAndAreas[jj].first);
      theNbExtremalFaces += aNumberToIncrease;
      aNumberToSplit = 3;
    }
    CorrectShell (aShell, theInputFace);
    ShapeUpgrade_UnifySameDomain aUnifier;
    aUnifier.Initialize (aShell, Standard_True, Standard_True);
    aUnifier.Build();
    TopoDS_Shape aUnifiedShape = aUnifier.Shape();
    //Splitting
    TopoDS_Shape aLocalResult = aUnifiedShape;
    Standard_Integer aNbFacesInLocalResult;
    if (aNumberToSplit > 1)
    {
#if OCC_VERSION_LARGE < 0x07050304
      aNbFacesInLocalResult = 0;
#else
      ShapeUpgrade_ShapeDivideArea aLocalTool (aUnifiedShape);
      aLocalTool.SetSplittingByNumber (Standard_True);
      aLocalTool.MaxArea() = -1;
      if (anIsUiso)
        aLocalTool.SetNumbersUVSplits (aNumberToSplit, 1);
      else
        aLocalTool.SetNumbersUVSplits (1, aNumberToSplit);
      aLocalTool.Perform();
      aLocalResult = aLocalTool.Result();
      aNbFacesInLocalResult = aNumberToSplit;
#endif
    }
    else
    {
      aNbFacesInLocalResult = 1;
      if (aNumberToSplit == 0)
      {
        theExtremalFaces.Add (theFacesAndAreas[theNbExtremalFaces].first);
        theNbExtremalFaces++;
      }
    }
    aBB.Add (theGlobalRes, aLocalResult);

    aSum += Abs(aNbFacesInTape - aNbFacesInLocalResult);
  } //end of global loop

  //Second global loop
  TopoDS_Compound aSecondComp;
  aBB.MakeCompound (aSecondComp);
  while (aSum < aDiff)
  {
    TopoDS_Shape aMaxShell;
    Standard_Integer aMaxNbFaces = 0;
    TopoDS_Iterator anIter (theGlobalRes);
    for (; anIter.More(); anIter.Next())
    {
      const TopoDS_Shape& aShell = anIter.Value();
      TopTools_IndexedMapOfShape aFaceMap;
      TopExp::MapShapes (aShell, TopAbs_FACE, aFaceMap);
      if (aFaceMap.Extent() > aMaxNbFaces)
      {
        aMaxNbFaces = aFaceMap.Extent();
        aMaxShell = aShell;
      }
    }

    if (aMaxNbFaces == 1)
      break;
    
    aBB.Remove (theGlobalRes, aMaxShell);
    //Find iso
    Standard_Boolean anIsUiso = Standard_True;
    TopTools_IndexedDataMapOfShapeListOfShape aLocalEFmap;
    TopExp::MapShapesAndAncestors (aMaxShell, TopAbs_EDGE, TopAbs_FACE, aLocalEFmap);
    for (Standard_Integer jj = 1; jj <= aLocalEFmap.Extent(); jj++)
    {
      const TopoDS_Edge& anEdge = TopoDS::Edge (aLocalEFmap.FindKey(jj));
      const TopTools_ListOfShape& aFaceList = aLocalEFmap(jj);
      if (aFaceList.Extent() == 2)
      {
        const TopoDS_Face& aFace = TopoDS::Face (aFaceList.First());
        anIsUiso = IsUiso (anEdge, aFace);
        break;
      }
    }
    CorrectShell (aMaxShell, theInputFace);
    ShapeUpgrade_UnifySameDomain aUnifier;
    aUnifier.Initialize (aMaxShell, Standard_True, Standard_True);
    aUnifier.Build();
    TopoDS_Shape aUnifiedShape = aUnifier.Shape();
    TopoDS_Shape aLocalResult = aUnifiedShape;
    
    Standard_Integer aNumberToSplit = (theIsToAddFaces)? aMaxNbFaces + (aDiff-aSum) : aMaxNbFaces - (aDiff-aSum);
    if (aNumberToSplit > 1)
    {
#if OCC_VERSION_LARGE < 0x07050304
      aNumberToSplit = 1;
#else
      ShapeUpgrade_ShapeDivideArea aLocalTool (aUnifiedShape);
      aLocalTool.SetSplittingByNumber (Standard_True);
      aLocalTool.MaxArea() = -1;
      if (anIsUiso)
        aLocalTool.SetNumbersUVSplits (aNumberToSplit, 1);
      else
        aLocalTool.SetNumbersUVSplits (1, aNumberToSplit);
      aLocalTool.Perform();
      aLocalResult = aLocalTool.Result();
#endif
    }
    else
      aNumberToSplit = 1;

    aBB.Add (aSecondComp, aLocalResult);
    
    if (theIsToAddFaces)
      break;
    aSum += aMaxNbFaces - aNumberToSplit;
  }
  aBB.Add (theGlobalRes, aSecondComp);
}
