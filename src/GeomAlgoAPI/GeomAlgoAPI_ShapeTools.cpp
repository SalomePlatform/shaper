// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeTools.h
// Created:     3 August 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_ShapeTools.h"

#include "GeomAlgoAPI_SketchBuilder.h"

#include <GeomAPI_Dir.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>

#include <Bnd_Box.hxx>
#include <BOPTools.hxx>
#include <BRep_Builder.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepGProp.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom_Curve.hxx>
#include <Geom2d_Curve.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <GeomLib_IsPlanarSurface.hxx>
#include <GeomLib_Tool.hxx>
#include <GeomProjLib.hxx>
#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>
#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>
#include <NCollection_Vector.hxx>
#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS.hxx>
#include <TopExp_Explorer.hxx>

//==================================================================================================
double GeomAlgoAPI_ShapeTools::volume(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if(!theShape.get()) {
    return 0.0;
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if(aShape.IsNull()) {
    return 0.0;
  }
  const Standard_Real anEps = 1.e-6;
  BRepGProp::VolumeProperties(aShape, aGProps, anEps);
  return aGProps.Mass();
}

//==================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAlgoAPI_ShapeTools::centreOfMass(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if(!theShape) {
    return std::shared_ptr<GeomAPI_Pnt>();
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if(aShape.IsNull()) {
    return std::shared_ptr<GeomAPI_Pnt>();
  }
  gp_Pnt aCentre;
  if(aShape.ShapeType() == TopAbs_VERTEX) {
    aCentre = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
  } else if(aShape.ShapeType() == TopAbs_EDGE || aShape.ShapeType() == TopAbs_WIRE) {
    BRepGProp::LinearProperties(aShape, aGProps);
    aCentre = aGProps.CentreOfMass();
  } else {
    BRepGProp::SurfaceProperties(aShape, aGProps);
    aCentre = aGProps.CentreOfMass();
  }
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCentre.X(), aCentre.Y(), aCentre.Z()));
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::combineShapes(const std::shared_ptr<GeomAPI_Shape> theCompound,
                                                                     const GeomAPI_Shape::ShapeType theType,
                                                                     ListOfShape& theCombinedShapes,
                                                                     ListOfShape& theFreeShapes)
{
  GeomShapePtr aResult = theCompound;

  if(!theCompound.get()) {
    return aResult;
  }

  if(theType != GeomAPI_Shape::SHELL && theType != GeomAPI_Shape::COMPSOLID) {
    return aResult;
  }

  TopAbs_ShapeEnum aTS = TopAbs_EDGE;
  TopAbs_ShapeEnum aTA = TopAbs_FACE;
  if(theType == GeomAPI_Shape::COMPSOLID) {
    aTS = TopAbs_FACE;
    aTA = TopAbs_SOLID;
  }

  theCombinedShapes.clear();
  theFreeShapes.clear();

  // Get free shapes.
  const TopoDS_Shape& aShapesComp = theCompound->impl<TopoDS_Shape>();
  for(TopoDS_Iterator anIter(aShapesComp); anIter.More(); anIter.Next() ) {
    const TopoDS_Shape& aShape = anIter.Value();
    if(aShape.ShapeType() > aTA) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
      aGeomShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
      theFreeShapes.push_back(aGeomShape);
    }
  }

  // Map subshapes and shapes.
  BOPCol_IndexedDataMapOfShapeListOfShape aMapSA;
  BOPTools::MapShapesAndAncestors(aShapesComp, aTS, aTA, aMapSA);
  if(aMapSA.IsEmpty()) {
    return aResult;
  }

  // Get all shapes with common subshapes and free shapes.
  NCollection_Map<TopoDS_Shape> aFreeShapes;
  NCollection_Vector<NCollection_Map<TopoDS_Shape>> aShapesWithCommonSubshapes;
  for(BOPCol_IndexedDataMapOfShapeListOfShape::Iterator anIter(aMapSA); anIter.More(); anIter.Next()) {
    const TopoDS_Shape& aShape = anIter.Key();
    BOPCol_ListOfShape& aListOfShape = anIter.ChangeValue();
    if(aListOfShape.IsEmpty()) {
      continue;
    }
    else if(aListOfShape.Size() == 1) {
      const TopoDS_Shape& aF = aListOfShape.First();
      aFreeShapes.Add(aF);
      aListOfShape.Clear();
    } else {
      NCollection_List<TopoDS_Shape> aTempList;
      NCollection_Map<TopoDS_Shape> aTempMap;
      const TopoDS_Shape& aF = aListOfShape.First();
      const TopoDS_Shape& aL = aListOfShape.Last();
      aTempList.Append(aF);
      aTempList.Append(aL);
      aTempMap.Add(aF);
      aTempMap.Add(aL);
      aFreeShapes.Remove(aF);
      aFreeShapes.Remove(aL);
      aListOfShape.Clear();
      for(NCollection_List<TopoDS_Shape>::Iterator aTempIter(aTempList); aTempIter.More(); aTempIter.Next()) {
        const TopoDS_Shape& aTempShape = aTempIter.Value();
        for(BOPCol_IndexedDataMapOfShapeListOfShape::Iterator anIter(aMapSA); anIter.More(); anIter.Next()) {
          BOPCol_ListOfShape& aTempListOfShape = anIter.ChangeValue();
          if(aTempListOfShape.IsEmpty()) {
            continue;
          } else if(aTempListOfShape.Size() == 1 && aTempListOfShape.First() == aTempShape) {
            aTempListOfShape.Clear();
          } else if(aTempListOfShape.Size() > 1) {
            if(aTempListOfShape.First() == aTempShape) {
              const TopoDS_Shape& aTL = aTempListOfShape.Last();
              if(aTempMap.Add(aTL)) {
                aTempList.Append(aTL);
                aFreeShapes.Remove(aTL);
              }
              aTempListOfShape.Clear();
            } else if(aTempListOfShape.Last() == aTempShape) {
              const TopoDS_Shape& aTF = aTempListOfShape.First();
              if(aTempMap.Add(aTF)) {
                aTempList.Append(aTF);
                aFreeShapes.Remove(aTF);
              }
              aTempListOfShape.Clear();
            }
          }
        }
      }
      aShapesWithCommonSubshapes.Append(aTempMap);
    }
  }

  // Combine shapes with common subshapes.
  for(NCollection_Vector<NCollection_Map<TopoDS_Shape>>::Iterator anIter(aShapesWithCommonSubshapes); anIter.More(); anIter.Next()) {
    TopoDS_Shell aShell;
    TopoDS_CompSolid aCSolid;
    TopoDS_Builder aBuilder;
    theType == GeomAPI_Shape::COMPSOLID ? aBuilder.MakeCompSolid(aCSolid) : aBuilder.MakeShell(aShell);
    NCollection_Map<TopoDS_Shape>& aShapesMap = anIter.ChangeValue();
    for(TopExp_Explorer anExp(aShapesComp, aTA); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aShape = anExp.Current();
      if(aShapesMap.Contains(aShape)) {
        theType == GeomAPI_Shape::COMPSOLID ? aBuilder.Add(aCSolid, aShape) : aBuilder.Add(aShell, aShape);
        aShapesMap.Remove(aShape);
      }
    }
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    TopoDS_Shape* aSh = theType == GeomAPI_Shape::COMPSOLID ? new TopoDS_Shape(aCSolid) : new TopoDS_Shape(aShell);
    aGeomShape->setImpl<TopoDS_Shape>(aSh);
    theCombinedShapes.push_back(aGeomShape);
  }

  // Adding free shapes.
  for(TopExp_Explorer anExp(aShapesComp, aTA); anExp.More(); anExp.Next()) {
    const TopoDS_Shape& aShape = anExp.Current();
    if(aFreeShapes.Contains(aShape)) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
      aGeomShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
      theFreeShapes.push_back(aGeomShape);
    }
  }

  if(theCombinedShapes.size() == 1 && theFreeShapes.size() == 0) {
    aResult = theCombinedShapes.front();
  } else if (theCombinedShapes.size() > 1 || (theCombinedShapes.size() >= 1 && theFreeShapes.size() >= 1)) {
    TopoDS_Compound aResultComp;
    TopoDS_Builder aBuilder;
    aBuilder.MakeCompound(aResultComp);
    for(ListOfShape::const_iterator anIter = theCombinedShapes.cbegin(); anIter != theCombinedShapes.cend(); anIter++) {
      aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
    }
    for(ListOfShape::const_iterator anIter = theFreeShapes.cbegin(); anIter != theFreeShapes.cend(); anIter++) {
      aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
    }
    aResult->setImpl(new TopoDS_Shape(aResultComp));
  }

  return aResult;
}

