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

#include "GeomAlgoAPI_ShapeTools.h"

#include "GeomAlgoAPI_SketchBuilder.h"

#include <GeomAPI_Ax1.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Dir.h>
#include <GeomAPI_Face.h>
#include <GeomAPI_Pln.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_Wire.h>

#include <Approx_CurvilinearParameter.hxx>

#include <Bnd_Box.hxx>

#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <BRepAlgo.hxx>
#include <BRepAlgo_FaceRestrictor.hxx>
#include <BRepAdaptor_Curve.hxx>
#include <BRepBndLib.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepBuilderAPI_FindPlane.hxx>
#include <BRepBuilderAPI_MakeEdge.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepBuilderAPI_MakeVertex.hxx>
#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepCheck_Analyzer.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepExtrema_ExtCF.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <BRepTools_WireExplorer.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <BRepClass_FaceClassifier.hxx>
#include <BRepLib_CheckCurveOnSurface.hxx>
#include <BRepLProp.hxx>

#include <BOPAlgo_Builder.hxx>

#include <Geom2d_Curve.hxx>
#include <Geom2d_Curve.hxx>

#include <Geom_BSplineCurve.hxx>
#include <Geom_CylindricalSurface.hxx>
#include <Geom_Line.hxx>
#include <Geom_Plane.hxx>
#include <Geom_RectangularTrimmedSurface.hxx>

#include <GeomAdaptor_HCurve.hxx>

#include <GeomAPI_ProjectPointOnCurve.hxx>
#include <GeomAPI_ShapeIterator.h>

#include <GeomLib_IsPlanarSurface.hxx>
#include <GeomLib_Tool.hxx>
#include <GeomAPI_IntCS.hxx>

#include <gp_Pln.hxx>
#include <GProp_GProps.hxx>

#include <IntAna_IntConicQuad.hxx>
#include <IntAna_Quadric.hxx>

#include <ShapeAnalysis.hxx>
#include <ShapeAnalysis_Surface.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS_Vertex.hxx>
#include <TopoDS_Builder.hxx>

#include <TopExp.hxx>
#include <TopExp_Explorer.hxx>

#include <TopTools_ListIteratorOfListOfShape.hxx>

#include <NCollection_Vector.hxx>

#include <LocalAnalysis_SurfaceContinuity.hxx>
#include<array>

//==================================================================================================
static GProp_GProps props(const TopoDS_Shape& theShape)
{
  GProp_GProps aGProps;

  if (theShape.ShapeType() == TopAbs_EDGE || theShape.ShapeType() == TopAbs_WIRE)
  {
    BRepGProp::LinearProperties(theShape, aGProps);
  }
  else if (theShape.ShapeType() == TopAbs_FACE || theShape.ShapeType() == TopAbs_SHELL)
  {
    const Standard_Real anEps = 1.e-6;
    BRepGProp::SurfaceProperties(theShape, aGProps, anEps);
  }
  else if (theShape.ShapeType() == TopAbs_SOLID || theShape.ShapeType() == TopAbs_COMPSOLID)
  {
    BRepGProp::VolumeProperties(theShape, aGProps);
  }
  else if (theShape.ShapeType() == TopAbs_COMPOUND)
  {
    for (TopoDS_Iterator anIt(theShape); anIt.More(); anIt.Next())
    {
      aGProps.Add(props(anIt.Value()));
    }
  }

  return aGProps;
}

//==================================================================================================
double GeomAlgoAPI_ShapeTools::length(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if (!theShape.get()) {
    return 0.0;
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull()) {
    return 0.0;
  }

  BRepGProp::LinearProperties(aShape, aGProps, Standard_True);
  return  aGProps.Mass();
}

//==================================================================================================
double GeomAlgoAPI_ShapeTools::volume(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  if (!theShape.get()) {
    return 0.0;
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull()) {
    return 0.0;
  }
  const Standard_Real anEps = 1.e-6;
  double aVolume = 0.0;
  for (TopExp_Explorer anExp(aShape, TopAbs_SOLID); anExp.More(); anExp.Next()) {
    GProp_GProps aGProps;
    BRepGProp::VolumeProperties(anExp.Current(), aGProps, anEps);
    aVolume += aGProps.Mass();
  }
  return aVolume;
}

//==================================================================================================
double GeomAlgoAPI_ShapeTools::area (const std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if (!theShape.get()) {
    return 0.0;
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull()) {
    return 0.0;
  }
  const Standard_Real anEps = 1.e-6;

  BRepGProp::SurfaceProperties(aShape, aGProps, anEps);
  return aGProps.Mass();
}

