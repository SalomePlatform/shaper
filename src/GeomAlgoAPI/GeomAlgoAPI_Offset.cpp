// Copyright (C) 2019-2026  CEA, EDF
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

#include "GeomAlgoAPI_Offset.h"

#include <GeomAPI_Pln.h>

#include <BRepOffsetAPI_MakeOffsetShape.hxx>
#include <BRepOffsetAPI_MakeOffset.hxx>

#include <BRepBuilderAPI_MakeWire.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

GeomAlgoAPI_Offset::GeomAlgoAPI_Offset(const GeomShapePtr& theShape,
                                       const double        theOffsetValue)
{
  buildSimple(theShape, theOffsetValue);
}

GeomAlgoAPI_Offset::GeomAlgoAPI_Offset(const GeomShapePtr& theShape,
                                       const double        theOffsetValue,
                                       const bool          isPipeJoint)
{
  buildByJoin(theShape, theOffsetValue, isPipeJoint);
}

GeomAlgoAPI_Offset::GeomAlgoAPI_Offset(const GeomShapePtr& theShape,
                                       const ListOfShape&  theFaces,
                                       const double        theOffsetValue)
{
  buildPartial(theShape, theFaces, theOffsetValue);
}

GeomAlgoAPI_Offset::GeomAlgoAPI_Offset(const GeomPlanePtr& thePlane,
                                       const GeomShapePtr& theEdgeOrWire,
                                       const double theOffsetValue,
                                       const GeomAlgoAPI_OffsetJoint theJoint,
                                       const bool theIsApprox)
{
  build2d(thePlane, theEdgeOrWire, theOffsetValue, theJoint, theIsApprox);
}

void GeomAlgoAPI_Offset::generated(const GeomShapePtr theOldShape,
                                   ListOfShape& theNewShapes)
{
  try {
    GeomAlgoAPI_MakeShape::generated(theOldShape, theNewShapes);
  } catch(...) {
    // nothing is generated
  }
}

void GeomAlgoAPI_Offset::buildSimple(const GeomShapePtr& theShape,
                                     const double        theOffsetValue)
{
  BRepOffsetAPI_MakeOffsetShape* anOffsetAlgo = new BRepOffsetAPI_MakeOffsetShape;
  anOffsetAlgo->PerformBySimple(theShape->impl<TopoDS_Shape>(), theOffsetValue);
  setImpl(anOffsetAlgo);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if (anOffsetAlgo->IsDone()) {
    const TopoDS_Shape& aResult = anOffsetAlgo->Shape();
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aResult));
    setShape(aShape);
    setDone(true);
  }
}

void GeomAlgoAPI_Offset::buildByJoin(const GeomShapePtr& theShape,
                                     const double        theOffsetValue,
                                     const bool          isPipeJoint)
{
  Standard_Real aTol = Precision::Confusion();
  BRepOffset_Mode aMode = BRepOffset_Skin;
  Standard_Boolean anIntersection = Standard_False;
  Standard_Boolean aSelfInter = Standard_False;

  BRepOffsetAPI_MakeOffsetShape* anOffsetAlgo = new BRepOffsetAPI_MakeOffsetShape;
  anOffsetAlgo->PerformByJoin(theShape->impl<TopoDS_Shape>(),
                              theOffsetValue,
                              aTol,
                              aMode,
                              anIntersection,
                              aSelfInter,
                              isPipeJoint ? GeomAbs_Arc : GeomAbs_Intersection);
  setImpl(anOffsetAlgo);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if (anOffsetAlgo->IsDone()) {
    const TopoDS_Shape& aResult = anOffsetAlgo->Shape();
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aResult));
    setShape(aShape);
    setDone(true);
  }
}