//==================================================================================================
static void addSimpleShapeToList(const TopoDS_Shape& theShape, NCollection_List<TopoDS_Shape>& theList)
{
  if(theShape.IsNull()) {
    return;
  }

  if(theShape.ShapeType() == TopAbs_COMPOUND) {
    for(TopoDS_Iterator anIt(theShape); anIt.More(); anIt.Next()) {
      addSimpleShapeToList(anIt.Value(), theList);
    }
  } else {
    theList.Append(theShape);
  }
}

//==================================================================================================
static TopoDS_Compound makeCompound(const NCollection_List<TopoDS_Shape> theShapes)
{
  TopoDS_Compound aCompound;

  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aCompound);

  for(NCollection_List<TopoDS_Shape>::Iterator anIt(theShapes); anIt.More(); anIt.Next()) {
    aBuilder.Add(aCompound, anIt.Value());
  }

  return aCompound;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::groupSharedTopology(const std::shared_ptr<GeomAPI_Shape> theCompound)
{
  GeomShapePtr aResult = theCompound;

  if(!theCompound.get()) {
    return aResult;
  }

  TopoDS_Shape anInShape = aResult->impl<TopoDS_Shape>();
  NCollection_List<TopoDS_Shape> anUngroupedShapes;
  addSimpleShapeToList(anInShape, anUngroupedShapes);

  NCollection_Vector<NCollection_List<TopoDS_Shape>> aGroups;
  while(!anUngroupedShapes.IsEmpty()) {
    NCollection_List<TopoDS_Shape> aGroupedShapes;
    aGroupedShapes.Append(anUngroupedShapes.First());
    anUngroupedShapes.RemoveFirst();
    for(NCollection_List<TopoDS_Shape>::Iterator aGroupIt(aGroupedShapes); aGroupIt.More(); aGroupIt.Next()) {
      const TopoDS_Shape& aGroupShape = aGroupIt.Value();
      for(NCollection_List<TopoDS_Shape>::Iterator anUngroupedIt(anUngroupedShapes); anUngroupedIt.More(); anUngroupedIt.Next()) {
        const TopoDS_Shape& anUngroupedShape = anUngroupedIt.Value();
        bool isFound = false;
        for(TopExp_Explorer aGroupShapeExp(aGroupShape, TopAbs_VERTEX); aGroupShapeExp.More(); aGroupShapeExp.Next()) {
          const TopoDS_Shape& aVertex1 = aGroupShapeExp.Current();
          for(TopExp_Explorer anUngroupedShapeExp(anUngroupedShape, TopAbs_VERTEX); anUngroupedShapeExp.More(); anUngroupedShapeExp.Next()) {
            const TopoDS_Shape& aVertex2 = anUngroupedShapeExp.Current();
            if(aVertex1.IsSame(aVertex2)) {
              aGroupedShapes.Append(anUngroupedShape);
              anUngroupedShapes.Remove(anUngroupedIt);
              isFound = true;
              break;
            }
          }
          if(isFound) {
            break;
          }
        }
        if(!anUngroupedIt.More()) {
          break;
        }
      }
    }
    aGroups.Append(aGroupedShapes);
  }

  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aCompound);
  ListOfShape aCompSolids, aFreeSolids;
  for(NCollection_Vector<NCollection_List<TopoDS_Shape>>::Iterator anIt(aGroups); anIt.More(); anIt.Next()) {
    TopoDS_Compound aGroupCompound = makeCompound(anIt.Value());
    GeomShapePtr aGeomShape(new GeomAPI_Shape());
    aGeomShape->setImpl(new TopoDS_Shape(aGroupCompound));
    aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                       GeomAPI_Shape::COMPSOLID,
                                                       aCompSolids,
                                                       aFreeSolids);
    aBuilder.Add(aCompound, aGeomShape->impl<TopoDS_Shape>());
  }

  if(!aCompound.IsNull()) {
    aResult->setImpl(new TopoDS_Shape(aCompound));
  }

  return aResult;
}

