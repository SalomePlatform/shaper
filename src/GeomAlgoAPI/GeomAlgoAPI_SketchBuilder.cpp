// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_SketchBuilder.cpp
// Created:     02 Jun 2014
// Author:      Artem ZHIDKOV

#include <GeomAlgoAPI_SketchBuilder.h>
#include <GeomAPI_PlanarEdges.h>

#include <BOPAlgo_Builder.hxx>
#include <BRep_Builder.hxx>
#include <BRepTopAdaptor_FClass2d.hxx>
#include <Geom_Plane.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>


void GeomAlgoAPI_SketchBuilder::createFaces(
    const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
    const std::shared_ptr<GeomAPI_Dir>& theDirX,
    const std::shared_ptr<GeomAPI_Dir>& theNorm,
    const std::list<std::shared_ptr<GeomAPI_Shape> >& theFeatures,
    std::list<std::shared_ptr<GeomAPI_Shape> >& theResultFaces)
{
  if (theFeatures.empty())
    return;

  BRep_Builder aBuilder;
  // Planar face, where the sketch was built
  Handle(Geom_Surface) aPlane(new Geom_Plane(theOrigin->impl<gp_Pnt>(), theNorm->impl<gp_Dir>()));
  TopoDS_Face aPlnFace;
  aBuilder.MakeFace(aPlnFace, aPlane, Precision::Confusion());

  // Use General Fuse algorithm to prepare all subfaces, bounded by given list of edges
  BOPAlgo_Builder aBB;
  aBB.AddArgument(aPlnFace);

  BOPCol_ListOfShape anEdges;
  BOPCol_ListIteratorOfListOfShape aShapeIt;
  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator aFeatIt = theFeatures.begin();
  for (; aFeatIt != theFeatures.end(); aFeatIt++) {
    std::shared_ptr<GeomAPI_Shape> aShape(*aFeatIt);
    const TopoDS_Edge& anEdge = aShape->impl<TopoDS_Edge>();
    if (anEdge.ShapeType() == TopAbs_EDGE)
      aBB.AddArgument(anEdge);
  }
  aBB.Perform();
  if (aBB.ErrorStatus())
    return;

  // Collect faces
  const TopTools_ListOfShape& anAreas = aBB.Modified(aPlnFace);
  TopTools_ListIteratorOfListOfShape anIt(anAreas);
  for (; anIt.More(); anIt.Next()) {
    TopoDS_Face aFace = TopoDS::Face(anIt.Value());
    // avoid infinite faces
    BRepTopAdaptor_FClass2d aFClass(aFace, Precision::Confusion());
    if (aFClass.PerformInfinitePoint() == TopAbs_IN)
      continue;

    // remove internal edges from faces
    TopExp_Explorer anExp(aFace, TopAbs_EDGE);
    for (; anExp.More(); anExp.Next())
      if (anExp.Current().Orientation() == TopAbs_INTERNAL)
        break;
    if (anExp.More()) {
      TopoDS_Face aNewFace;
      aBuilder.MakeFace(aNewFace, aPlane, Precision::Confusion());
      TopoDS_Wire aWire;
      aBuilder.MakeWire(aWire);
      for (anExp.ReInit(); anExp.More(); anExp.Next())
        if (anExp.Current().Orientation() != TopAbs_INTERNAL)
          aBuilder.Add(aWire, anExp.Current());
      aBuilder.Add(aNewFace, aWire);
      aFace = aNewFace;
    }

    // Store face
    std::shared_ptr<GeomAPI_Shape> aResFace(new GeomAPI_Shape);
    aResFace->setImpl(new TopoDS_Face(aFace));
    theResultFaces.push_back(aResFace);
  }
}

void GeomAlgoAPI_SketchBuilder::createFaces(const std::shared_ptr<GeomAPI_Pnt>& theOrigin,
                                            const std::shared_ptr<GeomAPI_Dir>& theDirX,
                                            const std::shared_ptr<GeomAPI_Dir>& theNorm,
                                            const std::shared_ptr<GeomAPI_Shape>& theWire,
                                            std::list<std::shared_ptr<GeomAPI_Shape> >& theResultFaces)
{
  std::shared_ptr<GeomAPI_PlanarEdges> aWire = 
    std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(theWire);
  if(aWire) {
    // Filter wires, return only faces.
    createFaces(theOrigin, theDirX, theNorm, aWire->getEdges(), theResultFaces);
  } else { // it may be only one circle
    std::shared_ptr<GeomAPI_Edge> anEdge = std::dynamic_pointer_cast<GeomAPI_Edge>(theWire);
    if (anEdge) {
      std::list<std::shared_ptr<GeomAPI_Shape> > aList;
      aList.push_back(anEdge);
      createFaces(theOrigin, theDirX, theNorm, aList, theResultFaces);
    }
  }
}
