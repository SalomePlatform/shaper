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

#include "GeomAlgoAPI_Thickness.h"

#include <GeomAPI_Pln.h>

#include <BRepClass3d_SolidClassifier.hxx>
#include <BRepOffsetAPI_MakeThickSolid.hxx>
#include <BRepOffset_MakeOffset.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopTools_MapOfShape.hxx>

static GeomShapePtr convert(const TopoDS_Shape& theShape)
{
  GeomShapePtr aNewShape(new GeomAPI_Shape);
  aNewShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(theShape));
  return aNewShape;
}

GeomAlgoAPI_Thickness::GeomAlgoAPI_Thickness(const GeomShapePtr& theShape,
                                             const double        theThickness,
                                             const bool          isInside)
{
  buildThickening(theShape, theThickness, isInside);
}

GeomAlgoAPI_Thickness::GeomAlgoAPI_Thickness(const GeomShapePtr& theShape,
                                             const ListOfShape&  theFaces,
                                             const double        theThickness,
                                             const bool          isInside)
{
  buildHollowedSolid(theShape, theFaces, theThickness, isInside);
}

void GeomAlgoAPI_Thickness::generated(const GeomShapePtr theOldShape,
                                      ListOfShape& theNewShapes)
{
  GeomAlgoAPI_MakeShape::generated(theOldShape, theNewShapes);

  MapModified::iterator aFound = myGenerated.find(theOldShape);
  if (aFound != myGenerated.end())
    theNewShapes.insert(theNewShapes.end(),
                        aFound->second.begin(), aFound->second.end());
}

void GeomAlgoAPI_Thickness::buildThickening(const GeomShapePtr& theShape,
                                            const double        theThickness,
                                            const bool          isInside)
{
  Standard_Real aTol = Precision::Confusion();
  if (theThickness < aTol) {
    myError = "The thickness value is Too small or negative";
    return;
  }

  Standard_Real anOffset = theThickness;
  if (isInside)
    anOffset = -anOffset;

  const TopoDS_Shape& aShapeBase = theShape->impl<TopoDS_Shape>();
  const TopAbs_ShapeEnum aType = aShapeBase.ShapeType();

  if (aType != TopAbs_FACE && aType != TopAbs_SHELL) {
    myError = "The base shape for thickening should be a face or a shell";
    return;
  }

  BRepClass3d_SolidClassifier aClassifier(aShapeBase);
  aClassifier.PerformInfinitePoint(Precision::Confusion());
  if (aClassifier.State()==TopAbs_IN) {
    // If the generated pipe faces normals are oriented towards the inside,
    // the offset is negative, so that the thickening is still towards outside
    anOffset = -anOffset;
  }

  Standard_Boolean anIntersection = Standard_False;
  Standard_Boolean aSelfInter = Standard_False;
  Standard_Boolean isThickening = Standard_True;

  BRepOffset_MakeOffset* aMakeOffset =
    new BRepOffset_MakeOffset(aShapeBase,
                              anOffset,
                              aTol,
                              BRepOffset_Skin,
                              anIntersection,
                              aSelfInter,
                              GeomAbs_Intersection,
                              isThickening);

  setImpl(aMakeOffset);
  setBuilderType(OCCT_BRepOffset_MakeOffset);

  if (aMakeOffset->IsDone()) {
    TopoDS_Shape aResult = aMakeOffset->Shape();

    // Control the solid orientation. This is mostly done to fix a bug in case
    // of extrusion of a circle. The built solid is then badly oriented.
    BRepClass3d_SolidClassifier anotherClassifier(aResult);
    anotherClassifier.PerformInfinitePoint(Precision::Confusion());
    if (anotherClassifier.State() == TopAbs_IN) {
      aResult.Reverse();
    }

    std::shared_ptr<GeomAPI_Shape> aShape (new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aResult));
    setShape(aShape);
    setDone(true);

    // History for ThickShell is incomplete (OCCT bug 33844)
    // Here we try to add generated walls (FACE from EDGE or VERTEX)

    // 1. Gather all generated/modified/initial faces in a map
    //    (for that faces we don't need to adjust history).
    //    Collect also edges and vertices of initial shape in a separate map.
    TopTools_MapOfShape aFacesMap;
    TopTools_MapOfShape anInitialMap;
    TopTools_ListOfShape aList, aListTmp;
    {
      TopExp_Explorer anExp (aShapeBase, TopAbs_FACE);
      for (; anExp.More(); anExp.Next()) {
        TopoDS_Shape aFace = anExp.Current();
        aList.Append(aFace);

        aListTmp = aMakeOffset->Modified(aFace);
        aList.Append(aListTmp);
        aListTmp = aMakeOffset->Generated(aFace);
        aList.Append(aListTmp);
      }
    }
    {
      TopExp_Explorer anExp (aShapeBase, TopAbs_EDGE);
      for (; anExp.More(); anExp.Next()) {
        TopoDS_Shape anEdge = anExp.Current();
        anInitialMap.Add(anEdge);

        aListTmp = aMakeOffset->Modified(anEdge);
        aList.Append(aListTmp);
        aListTmp = aMakeOffset->Generated(anEdge);
        aList.Append(aListTmp);
      }
    }
    {
      TopExp_Explorer anExp (aShapeBase, TopAbs_VERTEX);
      for (; anExp.More(); anExp.Next()) {
        TopoDS_Shape aVertex = anExp.Current();
        anInitialMap.Add(aVertex);

        aListTmp = aMakeOffset->Modified(aVertex);
        aList.Append(aListTmp);
        aListTmp = aMakeOffset->Generated(aVertex);
        aList.Append(aListTmp);
      }
    }
    TopTools_ListIteratorOfListOfShape anIt (aList);
    for (; anIt.More(); anIt.Next()) {
      TopoDS_Shape aSh = anIt.Value();
      if (aSh.ShapeType() == TopAbs_FACE) {
        aFacesMap.Add(aSh);
      }
    }

    // 2. Explode result shape into faces, try to construct new
    //    history for the faces, that are not in the map.
    TopExp_Explorer anExp (aResult, TopAbs_FACE);
    for (; anExp.More(); anExp.Next()) {
      TopoDS_Shape aFace = anExp.Current();
      if (!aFacesMap.Contains(aFace)) {
        bool isFound = false;
        // a. Try to find initial edge in this face
        TopExp_Explorer anExpE (aFace, TopAbs_EDGE);
        for (; anExpE.More() && !isFound; anExpE.Next()) {
          TopoDS_Shape anEdge = anExpE.Current();
          if (anInitialMap.Contains(anEdge)) {
            myGenerated[convert(anEdge)].push_back(convert(aFace));
            isFound = true;
          }
        }
        if (!isFound) {
          // b. Try to find initial vertex in this face
          TopExp_Explorer anExpV (aFace, TopAbs_VERTEX);
          for (; anExpV.More() && !isFound; anExpV.Next()) {
            TopoDS_Shape aVertex = anExpV.Current();
            if (anInitialMap.Contains(aVertex)) {
              myGenerated[convert(aVertex)].push_back(convert(aFace));
              isFound = true;
            }
          }
        }
      }
    }
    // END: history adjustment
  }
}

