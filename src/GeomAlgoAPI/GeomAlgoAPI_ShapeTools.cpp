// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeTools.h
// Created:     3 August 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_ShapeTools.h>

#include <GeomAlgoAPI_CompoundBuilder.h>

#include <BOPTools.hxx>
#include <BRepGProp.hxx>
#include <BRepTools.hxx>
#include <GProp_GProps.hxx>
#include <NCollection_Vector.hxx>
#include <TCollection_AsciiString.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Shell.hxx>

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
void GeomAlgoAPI_ShapeTools::combineFacesToShells(const ListOfShape& theFacesList,
                                                  ListOfShape& theShells,
                                                  ListOfShape& theFreeFaces)
{
  if(theFacesList.empty()) {
    return;
  }

  // Adding all faces to compoud.
  std::shared_ptr<GeomAPI_Shape> aFacesCompound = GeomAlgoAPI_CompoundBuilder::compound(theFacesList);
  if(!aFacesCompound.get()) {
    return;
  }

  // Map edges and faces.
  const TopoDS_Shape& aFacesComp = aFacesCompound->impl<TopoDS_Shape>();
  BOPCol_IndexedDataMapOfShapeListOfShape aMapEF;
  BOPTools::MapShapesAndAncestors(aFacesComp, TopAbs_EDGE, TopAbs_FACE, aMapEF);
  if(aMapEF.IsEmpty()) {
    return;
  }

  // Get all faces with common edges and free faces.
  NCollection_Map<TopoDS_Shape> aFreeFaces;
  NCollection_Vector<NCollection_Map<TopoDS_Shape>> aFacesWithCommonEdges;
  for(BOPCol_IndexedDataMapOfShapeListOfShape::Iterator anIter(aMapEF); anIter.More(); anIter.Next()) {
    const TopoDS_Shape& aShape = anIter.Key();
    BOPCol_ListOfShape& aListOfShape = anIter.ChangeValue();
    if(aListOfShape.IsEmpty()) {
      continue;
    }
    else if(aListOfShape.Size() == 1) {
      aFreeFaces.Add(aListOfShape.First());
      aListOfShape.Clear();
    } else {
      NCollection_Map<TopoDS_Shape> aTempMap;
      aTempMap.Add(aListOfShape.First());
      aTempMap.Add(aListOfShape.Last());
      aFreeFaces.Remove(aListOfShape.First());
      aFreeFaces.Remove(aListOfShape.Last());
      aListOfShape.Clear();
      for(NCollection_Map<TopoDS_Shape>::Iterator aTempIter(aTempMap); aTempIter.More(); aTempIter.Next()) {
        const TopoDS_Shape& aTempShape = aTempIter.Value();
        for(BOPCol_IndexedDataMapOfShapeListOfShape::Iterator anIter(aMapEF); anIter.More(); anIter.Next()) {
          BOPCol_ListOfShape& aTempListOfShape = anIter.ChangeValue();
          if(aTempListOfShape.IsEmpty()) {
            continue;
          } else if(aTempListOfShape.Size() == 1 && aTempListOfShape.First() == aTempShape) {
            aTempListOfShape.Clear();
          } else if(aTempListOfShape.Size() > 1) {
            if(aTempListOfShape.First() == aTempShape) {
              aTempMap.Add(aTempListOfShape.Last());
              aFreeFaces.Remove(aTempListOfShape.Last());
              aTempListOfShape.Clear();
            } else if(aTempListOfShape.Last() == aTempShape) {
              aTempMap.Add(aTempListOfShape.First());
              aFreeFaces.Remove(aTempListOfShape.First());
              aTempListOfShape.Clear();
            }
          }
        }
      }
      aFacesWithCommonEdges.Append(aTempMap);
    }
  }

  // Make shells from faces with common edges.
  NCollection_Vector<TopoDS_Shape> aShells;
  for(NCollection_Vector<NCollection_Map<TopoDS_Shape>>::Iterator anIter(aFacesWithCommonEdges); anIter.More(); anIter.Next()) {
    TopoDS_Shell aShell;
    TopoDS_Builder aBuilder;
    aBuilder.MakeShell(aShell);
    const NCollection_Map<TopoDS_Shape>& aShapesMap = anIter.Value();
    for(NCollection_Map<TopoDS_Shape>::Iterator aShIter(aShapesMap); aShIter.More(); aShIter.Next()) {
      const TopoDS_Shape& aFace = aShIter.Value();
      aBuilder.Add(aShell, aFace);
    }
    std::shared_ptr<GeomAPI_Shape> aGeomShell(std::make_shared<GeomAPI_Shape>());
    aGeomShell->setImpl<TopoDS_Shape>(new TopoDS_Shape(aShell));
    theShells.push_back(aGeomShell);
  }

  // Adding free faces.
  for(NCollection_Map<TopoDS_Shape>::Iterator aShIter(aFreeFaces); aShIter.More(); aShIter.Next()) {
    const TopoDS_Shape& aFace = aShIter.Value();
    std::shared_ptr<GeomAPI_Shape> aGeomFace(std::make_shared<GeomAPI_Shape>());
    aGeomFace->setImpl<TopoDS_Shape>(new TopoDS_Shape(aFace));
    theFreeFaces.push_back(aGeomFace);
  }
}
