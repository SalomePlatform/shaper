// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeTools.h
// Created:     3 August 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <gp_Pln.hxx>

#include <BOPTools.hxx>
#include <BRepBuilderAPI_MakeFace.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <BRep_Tool.hxx>
#include <Geom_Plane.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Vector.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Face.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>
#include <TopoDS.hxx>


//=================================================================================================
double GeomAlgoAPI_ShapeTools::volume(std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if(!theShape) {
    return 0.0;
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if(aShape.IsNull()) {
    return 0.0;
  }
  BRepGProp::VolumeProperties(aShape, aGProps);
  return aGProps.Mass();
}

//=================================================================================================
std::shared_ptr<GeomAPI_Pnt> GeomAlgoAPI_ShapeTools::centreOfMass(std::shared_ptr<GeomAPI_Shape> theShape)
{
  GProp_GProps aGProps;
  if(!theShape) {
    return std::shared_ptr<GeomAPI_Pnt>();
  }
  const TopoDS_Shape& aShape = theShape->impl<TopoDS_Shape>();
  if(aShape.IsNull()) {
    return std::shared_ptr<GeomAPI_Pnt>();
  }
  BRepGProp::SurfaceProperties(aShape, aGProps);
  gp_Pnt aCentre = aGProps.CentreOfMass();
  return std::shared_ptr<GeomAPI_Pnt>(new GeomAPI_Pnt(aCentre.X(), aCentre.Y(), aCentre.Z()));
}

//=================================================================================================
void GeomAlgoAPI_ShapeTools::combineShapes(const std::shared_ptr<GeomAPI_Shape> theCompound,
                                           const GeomAPI_Shape::ShapeType theType,
                                           ListOfShape& theCombinedShapes,
                                           ListOfShape& theFreeShapes)
{
  if(!theCompound.get()) {
    return;
  }

  if(theType != GeomAPI_Shape::SHELL && theType != GeomAPI_Shape::COMPSOLID) {
    return;
  }

  TopAbs_ShapeEnum aTS = TopAbs_EDGE;
  TopAbs_ShapeEnum aTA = TopAbs_FACE;
  if(theType == GeomAPI_Shape::COMPSOLID) {
    aTS = TopAbs_FACE;
    aTA = TopAbs_SOLID;
  }

  // Map subshapes and shapes.
  const TopoDS_Shape& aShapesComp = theCompound->impl<TopoDS_Shape>();
  BOPCol_IndexedDataMapOfShapeListOfShape aMapEF;
  BOPTools::MapShapesAndAncestors(aShapesComp, aTS, aTA, aMapEF);
  if(aMapEF.IsEmpty()) {
    return;
  }

  // Get all shapes with common subshapes and free shapes.
  NCollection_Map<TopoDS_Shape> aFreeShapes;
  NCollection_Vector<NCollection_Map<TopoDS_Shape>> aShapesWithCommonSubshapes;
  for(BOPCol_IndexedDataMapOfShapeListOfShape::Iterator anIter(aMapEF); anIter.More(); anIter.Next()) {
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
        for(BOPCol_IndexedDataMapOfShapeListOfShape::Iterator anIter(aMapEF); anIter.More(); anIter.Next()) {
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
    const NCollection_Map<TopoDS_Shape>& aShapesMap = anIter.Value();
    for(NCollection_Map<TopoDS_Shape>::Iterator aShIter(aShapesMap); aShIter.More(); aShIter.Next()) {
      const TopoDS_Shape& aShape = aShIter.Value();
      theType == GeomAPI_Shape::COMPSOLID ? aBuilder.Add(aCSolid, aShape) : aBuilder.Add(aShell, aShape);
    }
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    TopoDS_Shape* aSh = theType == GeomAPI_Shape::COMPSOLID ? new TopoDS_Shape(aCSolid) : new TopoDS_Shape(aShell);
    aGeomShape->setImpl<TopoDS_Shape>(aSh);
    theCombinedShapes.push_back(aGeomShape);
  }

  // Adding free shapes.
  for(NCollection_Map<TopoDS_Shape>::Iterator aShIter(aFreeShapes); aShIter.More(); aShIter.Next()) {
    const TopoDS_Shape& aShape = aShIter.Value();
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    aGeomShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShape));
    theFreeShapes.push_back(aGeomShape);
  }
}

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_ShapeTools::faceToInfinitePlane(const std::shared_ptr<GeomAPI_Shape>& theFace)
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