void GeomAlgoAPI_Thickness::buildHollowedSolid(const GeomShapePtr& theShape,
                                               const ListOfShape&  theFaces,
                                               const double        theThickness,
                                               const bool          isInside)
{
  if (theFaces.empty())
    return;

  Standard_Real aTol = Precision::Confusion();
  if (theThickness < aTol) {
    myError = "The thickness value is Too small or negative";
    return;
  }

  Standard_Real anOffset = theThickness;
  if (isInside)
    anOffset = -anOffset;

  //TopoDS_Shape aShapeBase = theShape->impl<TopoDS_Shape>();
  const TopoDS_Shape& aShapeBase = theShape->impl<TopoDS_Shape>();
  const TopAbs_ShapeEnum aType = aShapeBase.ShapeType();

  if (aType != TopAbs_SOLID) {
    myError = "The base shape for hollowed solid creation should be a solid";
    return;
  }

  // put selected faces into a list
  TopTools_ListOfShape aFacesToRm;
  for (ListOfShape::const_iterator anIt = theFaces.begin();
       anIt != theFaces.end(); ++anIt) {
    if ((*anIt)->isFace())
      aFacesToRm.Append((*anIt)->impl<TopoDS_Shape>());
  }

  Standard_Boolean anIntersection = Standard_False;
  Standard_Boolean aSelfInter = Standard_False;

  // Create a hollowed solid.
  BRepOffsetAPI_MakeThickSolid* aMkSolid = new BRepOffsetAPI_MakeThickSolid();
  aMkSolid->MakeThickSolidByJoin(aShapeBase,
                                 aFacesToRm,
                                 anOffset,
                                 aTol,
                                 BRepOffset_Skin,
                                 anIntersection,
                                 aSelfInter,
                                 GeomAbs_Intersection);

  setImpl(aMkSolid);
  setBuilderType(OCCT_BRepBuilderAPI_MakeShape);

  if (aMkSolid->IsDone()) {
    const TopoDS_Shape& aResult = aMkSolid->Shape();
    std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
    aShape->setImpl(new TopoDS_Shape(aResult));
    setShape(aShape);
    setDone(true);
  }
}
