// Copyright (C) 2019-2023  CEA/DEN, EDF R&D
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

#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

GeomAlgoAPI_Offset::GeomAlgoAPI_Offset(const GeomShapePtr& theShape,
                                       const double theOffsetValue)
{
  build(theShape, theOffsetValue);
}

void GeomAlgoAPI_Offset::build(const GeomShapePtr& theShape, const double theOffsetValue)
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

void GeomAlgoAPI_Offset::generated(const GeomShapePtr theOldShape,
                                   ListOfShape& theNewShapes)
{
  try {
    GeomAlgoAPI_MakeShape::generated(theOldShape, theNewShapes);
  } catch(...) {
    // nothing is generated
  }
}

GeomAlgoAPI_Offset::GeomAlgoAPI_Offset(const GeomPlanePtr& thePlane,
                                       const GeomShapePtr& theEdgeOrWire,
                                       const double theOffsetValue,
                                       const GeomAlgoAPI_OffsetJoint theJoint)
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
  aParal->Perform(theOffsetValue, 0.);
  if (aParal->IsDone()) {
    TopoDS_Shape anOffset = aParal->Shape();
    GeomShapePtr aResult(new GeomAPI_Shape());
    aResult->setImpl(new TopoDS_Shape(anOffset));
    setShape(aResult);
    setDone(true);
  }
}