//==================================================================================================
std::list<std::shared_ptr<GeomAPI_Pnt> > GeomAlgoAPI_ShapeTools::getBoundingBox(const ListOfShape& theShapes, const double theEnlarge)
{
  // Bounding box of all objects.
  Bnd_Box aBndBox;

  // Getting box.
  for (ListOfShape::const_iterator anObjectsIt = theShapes.begin(); anObjectsIt != theShapes.end(); anObjectsIt++) {
    const TopoDS_Shape& aShape = (*anObjectsIt)->impl<TopoDS_Shape>();
    BRepBndLib::Add(aShape, aBndBox);
  }

  if(theEnlarge != 0.0) {
    // We enlarge bounding box just to be sure that plane will be large enough to cut all objects.
    aBndBox.Enlarge(theEnlarge);
  }

  Standard_Real aXArr[2] = {aBndBox.CornerMin().X(), aBndBox.CornerMax().X()};
  Standard_Real aYArr[2] = {aBndBox.CornerMin().Y(), aBndBox.CornerMax().Y()};
  Standard_Real aZArr[2] = {aBndBox.CornerMin().Z(), aBndBox.CornerMax().Z()};
  std::list<std::shared_ptr<GeomAPI_Pnt> > aResultPoints;
  int aNum = 0;
  for(int i = 0; i < 2; i++) {
    for(int j = 0; j < 2; j++) {
      for(int k = 0; k < 2; k++) {
        std::shared_ptr<GeomAPI_Pnt> aPnt(new GeomAPI_Pnt(aXArr[i], aYArr[j], aZArr[k]));
        aResultPoints.push_back(aPnt);
      }
    }
  }

  return aResultPoints;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::faceToInfinitePlane(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  if (!theFace.get())
    return std::shared_ptr<GeomAPI_Shape>();

  TopoDS_Face aPlaneFace = TopoDS::Face(theFace->impl<TopoDS_Shape>());
  if (aPlaneFace.IsNull())
    return std::shared_ptr<GeomAPI_Shape>();

  Handle(Geom_Plane) aPlane = Handle(Geom_Plane)::DownCast(BRep_Tool::Surface(aPlaneFace));
  if (aPlane.IsNull())
    return std::shared_ptr<GeomAPI_Shape>();

  // make an infinity face on the plane
  TopoDS_Shape anInfiniteFace = BRepBuilderAPI_MakeFace(aPlane->Pln()).Shape();

  std::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape);
  aResult->setImpl(new TopoDS_Shape(anInfiniteFace));
  return aResult;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::fitPlaneToBox(const std::shared_ptr<GeomAPI_Shape> thePlane,
                                                                     const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints)
{
  std::shared_ptr<GeomAPI_Shape> aResultShape;

  if(!thePlane.get()) {
    return aResultShape;
  }

  const TopoDS_Shape& aShape = thePlane->impl<TopoDS_Shape>();
  if(aShape.ShapeType() != TopAbs_FACE) {
    return aResultShape;
  }

  TopoDS_Face aFace = TopoDS::Face(aShape);
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if(aSurf.IsNull()) {
    return aResultShape;
  }

  GeomLib_IsPlanarSurface isPlanar(aSurf);
  if(!isPlanar.IsPlanar()) {
    return aResultShape;
  }

  if(thePoints.size() != 8) {
    return aResultShape;
  }

  const gp_Pln& aFacePln = isPlanar.Plan();
  Handle(Geom_Plane) aFacePlane = new Geom_Plane(aFacePln);
  IntAna_Quadric aQuadric(aFacePln);
  Standard_Real UMin, UMax, VMin, VMax;
  UMin = UMax = VMin = VMax = 0;
  for (std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator aPointsIt = thePoints.begin(); aPointsIt != thePoints.end(); aPointsIt++) {
    const gp_Pnt& aPnt = (*aPointsIt)->impl<gp_Pnt>();
    gp_Lin aLin(aPnt, aFacePln.Axis().Direction());
    IntAna_IntConicQuad anIntAna(aLin, aQuadric);
    const gp_Pnt& aPntOnFace = anIntAna.Point(1);
    Standard_Real aPntU(0), aPntV(0);
    GeomLib_Tool::Parameters(aFacePlane, aPntOnFace, Precision::Confusion(), aPntU, aPntV);
    if(aPntU < UMin) UMin = aPntU;
    if(aPntU > UMax) UMax = aPntU;
    if(aPntV < VMin) VMin = aPntV;
    if(aPntV > VMax) VMax = aPntV;
  }
  aResultShape.reset(new GeomAPI_Shape);
  aResultShape->setImpl(new TopoDS_Shape(BRepLib_MakeFace(aFacePln, UMin, UMax, VMin, VMax).Face()));

  return aResultShape;
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::findBounds(const std::shared_ptr<GeomAPI_Shape> theShape,
                                        std::shared_ptr<GeomAPI_Vertex>& theV1,
                                        std::shared_ptr<GeomAPI_Vertex>& theV2)
{
  if(!theShape.get()) {
    std::shared_ptr<GeomAPI_Vertex> aVertex(new GeomAPI_Vertex);
    aVertex->setImpl(new TopoDS_Vertex());
    theV1 = aVertex;
    theV2 = aVertex;
    return;
  }

  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  TopoDS_Vertex aV1, aV2;
  ShapeAnalysis::FindBounds(aShape, aV1, aV2);

  std::shared_ptr<GeomAPI_Vertex> aGeomV1(new GeomAPI_Vertex()), aGeomV2(new GeomAPI_Vertex());
  aGeomV1->setImpl(new TopoDS_Vertex(aV1));
  aGeomV2->setImpl(new TopoDS_Vertex(aV2));
  theV1 = aGeomV1;
  theV2 = aGeomV2;
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::makeFacesWithHoles(const std::shared_ptr<GeomAPI_Pnt> theOrigin,
                                                const std::shared_ptr<GeomAPI_Dir> theDirection,
                                                const ListOfShape& theWires,
                                                ListOfShape& theFaces)
{
  BRepBuilderAPI_MakeFace aMKFace(gp_Pln(theOrigin->impl<gp_Pnt>(),
                                          theDirection->impl<gp_Dir>()));
  TopoDS_Face aFace = aMKFace.Face();

  BRepAlgo_FaceRestrictor aFRestrictor;
  aFRestrictor.Init(aFace, Standard_False, Standard_True);
  for(ListOfShape::const_iterator anIt = theWires.cbegin();
      anIt != theWires.cend();
      ++anIt) {
    TopoDS_Wire aWire = TopoDS::Wire((*anIt)->impl<TopoDS_Shape>());
    aFRestrictor.Add(aWire);
  }

  aFRestrictor.Perform();

  if(!aFRestrictor.IsDone()) {
    return;
  }

  for(; aFRestrictor.More(); aFRestrictor.Next()) {
    GeomShapePtr aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aFRestrictor.Current()));
    theFaces.push_back(aShape);
  }
}

//==================================================================================================
std::shared_ptr<GeomAPI_Pln> GeomAlgoAPI_ShapeTools::findPlane(const ListOfShape& theShapes)
{
  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aCompound);

  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    aBuilder.Add(aCompound, (*anIt)->impl<TopoDS_Shape>());
  }
  BRepBuilderAPI_FindPlane aFindPlane(aCompound);

  if(aFindPlane.Found() != Standard_True) {
    return std::shared_ptr<GeomAPI_Pln>();
  }

  Handle(Geom_Plane) aPlane = aFindPlane.Plane();
  gp_Pnt aLoc = aPlane->Location();
  gp_Dir aDir = aPlane->Axis().Direction();

  std::shared_ptr<GeomAPI_Pnt> aGeomPnt(new GeomAPI_Pnt(aLoc.X(), aLoc.Y(), aLoc.Z()));
  std::shared_ptr<GeomAPI_Dir> aGeomDir(new GeomAPI_Dir(aDir.X(), aDir.Y(), aDir.Z()));

  std::shared_ptr<GeomAPI_Pln> aPln(new GeomAPI_Pln(aGeomPnt, aGeomDir));

  return aPln;
}