void GeomAlgoAPI_Offset::buildPartial(const GeomShapePtr& theShape,
                                      const ListOfShape&  theFaces,
                                      const double        theOffsetValue)
{
  if (theFaces.empty())
    return;

  TopoDS_Shape aShapeBase = theShape->impl<TopoDS_Shape>();

  Standard_Real aTol = Precision::Confusion();
  BRepOffset_Mode aMode = BRepOffset_Skin;
  Standard_Boolean anIntersection = Standard_False;
  Standard_Boolean aSelfInter = Standard_False;

  BRepOffset_MakeOffset* aMakeOffset = new BRepOffset_MakeOffset;
  aMakeOffset->Initialize(aShapeBase,
                          theOffsetValue, // set offset on all faces to anOffset
                          aTol,
                          aMode,
                          anIntersection,
                          aSelfInter,
                          GeomAbs_Intersection,
                          Standard_False);

  // put selected faces into a map
  TopTools_MapOfShape aMapFaces;
  for (ListOfShape::const_iterator anIt = theFaces.begin();
       anIt != theFaces.end(); ++anIt) {
    if ((*anIt)->isFace())
      aMapFaces.Add((*anIt)->impl<TopoDS_Shape>());
  }

  // set offset on non-selected faces to zero
  TopExp_Explorer anExp (aShapeBase, TopAbs_FACE);
  for (; anExp.More(); anExp.Next()) {
    const TopoDS_Shape &aFace = anExp.Current();
    if (!aMapFaces.Contains(aFace)) {
      aMakeOffset->SetOffsetOnFace(TopoDS::Face(aFace), 0.0);
    }
  }

  // perform offset operation
  aMakeOffset->MakeOffsetShape();

  setImpl(aMakeOffset);
  setBuilderType(OCCT_BRepOffset_MakeOffset);

  if (aMakeOffset->IsDone()) {
    const TopoDS_Shape& aResult = aMakeOffset->Shape();
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aResult));
    setShape(aShape);
    setDone(true);
  }
}

void GeomAlgoAPI_Offset::build2d(const GeomPlanePtr& thePlane,
                                 const GeomShapePtr& theEdgeOrWire,
                                 const double theOffsetValue,
                                 const GeomAlgoAPI_OffsetJoint theJoint,
                                 const bool theIsApprox)
{
  // 1. Make wire from edge, if need
  TopoDS_Wire aWire;
  TopoDS_Shape anEdgeOrWire = theEdgeOrWire->impl<TopoDS_Shape>();
  if (anEdgeOrWire.ShapeType() == TopAbs_WIRE) {
    aWire = TopoDS::Wire(anEdgeOrWire);
  } else {
    if (anEdgeOrWire.ShapeType() == TopAbs_EDGE) {
      BRepBuilderAPI_MakeWire aWireBuilder;
      aWireBuilder.Add(TopoDS::Edge(anEdgeOrWire));
      if (aWireBuilder.IsDone()) {
        aWire = aWireBuilder.Wire();
      }
    }
  }
  if (aWire.IsNull())
    return;

  // 2. Make invalid face to pass it in Offset algorithm
  BRepBuilderAPI_MakeFace aFaceBuilder (thePlane->impl<gp_Pln>(), aWire);
  const TopoDS_Face& aFace = aFaceBuilder.Face();

  // 3. Make Offset
  BRepOffsetAPI_MakeOffset* aParal = new BRepOffsetAPI_MakeOffset;
  setImpl(aParal);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  // Joint type
  GeomAbs_JoinType aJoin = GeomAbs_Arc; // default mode, corresponding to KeepDistance
  if (theJoint == GeomAlgoAPI_OffsetJoint::Lines)
    aJoin = GeomAbs_Intersection;
  // for GeomAlgoAPI_OffsetJoint::Arcs do the same as for KeepDistance

  Standard_Boolean isOpenResult = !aWire.Closed();
  aParal->Init(aFace, aJoin, isOpenResult);
  aParal->SetApprox(theIsApprox);
  aParal->Perform(theOffsetValue, 0.);
  if (aParal->IsDone()) {
    TopoDS_Shape anOffset = aParal->Shape();
    GeomShapePtr aResult(new GeomAPI_Shape());
    aResult->setImpl(new TopoDS_Shape(anOffset));
    setShape(aResult);
    setDone(true);
  }
}
