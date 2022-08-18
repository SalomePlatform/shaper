// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include "GeomAlgoAPI_PointCloudOnFace.h"

#include <TopoDS_Shape.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepLib_MakeVertex.hxx>
#include <GeomLib.hxx>
#include <gp_Ax3.hxx>
#include <Geom_Plane.hxx>
#include <gp_Pln.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopExp_Explorer.hxx>
#include <TopExp.hxx>
#include <GProp_GProps.hxx>
#include <BRepGProp.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <ShapeUpgrade_UnifySameDomain.hxx>
#include <ShapeUpgrade_ShapeDivideArea.hxx>
#include <BRepAdaptor_Surface.hxx>
#include <BRepAdaptor_Curve2d.hxx>
#include <BRepBndLib.hxx>
#include <GeomLProp_SLProps.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <TopTools_DataMapOfShapeReal.hxx>

#include <Standard_Version.hxx>
// code from KERNEL_SRC/src/Basics/Basics_OCCTVersion.hxx
#ifdef OCC_VERSION_SERVICEPACK
#  define OCC_VERSION_LARGE (OCC_VERSION_MAJOR << 24 | OCC_VERSION_MINOR << 16 | OCC_VERSION_MAINTENANCE << 8 | OCC_VERSION_SERVICEPACK)
#else
#  ifdef OCC_VERSION_DEVELOPMENT
#    define OCC_VERSION_LARGE ((OCC_VERSION_MAJOR << 24 | OCC_VERSION_MINOR << 16 | OCC_VERSION_MAINTENANCE << 8)-1)
#  else
#    define OCC_VERSION_LARGE (OCC_VERSION_MAJOR << 24 | OCC_VERSION_MINOR << 16 | OCC_VERSION_MAINTENANCE << 8)
#  endif
#endif

#include <algorithm>

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
    Standard_Integer aNbFacesInLocalResult = 1;
    if (aNumberToSplit > 1)
    {
#if OCC_VERSION_LARGE < 0x07050304
      aNbFacesInLocalResult = 1;
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

//=================================================================================================
bool GeomAlgoAPI_PointCloudOnFace::PointCloud(GeomShapePtr theFace,
                                              const int    theNumberOfPoints,
                                              GeomShapePtr thePoints,
                                              std::string& theError)
{
#ifdef _DEBUG
  std::cout << "GeomAlgoAPI_PointCloudOnFace::PointCloud" << std::endl;
#endif

#if OCC_VERSION_LARGE < 0x07050304
  theError = "Improper OCCT version: please, use OCCT 7.5.3p4 or newer.";
  return false;
#else

  if (!theFace.get()) {
    theError = "Face for point cloud calculation is null";
    return false;
  }

  TopoDS_Shape anInputShape = theFace->impl<TopoDS_Shape>();

  if (anInputShape.ShapeType() != TopAbs_FACE) {
    theError = "Shape for normale calculation is not a face";
    return false;
  }

  TopoDS_Face anInputFace = TopoDS::Face (anInputShape);

  ShapeUpgrade_ShapeDivideArea tool (anInputFace);
  tool.SetSplittingByNumber (Standard_True);
  tool.NbParts() = theNumberOfPoints;
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
  if (aNbFaces != theNumberOfPoints)
  {
    Standard_Real aTotalArea = 0.;
    std::vector<std::pair<TopoDS_Shape, Standard_Real>> aFacesAndAreas (aNbFaces);
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

    Standard_Real anAverageArea = aTotalArea / theNumberOfPoints;

    TopTools_DataMapOfShapeReal aFaceAreaMap;
    for (Standard_Integer ii = 0; ii < aNbFaces; ii++)
      aFaceAreaMap.Bind (aFacesAndAreas[ii].first, aFacesAndAreas[ii].second);

    TopTools_MapOfShape aRemovedFaces;

    if (aNbFaces < theNumberOfPoints)
    {
      Standard_Integer aNbMissingFaces = theNumberOfPoints - aNbFaces;
      for (Standard_Integer ii = aNbFaces-1; ii > aNbFaces - aNbMissingFaces - 1; ii--)
        aBiggestFaces.Add (aFacesAndAreas[ii].first);

      ModifyFacesForGlobalResult (anInputFace, anAverageArea,
                                  Standard_True, //to add faces
                                  aNbMissingFaces, aBiggestFaces,
                                  aFacesAndAreas, aFaceAreaMap, aEFmap,
                                  res, aGlobalRes,
                                  aRemovedFaces);
    }
    else //aNbFaces > theNumberOfPoints
    {
      Standard_Integer aNbExcessFaces = aNbFaces - theNumberOfPoints;
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

      ModifyFacesForGlobalResult (anInputFace, anAverageArea,
                                  Standard_False, //to decrease number of faces
                                  aNbExcessFaces, aSmallestFaces,
                                  aFacesAndAreas, aFaceAreaMap, aEFmap,
                                  res, aGlobalRes,
                                  aRemovedFaces);
    }
  }

  aBB.Add (aGlobalRes, res);

  TopoDS_Compound aCompound;
  aBB.MakeCompound (aCompound);
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
    aBB.Add (aCompound, aVertex);
  }

  thePoints->setImpl(new TopoDS_Shape(aCompound));

  return true;
#endif
}
