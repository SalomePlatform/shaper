// File:        GeomAlgoAPI_DFLoader.cpp
// Created:     23 October 2014
// Author:      Sergey Zaritchny

#include <GeomAlgoAPI_DFLoader.h>
#include <TopoDS_Iterator.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
//=======================================================================
//function : refineResult
//purpose  :
//=======================================================================
const TopoDS_Shape GeomAlgoAPI_DFLoader::refineResult(const  TopoDS_Shape& theResult)
{
  TopoDS_Shape aResult;
  if (theResult.ShapeType() == TopAbs_COMPOUND) {
    Standard_Integer nbSubResults = 0;
    TopoDS_Iterator itr(theResult);
    for (; itr.More(); itr.Next()) nbSubResults++;
    if (nbSubResults == 1) {
      itr.Initialize(theResult);
      if (itr.More()) aResult = itr.Value();
    }
  }
  return aResult;
}
/*
//=======================================================================
//function : loadDeletedShapes
//purpose  : load deleted shapes in DF
//=======================================================================
void GeomAlgoAPI_DFLoader::loadDeletedShapes (BRepBuilderAPI_MakeShape& theMS,
                                       const TopoDS_Shape&     theShapeIn,
                                       const TopAbs_ShapeEnum  theKindOfShape,
                                       TNaming_Builder&        theBuilder)
{
  TopTools_MapOfShape aView;
  TopExp_Explorer ShapeExplorer (theShapeIn, theKindOfShape);
  for (; ShapeExplorer.More(); ShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = ShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
    if (theMS.IsDeleted (aRoot)) {
      theBuilder.Delete (aRoot);
    }
  }
}

//=======================================================================
//function : loadAndOrientModifiedShapes
//purpose  : load modified shapes in DF with preliminary orientation adjustment
//=======================================================================
void GeomAlgoAPI_DFLoader::loadAndOrientModifiedShapes (BRepBuilderAPI_MakeShape&    theMS,
                                           const TopoDS_Shape&        theShapeIn,
                                           const TopAbs_ShapeEnum     theKindOfShape,
                                           TNaming_Builder&           theBuilder,
                                           const TopTools_DataMapOfShapeShape& theSubShapes)
{
  TopTools_MapOfShape aView;
  TopExp_Explorer aShapeExplorer (theShapeIn, theKindOfShape);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
    const TopTools_ListOfShape& aShapes = theMS.Modified (aRoot);
    TopTools_ListIteratorOfListOfShape aShapesIterator (aShapes);
    for (;aShapesIterator.More (); aShapesIterator.Next ()) {
      TopoDS_Shape aNewShape = aShapesIterator.Value ();
      if (theSubShapes.IsBound(aNewShape)) {
        aNewShape.Orientation((theSubShapes(aNewShape)).Orientation());
      }
      if (!aRoot.IsSame (aNewShape)) theBuilder.Modify (aRoot, aNewShape );
    }
  }
}

//=======================================================================
//function : loadAndOrientGeneratedShapes
//purpose  : load generated shapes in DF with preliminary orientation adjustment
//=======================================================================

void GeomAlgoAPI_DFLoader::loadAndOrientGeneratedShapes (BRepBuilderAPI_MakeShape&     theMS,
                                                const TopoDS_Shape&           theShapeIn,
                                                const TopAbs_ShapeEnum        theKindOfShape,
                                                TNaming_Builder&              theBuilder,
                                                const TopTools_DataMapOfShapeShape&    theSubShapes)
{
  TopTools_MapOfShape aView;
  TopExp_Explorer aShapeExplorer (theShapeIn, theKindOfShape);
  for (; aShapeExplorer.More(); aShapeExplorer.Next ()) {
    const TopoDS_Shape& aRoot = aShapeExplorer.Current ();
    if (!aView.Add(aRoot)) continue;
    const TopTools_ListOfShape& aShapes = theMS.Generated (aRoot);
    TopTools_ListIteratorOfListOfShape aShapesIterator (aShapes);
    for (;aShapesIterator.More (); aShapesIterator.Next ()) {
      TopoDS_Shape aNewShape = aShapesIterator.Value ();
      if (theSubShapes.IsBound(aNewShape)) {
        aNewShape.Orientation((theSubShapes(aNewShape)).Orientation());
      }
      if (!aRoot.IsSame (aNewShape)) theBuilder.Generated (aRoot,aNewShape );
    }
  }
}
*/