//==================================================================================================
bool GeomAlgoAPI_ShapeTools::isSubShapeInsideShape(const std::shared_ptr<GeomAPI_Shape> theSubShape,
                                                   const std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  if(!theSubShape.get() || !theBaseShape.get()) {
    return false;
  }

  const TopoDS_Shape& aSubShape = theSubShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();

  if(aSubShape.ShapeType() == TopAbs_VERTEX) {
    // If sub-shape is a vertex check distance to shape. If it is <= Precision::Confusion() then OK.
    BRepExtrema_DistShapeShape aDist(aBaseShape, aSubShape);
    aDist.Perform();
    if(!aDist.IsDone() || aDist.Value() > Precision::Confusion()) {
      return false;
    }
  } else if (aSubShape.ShapeType() == TopAbs_EDGE) {
    if(aBaseShape.ShapeType() == TopAbs_FACE) {
      // Check that edge is on face surface.
      TopoDS_Face aFace = TopoDS::Face(aBaseShape);
      TopoDS_Edge anEdge = TopoDS::Edge(aSubShape);
      BRepLib_CheckCurveOnSurface aCheck(anEdge, aFace);
      aCheck.Perform();
      if(!aCheck.IsDone() || aCheck.MaxDistance() > Precision::Confusion()) {
        return false;
      }

      // Check intersections.
      TopoDS_Vertex aV1, aV2;
      ShapeAnalysis::FindBounds(anEdge, aV1, aV2);
      gp_Pnt aPnt1 = BRep_Tool::Pnt(aV1);
      gp_Pnt aPnt2 = BRep_Tool::Pnt(aV2);
      for(TopExp_Explorer anExp(aBaseShape, TopAbs_EDGE); anExp.More(); anExp.Next()) {
        const TopoDS_Shape& anEdgeOnFace = anExp.Current();
        BRepExtrema_DistShapeShape aDist(anEdgeOnFace, anEdge);
        aDist.Perform();
        if(aDist.IsDone() && aDist.Value() <= Precision::Confusion()) {
          // Edge intersect face bound. Check that it is not on edge begin or end.
          for(Standard_Integer anIndex = 1; anIndex <= aDist.NbSolution(); ++anIndex) {
            gp_Pnt aPntOnSubShape = aDist.PointOnShape2(anIndex);
            if(aPntOnSubShape.Distance(aPnt1) > Precision::Confusion()
                && aPntOnSubShape.Distance(aPnt2) > Precision::Confusion()) {
              return false;
            }
          }
        }
      }

      // No intersections found. Edge is inside or outside face. Check it.
      BRepAdaptor_Curve aCurveAdaptor(anEdge);
      gp_Pnt aPointToCheck = aCurveAdaptor.Value((aCurveAdaptor.FirstParameter() + aCurveAdaptor.LastParameter()) / 2.0);
      Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
      ShapeAnalysis_Surface aSAS(aSurface);
      gp_Pnt2d aPointOnFace = aSAS.ValueOfUV(aPointToCheck, Precision::Confusion());
      BRepTopAdaptor_FClass2d aFClass2d(aFace, Precision::Confusion());
      if(aFClass2d.Perform(aPointOnFace) == TopAbs_OUT) {
        return false;
      }

    } else {
      return false;
    }
  } else {
    return false;
  }

  return true;
}

//==================================================================================================
bool GeomAlgoAPI_ShapeTools::isShapeValid(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  if(!theShape.get()) {
    return false;
  }

  BRepCheck_Analyzer aChecker(theShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}