//==================================================================================================
bool GeomAlgoAPI_ShapeTools::isContinuousFaces(const GeomShapePtr& theFace1,
                                               const GeomShapePtr& theFace2,
                                               const GeomPointPtr& thePoint,
                                               const double & theAngle,
                                               std::string& theError)
{

  #ifdef _DEBUG
  std::cout << "isContinuousFaces " << std::endl;
  #endif

  if (!thePoint.get()) {
      theError = "isContinuousFaces : An invalid argument";
      return false;
  }
  const gp_Pnt& aPoint = thePoint->impl<gp_Pnt>();

  // Getting base shape.
  if (!theFace1.get()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  TopoDS_Shape aShape1 = theFace1->impl<TopoDS_Shape>();

  if (aShape1.IsNull()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  // Getting base shape.
  if (!theFace2.get()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  TopoDS_Shape aShape2 = theFace2->impl<TopoDS_Shape>();

  if (aShape2.IsNull()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  TopoDS_Face aFace1 = TopoDS::Face(aShape1);
  if (aFace1.IsNull()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  Handle(Geom_Surface) aSurf1 = BRep_Tool::Surface(aFace1);
  if (aSurf1.IsNull()) {
    theError = "isContinuousFaces : An invalid surface";
    return false;
  }

  ShapeAnalysis_Surface aSAS1(aSurf1);
  gp_Pnt2d aPointOnFace1 = aSAS1.ValueOfUV(aPoint, Precision::Confusion());

  TopoDS_Face aFace2 = TopoDS::Face(aShape2);
  if (aFace2.IsNull()) {
    theError = "isContinuousFaces : An invalid argument";
    return false;
  }

  Handle(Geom_Surface) aSurf2 = BRep_Tool::Surface(aFace2);
  if (aSurf2.IsNull()) {
    theError = "isContinuousFaces : An invalid surface";
    return false;
  }

  ShapeAnalysis_Surface aSAS2(aSurf2);
  gp_Pnt2d aPointOnFace2= aSAS2.ValueOfUV(aPoint, Precision::Confusion());

  bool aRes = false;
  try {
    OCC_CATCH_SIGNALS;
    LocalAnalysis_SurfaceContinuity aLocAnal(aSurf1,
                                             aPointOnFace1.X(),
                                             aPointOnFace1.Y(),
                                             aSurf2,
                                             aPointOnFace2.X(),
                                             aPointOnFace2.Y(),
                                             GeomAbs_Shape::GeomAbs_G1, // Order
                                             0.001, // EpsNul
                                             0.001, // EpsC0
                                             0.001, // EpsC1
                                             0.001, // EpsC2
                                             theAngle * M_PI / 180.0); //EpsG1
    aRes = aLocAnal.IsG1();
  }
  catch (Standard_Failure const& anException) {
    theError = "LocalAnalysis_SurfaceContinuity error :";
    theError += anException.GetMessageString();
  }

  return aRes;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Pnt>
  GeomAlgoAPI_ShapeTools::centreOfMass(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if (!theShape) {
    return std::shared_ptr<GeomAPI_Pnt>();
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if (aShape.IsNull()) {
    return std::shared_ptr<GeomAPI_Pnt>();
  }
  gp_Pnt aCentre;
  if (aShape.ShapeType() == TopAbs_VERTEX) {
    aCentre = BRep_Tool::Pnt(TopoDS::Vertex(aShape));
  } else {
    aGProps = props(aShape);
    aCentre = aGProps.CentreOfMass();
  }

  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCentre.X(), aCentre.Y(), aCentre.Z()));
}

//==================================================================================================
double GeomAlgoAPI_ShapeTools::radius(const std::shared_ptr<GeomAPI_Face>& theCylinder)
{
  double aRadius = -1.0;
  if (theCylinder->isCylindrical()) {
    const TopoDS_Shape& aShape = theCylinder->impl<TopoDS_Shape>();
    Handle(Geom_Surface) aSurf = BRep_Tool::Surface(TopoDS::Face(aShape));
    Handle(Geom_CylindricalSurface) aCyl = Handle(Geom_CylindricalSurface)::DownCast(aSurf);
    if (!aCyl.IsNull())
      aRadius = aCyl->Radius();
  }
  return aRadius;
}

//==================================================================================================
namespace {

auto getExtemaDistShape = [](const GeomShapePtr& theShape1,
    const GeomShapePtr& theShape2) -> BRepExtrema_DistShapeShape
{
  const TopoDS_Shape& aShape1 = theShape1->impl<TopoDS_Shape>();
  const TopoDS_Shape& aShape2 = theShape2->impl<TopoDS_Shape>();

  BRepExtrema_DistShapeShape aDist(aShape1, aShape2);
  aDist.Perform();
  return aDist;
};
}

double GeomAlgoAPI_ShapeTools::minimalDistance(const GeomShapePtr& theShape1,
                                               const GeomShapePtr& theShape2)
{
  BRepExtrema_DistShapeShape aDist = getExtemaDistShape(theShape1, theShape2);
  return aDist.IsDone() ? aDist.Value() : Precision::Infinite();
}

double GeomAlgoAPI_ShapeTools::minimalDistance(const GeomShapePtr& theShape1,
                                               const GeomShapePtr& theShape2,
                                               std::array<double, 3> & fromShape1To2)
{
  BRepExtrema_DistShapeShape aDist = getExtemaDistShape(theShape1, theShape2);
  const auto & pt1 = aDist.PointOnShape1(1);
  const auto & pt2 = aDist.PointOnShape2(1) ;
  fromShape1To2[0] = pt2.X() - pt1.X();
  fromShape1To2[1] = pt2.Y() - pt1.Y();
  fromShape1To2[2] = pt2.Z() - pt1.Z();
  return aDist.IsDone() ? aDist.Value() : Precision::Infinite();
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::combineShapes(
  const std::shared_ptr<GeomAPI_Shape> theCompound,
  const GeomAPI_Shape::ShapeType theType,
  ListOfShape& theResuts)
{

  ListOfShape aResCombinedShapes;
  ListOfShape aResFreeShapes;

  GeomShapePtr aResult = theCompound;

  if (!theCompound.get()) {
    return aResult;
  }

  if (theType != GeomAPI_Shape::SHELL && theType != GeomAPI_Shape::COMPSOLID) {
    return aResult;
  }

  TopAbs_ShapeEnum aTS = TopAbs_EDGE;
  TopAbs_ShapeEnum aTA = TopAbs_FACE;
  if (theType == GeomAPI_Shape::COMPSOLID) {
    aTS = TopAbs_FACE;
    aTA = TopAbs_SOLID;
  }

  // map from the resulting shapes to minimal index of the used shape from theCompound list
  std::map<GeomShapePtr, int> anInputOrder;
  // map from ancestors-shapes to the index of shapes in theCompound
  NCollection_DataMap<TopoDS_Shape, int> anAncestorsOrder;

  // Get free shapes.
  int anOrder = 0;
  const TopoDS_Shape& aShapesComp = theCompound->impl<TopoDS_Shape>();
  for (TopoDS_Iterator anIter(aShapesComp); anIter.More(); anIter.Next(), anOrder++) {
    const TopoDS_Shape& aShape = anIter.Value();
    if (aShape.ShapeType() > aTA) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
      aGeomShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
      aResFreeShapes.push_back(aGeomShape);
      anInputOrder[aGeomShape] = anOrder;
    } else {
      for (TopExp_Explorer anExp(aShape, aTA); anExp.More(); anExp.Next()) {
        anAncestorsOrder.Bind(anExp.Current(), anOrder);
      }
    }
  }

  // Map sub-shapes and shapes.
  TopTools_IndexedDataMapOfShapeListOfShape aMapSA;
  TopExp::MapShapesAndAncestors(aShapesComp, aTS, aTA, aMapSA);
  if (aMapSA.IsEmpty()) {
    return aResult;
  }
  theResuts.clear();

  // Get all shapes with common sub-shapes and free shapes.
  NCollection_Map<TopoDS_Shape> aFreeShapes;
  NCollection_Vector<NCollection_Map<TopoDS_Shape>> aShapesWithCommonSubshapes;
  for (TopTools_IndexedDataMapOfShapeListOfShape::Iterator
      anIter(aMapSA); anIter.More(); anIter.Next()) {
    TopTools_ListOfShape& aListOfShape = anIter.ChangeValue();
    if (aListOfShape.IsEmpty()) {
      continue;
    }
    else if (aListOfShape.Size() == 1) {
      const TopoDS_Shape& aF = aListOfShape.First();
      aFreeShapes.Add(aF);
      aListOfShape.Clear();
    } else {
      NCollection_List<TopoDS_Shape> aTempList;
      NCollection_Map<TopoDS_Shape> aTempMap;
      for (TopTools_ListOfShape::Iterator aListIt(aListOfShape); aListIt.More(); aListIt.Next()) {
        aTempList.Append(aListIt.Value());
        aTempMap.Add(aListIt.Value());
        aFreeShapes.Remove(aListIt.Value());
      }
      aListOfShape.Clear();
      for (NCollection_List<TopoDS_Shape>::Iterator
          aTempIter(aTempList); aTempIter.More(); aTempIter.Next()) {
        const TopoDS_Shape& aTempShape = aTempIter.Value();
        for (TopTools_IndexedDataMapOfShapeListOfShape::Iterator
            anIter2(aMapSA); anIter2.More(); anIter2.Next()) {
          TopTools_ListOfShape& aTempListOfShape = anIter2.ChangeValue();
          if (aTempListOfShape.IsEmpty()) {
            continue;
          } else if (aTempListOfShape.Size() == 1 && aTempListOfShape.First() == aTempShape) {
            aTempListOfShape.Clear();
          } else if (aTempListOfShape.Size() > 1) {
            TopTools_ListOfShape::Iterator anIt1(aTempListOfShape);
            for (; anIt1.More(); anIt1.Next()) {
              if (anIt1.Value() == aTempShape) {
                TopTools_ListOfShape::Iterator anIt2(aTempListOfShape);
                for (; anIt2.More(); anIt2.Next())
                {
                  if (anIt2.Value() != anIt1.Value()) {
                    if (aTempMap.Add(anIt2.Value())) {
                      aTempList.Append(anIt2.Value());
                      aFreeShapes.Remove(anIt2.Value());
                    }
                  }
                }
                aTempListOfShape.Clear();
                break;
              }
            }
          }
        }
      }
      aShapesWithCommonSubshapes.Append(aTempMap);
    }
  }

  // Combine shapes with common sub-shapes.
  for (NCollection_Vector<NCollection_Map<TopoDS_Shape>>::Iterator
      anIter(aShapesWithCommonSubshapes); anIter.More(); anIter.Next()) {
    TopoDS_Shell aShell;
    TopoDS_CompSolid aCSolid;
    TopoDS_Builder aBuilder;
    anOrder = -1;
    theType ==
      GeomAPI_Shape::COMPSOLID ? aBuilder.MakeCompSolid(aCSolid) : aBuilder.MakeShell(aShell);
    NCollection_Map<TopoDS_Shape>& aShapesMap = anIter.ChangeValue();
    for (TopExp_Explorer anExp(aShapesComp, aTA); anExp.More(); anExp.Next()) {
      const TopoDS_Shape& aShape = anExp.Current();
      if (aShapesMap.Contains(aShape)) {
        theType ==
          GeomAPI_Shape::COMPSOLID ? aBuilder.Add(aCSolid, aShape) : aBuilder.Add(aShell, aShape);
        aShapesMap.Remove(aShape);
        int aThisOrder = anAncestorsOrder.Find(aShape);
        if (anOrder == -1 || aThisOrder < anOrder)
          anOrder = aThisOrder; // take the minimum order position
      }
    }
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    TopoDS_Shape* aSh = theType == GeomAPI_Shape::COMPSOLID ? new TopoDS_Shape(aCSolid) :
                                                              new TopoDS_Shape(aShell);
    aGeomShape->setImpl<TopoDS_Shape>(aSh);
    aResCombinedShapes.push_back(aGeomShape);
    anInputOrder[aGeomShape] = anOrder;
  }

  // Adding free shapes.
  for (TopExp_Explorer anExp(aShapesComp, aTA); anExp.More(); anExp.Next()) {
    const TopoDS_Shape& aShape = anExp.Current();
    if (aFreeShapes.Contains(aShape)) {
      std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
      aGeomShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
      aResFreeShapes.push_back(aGeomShape);
      anInputOrder[aGeomShape] = anAncestorsOrder.Find(aShape);
    }
  }

  if (aResCombinedShapes.size() == 1 && aResFreeShapes.size() == 0) {
    aResult = aResCombinedShapes.front();
    theResuts.push_back(aResult);
  } else if (aResCombinedShapes.size() == 0 && aResFreeShapes.size() == 1) {
    aResult = aResFreeShapes.front();
    theResuts.push_back(aResult);
  } else {
    TopoDS_Compound aResultComp;
    TopoDS_Builder aBuilder;
    aBuilder.MakeCompound(aResultComp);
    // put to result compound and result list in accordance to the order numbers
    std::map<GeomShapePtr, int>::iterator anInputIter = anInputOrder.begin();
    std::map<int, GeomShapePtr> aNums;
    for (; anInputIter != anInputOrder.end(); anInputIter++)
      aNums[anInputIter->second] = anInputIter->first;
    std::map<int, GeomShapePtr>::iterator aNumsIter = aNums.begin();
    for (; aNumsIter != aNums.end(); aNumsIter++) {
      aBuilder.Add(aResultComp, (aNumsIter->second)->impl<TopoDS_Shape>());
      theResuts.push_back(aNumsIter->second);
    }
    aResult->setImpl(new TopoDS_Shape(aResultComp));
  }

  return aResult;
}

//==================================================================================================
static void addSimpleShapeToList(const TopoDS_Shape& theShape,
                                 NCollection_List<TopoDS_Shape>& theList)
{
  if (theShape.IsNull()) {
    return;
  }

  if (theShape.ShapeType() == TopAbs_COMPOUND) {
    for (TopoDS_Iterator anIt(theShape); anIt.More(); anIt.Next()) {
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

  for (NCollection_List<TopoDS_Shape>::Iterator anIt(theShapes); anIt.More(); anIt.Next()) {
    aBuilder.Add(aCompound, anIt.Value());
  }

  return aCompound;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::groupSharedTopology(
  const std::shared_ptr<GeomAPI_Shape> theCompound)
{
  GeomShapePtr aResult = theCompound;

  if (!theCompound.get()) {
    return aResult;
  }

  TopoDS_Shape anInShape = aResult->impl<TopoDS_Shape>();
  NCollection_List<TopoDS_Shape> anUngroupedShapes, aStillUngroupedShapes;
  addSimpleShapeToList(anInShape, anUngroupedShapes);

  // Iterate over all shapes and find shapes with shared vertices.
  TopTools_ListOfShape allVertices;
  TopTools_DataMapOfShapeListOfShape aVertexShapesMap;
  for (NCollection_List<TopoDS_Shape>::Iterator aShapesIt(anUngroupedShapes);
    aShapesIt.More();
    aShapesIt.Next()) {
    const TopoDS_Shape& aShape = aShapesIt.Value();
    for (TopExp_Explorer aShapeExp(aShape, TopAbs_VERTEX);
      aShapeExp.More();
      aShapeExp.Next()) {
      const TopoDS_Shape& aVertex = aShapeExp.Current();
      if (!aVertexShapesMap.IsBound(aVertex)) {
        NCollection_List<TopoDS_Shape> aList;
        aList.Append(aShape);
        allVertices.Append(aVertex);
        aVertexShapesMap.Bind(aVertex, aList);
      }
      else {
        if (!aVertexShapesMap.ChangeFind(aVertex).Contains(aShape)) {
          aVertexShapesMap.ChangeFind(aVertex).Append(aShape);
        }
      }
    }
  }

  // Iterate over the map and group shapes.
  NCollection_Vector<TopTools_MapOfShape> aGroups; // groups of shapes connected by vertices
  while (!allVertices.IsEmpty()) {
    // Get first group of shapes in map, and then unbind it.
    const TopoDS_Shape& aKey = allVertices.First();
    TopTools_ListOfShape aConnectedShapes = aVertexShapesMap.Find(aKey);
    aVertexShapesMap.UnBind(aKey);
    allVertices.Remove(aKey);
    // Iterate over shapes in this group and add to it shapes from groups in map.
    for (TopTools_ListOfShape::Iterator aConnectedIt(aConnectedShapes);
      aConnectedIt.More(); aConnectedIt.Next()) {
      const TopoDS_Shape& aConnected = aConnectedIt.Value();
      TopTools_ListOfShape aKeysToUnbind;
      for (TopTools_ListOfShape::Iterator aKeysIt(allVertices);
        aKeysIt.More();
        aKeysIt.Next()) {
        const TopTools_ListOfShape& anOtherConnected = aVertexShapesMap(aKeysIt.Value());
        if (!anOtherConnected.Contains(aConnected)) {
          // Other connected group does not contain shape from our connected group
          continue;
        }
        // Other is connected to our, so add them to our connected
        for (TopTools_ListOfShape::Iterator anOtherIt(anOtherConnected);
          anOtherIt.More();
          anOtherIt.Next()) {
          const TopoDS_Shape& aShape = anOtherIt.Value();
          if (!aConnectedShapes.Contains(aShape)) {
            aConnectedShapes.Append(aShape);
          }
        }
        // Save key to unbind from this map.
        aKeysToUnbind.Append(aKeysIt.Value());
      }
      // Unbind groups from map that we added to our group.
      for (TopTools_ListOfShape::Iterator aKeysIt(aKeysToUnbind);
        aKeysIt.More();
        aKeysIt.Next()) {
        aVertexShapesMap.UnBind(aKeysIt.Value());
        allVertices.Remove(aKeysIt.Value());
      }
    }
    // Sort shapes from the most complicated to the simplest ones
    TopTools_MapOfShape aSortedGroup;
    for (int aST = TopAbs_COMPOUND; aST <= TopAbs_SHAPE; ++aST) {
      TopTools_ListOfShape::Iterator anIt(aConnectedShapes);
      while (anIt.More()) {
        if (anIt.Value().ShapeType() == aST) {
          aSortedGroup.Add(anIt.Value());
          aConnectedShapes.Remove(anIt);
        }
        else {
          anIt.Next();
        }
      }
    }
    aGroups.Append(aSortedGroup);
  }

  TopoDS_Compound aCompound;
  BRep_Builder aBuilder;
  aBuilder.MakeCompound(aCompound);
  ListOfShape aSolids;
  for (NCollection_Vector<TopTools_MapOfShape>::Iterator anIt(aGroups); anIt.More(); anIt.Next()) {
    const TopTools_MapOfShape& aGroup = anIt.ChangeValue();
    GeomShapePtr aGeomShape(new GeomAPI_Shape());
    if (aGroup.Size() == 1) {
      TopTools_MapOfShape::Iterator aOneShapeIter(aGroup);
      aGeomShape->setImpl(new TopoDS_Shape(aOneShapeIter.Value()));
    } else {
      // make sub-shapes in the group have order same as in original shape
      TopTools_ListOfShape anOrderedGoup;
      NCollection_List<TopoDS_Shape>::Iterator anUngrouped(anUngroupedShapes);
      for (; anUngrouped.More(); anUngrouped.Next()) {
        if (aGroup.Contains(anUngrouped.Value()))
          anOrderedGoup.Append(anUngrouped.Value());
      }
      aGeomShape->setImpl(new TopoDS_Shape(makeCompound(anOrderedGoup)));
      aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                         GeomAPI_Shape::COMPSOLID,
                                                         aSolids);
    }
    aBuilder.Add(aCompound, aGeomShape->impl<TopoDS_Shape>());
  }

  if (!aCompound.IsNull()) {
    aResult->setImpl(new TopoDS_Shape(aCompound));
  }

  return aResult;
}

//==================================================================================================
bool GeomAlgoAPI_ShapeTools::hasSharedTopology(const ListOfShape& theShapes,
                                               const GeomAPI_Shape::ShapeType theShapeType)
{
  TopTools_IndexedMapOfShape aSubs;
  for (ListOfShape::const_iterator anIt = theShapes.begin(); anIt != theShapes.end(); ++anIt) {
    TopTools_IndexedMapOfShape aCurSubs;
    TopExp::MapShapes((*anIt)->impl<TopoDS_Shape>(), (TopAbs_ShapeEnum)theShapeType, aCurSubs);
    for (TopTools_IndexedMapOfShape::Iterator aSubIt(aCurSubs); aSubIt.More(); aSubIt.Next()) {
      if (aSubs.Contains(aSubIt.Value()))
        return true;
      else
        aSubs.Add(aSubIt.Value());
    }
  }
  return false;
}

//==================================================================================================
std::list<std::shared_ptr<GeomAPI_Pnt> >
  GeomAlgoAPI_ShapeTools::getBoundingBox(const ListOfShape& theShapes, const double theEnlarge)
{
  // Bounding box of all objects.
  Bnd_Box aBndBox;

  // Getting box.
  for (ListOfShape::const_iterator
    anObjectsIt = theShapes.begin(); anObjectsIt != theShapes.end(); anObjectsIt++) {
    const TopoDS_Shape& aShape = (*anObjectsIt)->impl<TopoDS_Shape>();
    BRepBndLib::Add(aShape, aBndBox);
  }

  if (theEnlarge != 0.0) {
    // We enlarge bounding box just to be sure that plane will be large enough to cut all objects.
    aBndBox.Enlarge(theEnlarge);
  }

  Standard_Real aXArr[2] = {aBndBox.CornerMin().X(), aBndBox.CornerMax().X()};
  Standard_Real aYArr[2] = {aBndBox.CornerMin().Y(), aBndBox.CornerMax().Y()};
  Standard_Real aZArr[2] = {aBndBox.CornerMin().Z(), aBndBox.CornerMax().Z()};
  std::list<std::shared_ptr<GeomAPI_Pnt> > aResultPoints;
  for (int i = 0; i < 2; i++) {
    for (int j = 0; j < 2; j++) {
      for (int k = 0; k < 2; k++) {
        std::shared_ptr<GeomAPI_Pnt> aPnt(new GeomAPI_Pnt(aXArr[i], aYArr[j], aZArr[k]));
        aResultPoints.push_back(aPnt);
      }
    }
  }

  return aResultPoints;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Face> GeomAlgoAPI_ShapeTools::fitPlaneToBox(
  const std::shared_ptr<GeomAPI_Shape> thePlane,
  const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints)
{
  std::shared_ptr<GeomAPI_Face> aResultFace;

  if (!thePlane.get()) {
    return aResultFace;
  }

  const TopoDS_Shape& aShape = thePlane->impl<TopoDS_Shape>();
  if (aShape.ShapeType() != TopAbs_FACE) {
    return aResultFace;
  }

  TopoDS_Face aFace = TopoDS::Face(aShape);
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);
  if (aSurf.IsNull()) {
    return aResultFace;
  }

  GeomLib_IsPlanarSurface isPlanar(aSurf);
  if (!isPlanar.IsPlanar()) {
    return aResultFace;
  }

  if (thePoints.size() != 8) {
    return aResultFace;
  }

  const gp_Pln& aFacePln = isPlanar.Plan();
  Handle(Geom_Plane) aFacePlane = new Geom_Plane(aFacePln);
  IntAna_Quadric aQuadric(aFacePln);
  Standard_Real UMin, UMax, VMin, VMax;
  UMin = UMax = VMin = VMax = 0;
  for (std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator
       aPointsIt = thePoints.begin(); aPointsIt != thePoints.end(); aPointsIt++) {
    const gp_Pnt& aPnt = (*aPointsIt)->impl<gp_Pnt>();
    gp_Lin aLin(aPnt, aFacePln.Axis().Direction());
    IntAna_IntConicQuad anIntAna(aLin, aQuadric);
    const gp_Pnt& aPntOnFace = anIntAna.Point(1);
    Standard_Real aPntU(0), aPntV(0);
    GeomLib_Tool::Parameters(aFacePlane, aPntOnFace, Precision::Confusion(), aPntU, aPntV);
    if (aPntU < UMin) UMin = aPntU;
    if (aPntU > UMax) UMax = aPntU;
    if (aPntV < VMin) VMin = aPntV;
    if (aPntV > VMax) VMax = aPntV;
  }
  aResultFace.reset(new GeomAPI_Face());
  aResultFace->setImpl(new TopoDS_Face(BRepLib_MakeFace(aFacePln, UMin, UMax, VMin, VMax).Face()));

  return aResultFace;
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::findBounds(const std::shared_ptr<GeomAPI_Shape> theShape,
                                        std::shared_ptr<GeomAPI_Vertex>& theV1,
                                        std::shared_ptr<GeomAPI_Vertex>& theV2)
{
  static GeomVertexPtr aVertex;
  if (!aVertex) {
    aVertex = GeomVertexPtr(new GeomAPI_Vertex);
    aVertex->setImpl(new TopoDS_Vertex());
  }

  theV1 = aVertex;
  theV2 = aVertex;

  if (theShape) {
    const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
    TopoDS_Vertex aV1, aV2;
    ShapeAnalysis::FindBounds(aShape, aV1, aV2);

    std::shared_ptr<GeomAPI_Vertex> aGeomV1(new GeomAPI_Vertex()), aGeomV2(new GeomAPI_Vertex());
    aGeomV1->setImpl(new TopoDS_Vertex(aV1));
    aGeomV2->setImpl(new TopoDS_Vertex(aV2));
    theV1 = aGeomV1;
    theV2 = aGeomV2;
  }
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
  for (ListOfShape::const_iterator anIt = theWires.cbegin();
      anIt != theWires.cend();
      ++anIt) {
    TopoDS_Wire aWire = TopoDS::Wire((*anIt)->impl<TopoDS_Shape>());
    aFRestrictor.Add(aWire);
  }

  aFRestrictor.Perform();

  if (!aFRestrictor.IsDone()) {
    return;
  }

  for (; aFRestrictor.More(); aFRestrictor.Next()) {
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

  for (ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    aBuilder.Add(aCompound, (*anIt)->impl<TopoDS_Shape>());
  }
  BRepBuilderAPI_FindPlane aFindPlane(aCompound);

  if (aFindPlane.Found() != Standard_True) {
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
bool GeomAlgoAPI_ShapeTools::isSubShapeInsideShape(
  const std::shared_ptr<GeomAPI_Shape> theSubShape,
  const std::shared_ptr<GeomAPI_Shape> theBaseShape)
{
  if (!theSubShape.get() || !theBaseShape.get()) {
    return false;
  }

  const TopoDS_Shape& aSubShape = theSubShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aBaseShape = theBaseShape->impl<TopoDS_Shape>();

  if (aSubShape.ShapeType() == TopAbs_VERTEX) {
    // If sub-shape is a vertex check distance to shape. If it is <= Precision::Confusion() then OK.
    BRepExtrema_DistShapeShape aDist(aBaseShape, aSubShape);
    aDist.Perform();
    if (!aDist.IsDone() || aDist.Value() > Precision::Confusion()) {
      return false;
    }
  } else if (aSubShape.ShapeType() == TopAbs_EDGE) {
    if (aBaseShape.ShapeType() == TopAbs_FACE) {
      // Check that edge is on face surface.
      TopoDS_Face aFace = TopoDS::Face(aBaseShape);
      TopoDS_Edge anEdge = TopoDS::Edge(aSubShape);
      BRepLib_CheckCurveOnSurface aCheck(anEdge, aFace);
      aCheck.Perform();
      if (!aCheck.IsDone() || aCheck.MaxDistance() > Precision::Confusion()) {
        return false;
      }

      // Check intersections.
      TopoDS_Vertex aV1, aV2;
      ShapeAnalysis::FindBounds(anEdge, aV1, aV2);
      gp_Pnt aPnt1 = BRep_Tool::Pnt(aV1);
      gp_Pnt aPnt2 = BRep_Tool::Pnt(aV2);
      for (TopExp_Explorer anExp(aBaseShape, TopAbs_EDGE); anExp.More(); anExp.Next()) {
        const TopoDS_Shape& anEdgeOnFace = anExp.Current();
        BRepExtrema_DistShapeShape aDist(anEdgeOnFace, anEdge);
        aDist.Perform();
        if (aDist.IsDone() && aDist.Value() <= Precision::Confusion()) {
          // Edge intersect face bound. Check that it is not on edge begin or end.
          for (Standard_Integer anIndex = 1; anIndex <= aDist.NbSolution(); ++anIndex) {
            gp_Pnt aPntOnSubShape = aDist.PointOnShape2(anIndex);
            if (aPntOnSubShape.Distance(aPnt1) > Precision::Confusion()
                && aPntOnSubShape.Distance(aPnt2) > Precision::Confusion()) {
              return false;
            }
          }
        }
      }

      // No intersections found. Edge is inside or outside face. Check it.
      BRepAdaptor_Curve aCurveAdaptor(anEdge);
      gp_Pnt aPointToCheck =
        aCurveAdaptor.Value((aCurveAdaptor.FirstParameter() +
                              aCurveAdaptor.LastParameter()) / 2.0);
      Handle(Geom_Surface) aSurface = BRep_Tool::Surface(aFace);
      ShapeAnalysis_Surface aSAS(aSurface);
      gp_Pnt2d aPointOnFace = aSAS.ValueOfUV(aPointToCheck, Precision::Confusion());
      BRepTopAdaptor_FClass2d aFClass2d(aFace, Precision::Confusion());
      if (aFClass2d.Perform(aPointOnFace) == TopAbs_OUT) {
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
  if (!theShape.get()) {
    return false;
  }

  BRepCheck_Analyzer aChecker(theShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape>
  GeomAlgoAPI_ShapeTools::getFaceOuterWire(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  GeomShapePtr anOuterWire;

  if (!theFace.get() || !theFace->isFace()) {
    return anOuterWire;
  }

  TopoDS_Face aFace = TopoDS::Face(theFace->impl<TopoDS_Shape>());
  TopoDS_Wire aWire = BRepTools::OuterWire(aFace);

  anOuterWire.reset(new GeomAPI_Shape());
  anOuterWire->setImpl(new TopoDS_Shape(aWire));

  return anOuterWire;
}

//==================================================================================================
static bool boundaryOfEdge(const std::shared_ptr<GeomAPI_Edge> theEdge,
                          const std::shared_ptr<GeomAPI_Vertex> theVertex,
                          double& theParam)
{
  GeomPointPtr aPoint = theVertex->point();
  GeomPointPtr aFirstPnt = theEdge->firstPoint();
  double aFirstPntTol = theEdge->firstPointTolerance();
  GeomPointPtr aLastPnt = theEdge->lastPoint();
  double aLastPntTol = theEdge->lastPointTolerance();

  double aFirst, aLast;
  theEdge->getRange(aFirst, aLast);

  bool isFirst = aPoint->distance(aFirstPnt) <= aFirstPntTol;
  bool isLast = aPoint->distance(aLastPnt) <= aLastPntTol;
  if (isFirst)
    theParam = aFirst;
  else if (isLast)
    theParam = aLast;

  return isFirst != isLast;
}

bool GeomAlgoAPI_ShapeTools::isTangent(const std::shared_ptr<GeomAPI_Edge> theEdge1,
                                       const std::shared_ptr<GeomAPI_Edge> theEdge2,
                                       const std::shared_ptr<GeomAPI_Vertex> theTgPoint)
{
  double aParE1 = 0, aParE2 = 0;
  if (!boundaryOfEdge(theEdge1, theTgPoint, aParE1) ||
      !boundaryOfEdge(theEdge2, theTgPoint, aParE2))
    return false;

  BRepAdaptor_Curve aC1(theEdge1->impl<TopoDS_Edge>());
  BRepAdaptor_Curve aC2(theEdge2->impl<TopoDS_Edge>());
  return BRepLProp::Continuity(aC1, aC2, aParE1, aParE2) >= GeomAbs_G1;
}

//==================================================================================================
bool GeomAlgoAPI_ShapeTools::isParallel(const std::shared_ptr<GeomAPI_Edge> theEdge,
                                        const std::shared_ptr<GeomAPI_Face> theFace)
{
  if (!theEdge.get() || !theFace.get()) {
    return false;
  }

  TopoDS_Edge anEdge = TopoDS::Edge(theEdge->impl<TopoDS_Shape>());
  TopoDS_Face aFace  = TopoDS::Face(theFace->impl<TopoDS_Shape>());

  BRepExtrema_ExtCF anExt(anEdge, aFace);
  return anExt.IsParallel() == Standard_True;
}

//==================================================================================================
std::list<std::shared_ptr<GeomAPI_Vertex> > GeomAlgoAPI_ShapeTools::intersect(
  const std::shared_ptr<GeomAPI_Edge> theEdge, const std::shared_ptr<GeomAPI_Face> theFace)
{
  std::list<std::shared_ptr<GeomAPI_Vertex> > aResult;
  if (!theEdge.get() || !theFace.get()) {
    return aResult;
  }

  TopoDS_Edge anEdge = TopoDS::Edge(theEdge->impl<TopoDS_Shape>());
  double aFirstOnCurve, aLastOnCurve;
  Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirstOnCurve, aLastOnCurve);

  TopoDS_Face aFace  = TopoDS::Face(theFace->impl<TopoDS_Shape>());
  Handle(Geom_Surface) aSurf = BRep_Tool::Surface(aFace);

  GeomAPI_IntCS anIntAlgo(aCurve, aSurf);
  if (!anIntAlgo.IsDone())
    return aResult;
  // searching for points-intersection
  for (int anIntNum = 1; anIntNum <= anIntAlgo.NbPoints() + anIntAlgo.NbSegments(); anIntNum++) {
    gp_Pnt anInt;
    if (anIntNum <= anIntAlgo.NbPoints()) {
      anInt = anIntAlgo.Point(anIntNum);
    } else { // take the middle point on the segment of the intersection
      Handle(Geom_Curve) anIntCurve = anIntAlgo.Segment(anIntNum - anIntAlgo.NbPoints());
      anIntCurve->D0((anIntCurve->FirstParameter() + anIntCurve->LastParameter()) / 2., anInt);
    }
    aResult.push_back(std::shared_ptr<GeomAPI_Vertex>(
      new GeomAPI_Vertex(anInt.X(), anInt.Y(), anInt.Z())));
  }
  return aResult;
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::splitShape(const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                      const GeomAlgoAPI_ShapeTools::PointToRefsMap& thePointsInfo,
                                      std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes)
{
  // to split shape at least one point should be presented in the points container
  if (thePointsInfo.empty())
    return;

    // General Fuse to split edge by vertices
  BOPAlgo_Builder aBOP;
  TopoDS_Edge aBaseEdge = theBaseShape->impl<TopoDS_Edge>();
  // Rebuild closed edge to place vertex to one of split points.
  // This will prevent edge to be split on same vertex.
  if (BRep_Tool::IsClosed(aBaseEdge))
  {
    Standard_Real aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aBaseEdge, aFirst, aLast);

    PointToRefsMap::const_iterator aPIt = thePointsInfo.begin();
    std::shared_ptr<GeomAPI_Pnt> aPnt = aPIt->first;
    gp_Pnt aPoint(aPnt->x(), aPnt->y(), aPnt->z());

    TopAbs_Orientation anOrientation = aBaseEdge.Orientation();
    aBaseEdge = BRepBuilderAPI_MakeEdge(aCurve, aPoint, aPoint).Edge();
    aBaseEdge.Orientation(anOrientation);
  }
  aBOP.AddArgument(aBaseEdge);

  PointToRefsMap::const_iterator aPIt = thePointsInfo.begin();
  for (; aPIt != thePointsInfo.end(); ++aPIt) {
    std::shared_ptr<GeomAPI_Pnt> aPnt = aPIt->first;
    TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(gp_Pnt(aPnt->x(), aPnt->y(), aPnt->z()));
    aBOP.AddArgument(aV);
  }

  aBOP.Perform();
  if (aBOP.HasErrors())
    return;

  // Collect splits
  const TopTools_ListOfShape& aSplits = aBOP.Modified(aBaseEdge);
  TopTools_ListIteratorOfListOfShape anIt(aSplits);
  for (; anIt.More(); anIt.Next()) {
    std::shared_ptr<GeomAPI_Shape> anEdge(new GeomAPI_Shape);
    anEdge->setImpl(new TopoDS_Shape(anIt.Value()));
    theShapes.insert(anEdge);
  }
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::splitShape_p(const std::shared_ptr<GeomAPI_Shape>& theBaseShape,
                                          const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                          std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes)
{
  // General Fuse to split edge by vertices
  BOPAlgo_Builder aBOP;
  TopoDS_Edge aBaseEdge = theBaseShape->impl<TopoDS_Edge>();
  // Rebuild closed edge to place vertex to one of split points.
  // This will prevent edge to be split on seam vertex.
  if (BRep_Tool::IsClosed(aBaseEdge))
  {
    Standard_Real aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aBaseEdge, aFirst, aLast);

    std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator aPIt = thePoints.begin();
    gp_Pnt aPoint((*aPIt)->x(), (*aPIt)->y(), (*aPIt)->z());

    TopAbs_Orientation anOrientation = aBaseEdge.Orientation();
    aBaseEdge = BRepBuilderAPI_MakeEdge(aCurve, aPoint, aPoint).Edge();
    aBaseEdge.Orientation(anOrientation);
  }
  aBOP.AddArgument(aBaseEdge);

  std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator aPtIt = thePoints.begin();
  for (; aPtIt != thePoints.end(); ++aPtIt) {
    std::shared_ptr<GeomAPI_Pnt> aPnt = *aPtIt;
    TopoDS_Vertex aV = BRepBuilderAPI_MakeVertex(gp_Pnt(aPnt->x(), aPnt->y(), aPnt->z()));
    aBOP.AddArgument(aV);
  }

  aBOP.Perform();
  if (aBOP.HasErrors())
    return;

  // Collect splits
  const TopTools_ListOfShape& aSplits = aBOP.Modified(aBaseEdge);
  TopTools_ListIteratorOfListOfShape anIt(aSplits);
  for (; anIt.More(); anIt.Next()) {
    std::shared_ptr<GeomAPI_Shape> anEdge(new GeomAPI_Shape);
    anEdge->setImpl(new TopoDS_Shape(anIt.Value()));
    theShapes.insert(anEdge);
  }
}

//==================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::findShape(
                                  const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                  const std::set<std::shared_ptr<GeomAPI_Shape> >& theShapes)
{
  std::shared_ptr<GeomAPI_Shape> aResultShape;

  if (thePoints.size() == 2) {
    std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator aPntIt = thePoints.begin();
    std::shared_ptr<GeomAPI_Pnt> aFirstPoint = *aPntIt;
    aPntIt++;
    std::shared_ptr<GeomAPI_Pnt> aLastPoint = *aPntIt;

    std::set<std::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = theShapes.begin(),
                                                              aLast = theShapes.end();
    for (; anIt != aLast; anIt++) {
      GeomShapePtr aShape = *anIt;
      std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge(aShape));
      if (anEdge.get()) {
        std::shared_ptr<GeomAPI_Pnt> anEdgeFirstPoint = anEdge->firstPoint();
        std::shared_ptr<GeomAPI_Pnt> anEdgeLastPoint = anEdge->lastPoint();
        if (anEdgeFirstPoint->isEqual(aFirstPoint) &&
            anEdgeLastPoint->isEqual(aLastPoint))
            aResultShape = aShape;
      }
    }
  }

  return aResultShape;
}

//==================================================================================================
#ifdef FEATURE_MULTIROTATION_TWO_DIRECTIONS
std::shared_ptr<GeomAPI_Dir> GeomAlgoAPI_ShapeTools::buildDirFromAxisAndShape(
                                    const std::shared_ptr<GeomAPI_Shape> theBaseShape,
                                    const std::shared_ptr<GeomAPI_Ax1> theAxis)
{
  gp_Pnt aCentreOfMassPoint =
    GeomAlgoAPI_ShapeTools::centreOfMass(theBaseShape)->impl<gp_Pnt>();
  Handle(Geom_Line) aLine = new Geom_Line(theAxis->impl<gp_Ax1>());
  GeomAPI_ProjectPointOnCurve aPrjTool(aCentreOfMassPoint, aLine);
  gp_Pnt aPoint = aPrjTool.NearestPoint();

  std::shared_ptr<GeomAPI_Dir> aDir(new GeomAPI_Dir(aCentreOfMassPoint.X()-aPoint.X(),
                                                    aCentreOfMassPoint.Y()-aPoint.Y(),
                                                    aCentreOfMassPoint.Z()-aPoint.Z()));
  return aDir;
}
#endif

//==================================================================================================
static TopoDS_Wire fixParametricGaps(const TopoDS_Wire& theWire)
{
  TopoDS_Wire aFixedWire;
  Handle(Geom_Curve) aPrevCurve;
  double aPrevLastParam = -Precision::Infinite();

  BRep_Builder aBuilder;
  aBuilder.MakeWire(aFixedWire);

  BRepTools_WireExplorer aWExp(theWire);
  for (; aWExp.More(); aWExp.Next()) {
    TopoDS_Edge anEdge = aWExp.Current();
    double aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
    if (aCurve == aPrevCurve && Abs(aFirst - aPrevLastParam) > Precision::Confusion()) {
      // if parametric gap occurs, create new edge based on the copied curve
      aCurve = Handle(Geom_Curve)::DownCast(aCurve->Copy());
      TopoDS_Vertex aV1, aV2;
      TopExp::Vertices(anEdge, aV1, aV2);
      anEdge = TopoDS::Edge(anEdge.EmptyCopied());
      aBuilder.UpdateEdge(anEdge, aCurve, BRep_Tool::Tolerance(anEdge));
      aBuilder.Add(anEdge, aV1);
      aBuilder.Add(anEdge, aV2);
    }

    aBuilder.Add(aFixedWire, anEdge);

    aPrevCurve = aCurve;
    aPrevLastParam = aLast;
  }

  return aFixedWire;
}

//==================================================================================================
std::shared_ptr<GeomAPI_Edge> GeomAlgoAPI_ShapeTools::wireToEdge(
      const std::shared_ptr<GeomAPI_Wire>& theWire)
{
  GeomEdgePtr anEdge;
  if (theWire) {
    TopoDS_Wire aWire = theWire->impl<TopoDS_Wire>();
    BRepTools_WireExplorer aWExp(aWire);
    TopoDS_Edge aNewEdge = aWExp.Current();
    aWExp.Next();
    if (aWExp.More()) {
      // Workaround for the closed wire to avoid jumping of its start point:
      // split this wire for two parts, convert them to edges, then compose together
      if (BRep_Tool::IsClosed(aWire)) {
        aWire = TopoDS::Wire(BRepBuilderAPI_Copy(aWire).Shape());
        aWExp.Init(aWire);
        aNewEdge = aWExp.Current();

        BRep_Builder().Remove(aWire, aNewEdge);
        GeomWirePtr aSplitWire(new GeomAPI_Wire);
        aSplitWire->setImpl(new TopoDS_Wire(aWire));
        GeomEdgePtr aMergedEdge = wireToEdge(aSplitWire);

        aWire = BRepBuilderAPI_MakeWire(aNewEdge, aMergedEdge->impl<TopoDS_Edge>());
      }

      // Workaround: when concatenate a wire consisting of two edges based on the same B-spline
      // curve (non-periodic, but having equal start and end points), first of which is placed
      // at the end on the curve and second is placed at the start, this workaround copies
      // second curve to avoid treating these edges as a single curve by setting trim parameters.
      aWire = fixParametricGaps(aWire);
      aWire = BRepAlgo::ConcatenateWire(aWire, GeomAbs_G1); // join smooth parts of wire
      aNewEdge = BRepAlgo::ConcatenateWireC0(aWire); // join C0 parts of wire

      // Reapproximate the result edge to have the parameter equal to curvilinear abscissa.
      static const int THE_MAX_DEGREE = 14;
      static const int THE_MAX_INTERVALS = 32;
      double aFirst, aLast;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(aNewEdge, aFirst, aLast);
      Handle(GeomAdaptor_HCurve) aHCurve = new GeomAdaptor_HCurve(aCurve);
      Approx_CurvilinearParameter anApprox(aHCurve, Precision::Confusion(), aCurve->Continuity(),
                                           THE_MAX_DEGREE, THE_MAX_INTERVALS);
      if (anApprox.HasResult()) {
        Handle(Geom_BSplineCurve) aNewCurve = anApprox.Curve3d();
        TColStd_Array1OfReal aKnots = aNewCurve->Knots();
        BSplCLib::Reparametrize(aFirst, aLast, aKnots);
        aNewCurve->SetKnots(aKnots);
        BRep_Builder().UpdateEdge(aNewEdge, aNewCurve, BRep_Tool::Tolerance(aNewEdge));
      }
    }
    anEdge = GeomEdgePtr(new GeomAPI_Edge);
    anEdge->setImpl(new TopoDS_Edge(aNewEdge));
  }
  return anEdge;
}

//==================================================================================================
ListOfShape GeomAlgoAPI_ShapeTools::getLowLevelSubShapes(const GeomShapePtr& theShape)
{
  ListOfShape aSubShapes;

  if (!theShape->isCompound() && !theShape->isCompSolid() &&
      !theShape->isShell() && !theShape->isWire()) {
    return aSubShapes;
  }

  for (GeomAPI_ShapeIterator anIt(theShape); anIt.more(); anIt.next()) {
    GeomShapePtr aSubShape = anIt.current();
    if (aSubShape->isVertex() || aSubShape->isEdge() ||
        aSubShape->isFace() || aSubShape->isSolid()) {
      aSubShapes.push_back(aSubShape);
    } else {
      aSubShapes.splice(aSubShapes.end(), getLowLevelSubShapes(aSubShape));
    }
  }

  return aSubShapes;
}

//==================================================================================================
static void getMinMaxPointsOnLine(const std::list<std::shared_ptr<GeomAPI_Pnt> >& thePoints,
                                  const gp_Dir theDir,
                                  double& theMin, double& theMax)
{
  theMin = RealLast();
  theMax = RealFirst();
  // Project bounding points on theDir
  for (std::list<std::shared_ptr<GeomAPI_Pnt> >::const_iterator
         aPointsIt = thePoints.begin(); aPointsIt != thePoints.end(); aPointsIt++) {
    const gp_Pnt& aPnt = (*aPointsIt)->impl<gp_Pnt>();
    gp_Dir aPntDir (aPnt.XYZ());
    Standard_Real proj = (theDir*aPntDir) * aPnt.XYZ().Modulus();
    if (proj < theMin) theMin = proj;
    if (proj > theMax) theMax = proj;
  }
}

//==================================================================================================
void GeomAlgoAPI_ShapeTools::computeThroughAll(const ListOfShape& theObjects,
                                               const ListOfShape& theBaseShapes,
                                               const std::shared_ptr<GeomAPI_Dir> theDir,
                                               double& theToSize, double& theFromSize)
{
  // Bounding box of objects
  std::list<std::shared_ptr<GeomAPI_Pnt> > aBndObjs =
      GeomAlgoAPI_ShapeTools::getBoundingBox(theObjects);
  if (aBndObjs.size() != 8) {
    return;
  }

  // the value to enlarge the bounding box of each object to make the extruded shape
  // a little bit larger than overall objects to get the correct result of Boolean CUT operation
  double anEnlargement = 0.1 * aBndObjs.front()->distance(aBndObjs.back());

  // Prism direction
  if (theDir.get()) {
    // One direction for all prisms
    gp_Dir aDir = theDir->impl<gp_Dir>();

    // Bounding box of the base
    std::list<std::shared_ptr<GeomAPI_Pnt> > aBndBases =
        GeomAlgoAPI_ShapeTools::getBoundingBox(theBaseShapes);
    if (aBndBases.size() != 8) {
      return;
    }

    // Objects bounds
    Standard_Real lowBnd, upperBnd;
    getMinMaxPointsOnLine(aBndObjs, aDir, lowBnd, upperBnd);

    // Base bounds
    Standard_Real lowBase, upperBase;
    getMinMaxPointsOnLine(aBndBases, aDir, lowBase, upperBase);

    // ----------.-----.---------.--------------.-----------> theDir
    //       lowBnd   lowBase   upperBase    upperBnd

    theToSize = upperBnd - lowBase;
    theFromSize = upperBase - lowBnd;
  } else {
    // Direction is a normal to each base shape (different normals to bases)
    // So we calculate own sizes for each base shape
    theToSize = 0.0;
    theFromSize = 0.0;

    for (ListOfShape::const_iterator anIt = theBaseShapes.begin();
         anIt != theBaseShapes.end(); ++anIt) {
      const GeomShapePtr& aBaseShape_i = (*anIt);
      ListOfShape aBaseShapes_i;
      aBaseShapes_i.push_back(aBaseShape_i);

      // Bounding box of the base
      std::list<std::shared_ptr<GeomAPI_Pnt> > aBndBases =
          GeomAlgoAPI_ShapeTools::getBoundingBox(aBaseShapes_i, anEnlargement);
      if (aBndBases.size() != 8) {
        return;
      }

      // Direction (normal to aBaseShapes_i)
      // Code like in GeomAlgoAPI_Prism
      gp_Dir aDir;
      const TopoDS_Shape& aBaseShape = aBaseShape_i->impl<TopoDS_Shape>();
      BRepBuilderAPI_FindPlane aFindPlane(aBaseShape);
      if (aFindPlane.Found() == Standard_True) {
        Handle(Geom_Plane) aPlane;
        if (aBaseShape.ShapeType() == TopAbs_FACE || aBaseShape.ShapeType() == TopAbs_SHELL) {
          TopExp_Explorer anExp(aBaseShape, TopAbs_FACE);
          const TopoDS_Shape& aFace = anExp.Current();
          Handle(Geom_Surface) aSurface = BRep_Tool::Surface(TopoDS::Face(aFace));
          if (aSurface->DynamicType() == STANDARD_TYPE(Geom_RectangularTrimmedSurface)) {
            Handle(Geom_RectangularTrimmedSurface) aTrimSurface =
              Handle(Geom_RectangularTrimmedSurface)::DownCast(aSurface);
            aSurface = aTrimSurface->BasisSurface();
          }
          if (aSurface->DynamicType() != STANDARD_TYPE(Geom_Plane)) {
            return;
          }
          aPlane = Handle(Geom_Plane)::DownCast(aSurface);
        } else {
          aPlane = aFindPlane.Plane();
        }
        aDir = aPlane->Axis().Direction();
      } else {
        return;
      }

      // Objects bounds
      Standard_Real lowBnd, upperBnd;
      getMinMaxPointsOnLine(aBndObjs, aDir, lowBnd, upperBnd);

      // Base bounds
      Standard_Real lowBase, upperBase;
      getMinMaxPointsOnLine(aBndBases, aDir, lowBase, upperBase);

      // ----------.-----.---------.--------------.-----------> theDir
      //       lowBnd   lowBase   upperBase    upperBnd

      double aToSize_i = upperBnd - lowBase;
      double aFromSize_i = upperBase - lowBnd;

      if (aToSize_i > theToSize) theToSize = aToSize_i;
      if (aFromSize_i > theFromSize) theFromSize = aFromSize_i;
    }
  }
}
