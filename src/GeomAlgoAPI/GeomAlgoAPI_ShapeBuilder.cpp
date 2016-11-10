// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_ShapeBuilder.cpp
// Created:     27 April 2016
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_ShapeBuilder.h"

#include "GeomAlgoAPI_MakeShapeCustom.h"

#include <GeomAPI_ShapeIterator.h>

#include <BRep_Builder.hxx>
#include <BRepBuilderAPI_Copy.hxx>
#include <BRepExtrema_DistShapeShape.hxx>
#include <BRepTools_ReShape.hxx>
#include <Precision.hxx>
#include <TopExp_Explorer.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS_Iterator.hxx>
#include <TopoDS_Shape.hxx>

//==================================================================================================
void GeomAlgoAPI_ShapeBuilder::add(std::shared_ptr<GeomAPI_Shape> theShape,
                                   const std::shared_ptr<GeomAPI_Shape> theShapeToAdd)
{
  if(!theShape.get() || !theShapeToAdd.get()) {
    return;
  }

  TopoDS_Shape* aShape = theShape->implPtr<TopoDS_Shape>();
  const TopoDS_Shape& aShapeToAdd = theShapeToAdd->impl<TopoDS_Shape>();

  BRep_Builder aBuilder;
  aBuilder.Add(*aShape, aShapeToAdd);
}


//==================================================================================================
void GeomAlgoAPI_ShapeBuilder::remove(std::shared_ptr<GeomAPI_Shape> theShape,
                                      const std::shared_ptr<GeomAPI_Shape> theShapeToRemove)
{
  if(!theShape.get() || !theShapeToRemove.get()) {
    return;
  }

  TopoDS_Shape* aShape = theShape->implPtr<TopoDS_Shape>();
  const TopoDS_Shape& aShapeToRemove = theShapeToRemove->impl<TopoDS_Shape>();

  BRep_Builder aBuilder;
  aBuilder.Remove(*aShape, aShapeToRemove);
}

//==================================================================================================
GeomAlgoAPI_ShapeBuilder::GeomAlgoAPI_ShapeBuilder()
{
}

//==================================================================================================
void GeomAlgoAPI_ShapeBuilder::removeInternal(const std::shared_ptr<GeomAPI_Shape> theShape)
{
  GeomShapePtr aResultShape;

  GeomAPI_Shape::ShapeType aBaseShapeType = theShape->shapeType();
  if(aBaseShapeType == GeomAPI_Shape::WIRE) {
    aResultShape = theShape->emptyCopied();
    std::shared_ptr<GeomAlgoAPI_MakeShapeCustom>
      aMakeShapeCustom(new GeomAlgoAPI_MakeShapeCustom());
    for(GeomAPI_ShapeIterator anIter(theShape); anIter.more(); anIter.next()) {
      GeomShapePtr aSubShape = anIter.current();
      GeomShapePtr aSubShapeCopy = aSubShape->emptyCopied();
      for(GeomAPI_ShapeIterator aSubIter(aSubShape); aSubIter.more(); aSubIter.next()) {
        GeomShapePtr aSubOfSubShape = aSubIter.current();
        if(aSubOfSubShape->orientation() != GeomAPI_Shape::INTERNAL) {
          GeomAlgoAPI_ShapeBuilder::add(aSubShapeCopy, aSubOfSubShape);
        }
      }
      aMakeShapeCustom->addModified(aSubShape, aSubShapeCopy);
      GeomAlgoAPI_ShapeBuilder::add(aResultShape, aSubShapeCopy);
    }
    this->appendAlgo(aMakeShapeCustom);
  } else if(aBaseShapeType == GeomAPI_Shape::FACE) {
    const TopoDS_Shape& aBaseShape = theShape->impl<TopoDS_Shape>();
    BRepBuilderAPI_Copy* aCopyBuilder = new BRepBuilderAPI_Copy(aBaseShape);
    this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(
      new GeomAlgoAPI_MakeShape(aCopyBuilder)));
    if(!aCopyBuilder->IsDone()) {
      return;
    }
    TopoDS_Shape aShape = aCopyBuilder->Shape();
    TopoDS_Shape aShapeCopy = aShape.EmptyCopied();
    BRep_Builder aBuilder;
    for(TopoDS_Iterator anIt(aShape); anIt.More(); anIt.Next()) {
      const TopoDS_Shape& aSubShape = anIt.Value();
      if(aSubShape.ShapeType() == TopAbs_WIRE
          && aSubShape.Orientation() != TopAbs_INTERNAL) {
        aBuilder.Add(aShapeCopy, aSubShape);
      }
    }
    aResultShape.reset(new GeomAPI_Shape());
    aResultShape->setImpl(new TopoDS_Shape(aShapeCopy));
  }

  setShape(aResultShape);
  setDone(true);
}

//==================================================================================================
void GeomAlgoAPI_ShapeBuilder::addInternal(const std::shared_ptr<GeomAPI_Shape> theShape,
                                           const ListOfShape& theShapesToAdd)
{
  // Get base shape.
  if(!theShape.get()) {
    return;
  }
  const TopoDS_Shape& aBaseShape = theShape->impl<TopoDS_Shape>();
  TopAbs_ShapeEnum aBaseShapeType = aBaseShape.ShapeType();

  // Copy base shape.
  BRepBuilderAPI_Copy* aCopyBuilder = new BRepBuilderAPI_Copy(aBaseShape);
  this->appendAlgo(std::shared_ptr<GeomAlgoAPI_MakeShape>(new GeomAlgoAPI_MakeShape(aCopyBuilder)));
  if(!aCopyBuilder->IsDone()) {
    return;
  }
  TopoDS_Shape aResultShape = aCopyBuilder->Shape();

  // Copy sub-shapes from list to new shape.
  BRep_Builder aBuilder;
  std::shared_ptr<GeomAlgoAPI_MakeShapeCustom> aMakeShapeCustom(new GeomAlgoAPI_MakeShapeCustom());
  for(ListOfShape::const_iterator
      anIt = theShapesToAdd.cbegin(); anIt != theShapesToAdd.cend(); ++anIt) {
    TopoDS_Shape aShapeToAdd = (*anIt)->impl<TopoDS_Shape>();
    TopoDS_Shape aModShapeToAdd = aShapeToAdd;
    aModShapeToAdd.Orientation(TopAbs_INTERNAL);
    for(TopExp_Explorer aResExp(aResultShape, TopAbs_VERTEX); aResExp.More(); aResExp.Next()) {
      const TopoDS_Vertex& aVertexInRes = TopoDS::Vertex(aResExp.Current());
      const gp_Pnt aPntInRes = BRep_Tool::Pnt(aVertexInRes);
      for(TopExp_Explorer anAddExp(aShapeToAdd, TopAbs_VERTEX); anAddExp.More(); anAddExp.Next()) {
        const TopoDS_Vertex& aVertexInAdd = TopoDS::Vertex(anAddExp.Current());
        const gp_Pnt aPntInAdd = BRep_Tool::Pnt(aVertexInAdd);
        if(aPntInRes.Distance(aPntInAdd) < Precision::Confusion()) {
          BRepTools_ReShape aReShape;
          TopoDS_Shape aVertexInResMod = aVertexInRes;
          aVertexInResMod.Orientation(aVertexInAdd.Orientation());
          aReShape.Replace(aVertexInAdd, aVertexInResMod);
          aModShapeToAdd = aReShape.Apply(aModShapeToAdd);
        }
      }
    }

    GeomShapePtr aGeomBaseShape(new GeomAPI_Shape());
    GeomShapePtr aGeomModShape(new GeomAPI_Shape());
    aGeomBaseShape->setImpl(new TopoDS_Shape(aShapeToAdd));
    aGeomModShape->setImpl(new TopoDS_Shape(aModShapeToAdd));
    aMakeShapeCustom->addModified(aGeomBaseShape, aGeomModShape);
    aShapeToAdd = aModShapeToAdd;

    TopAbs_ShapeEnum aShapeToAddType = aShapeToAdd.ShapeType();
    if(aBaseShapeType == TopAbs_WIRE) {
      if(aShapeToAddType == TopAbs_VERTEX) {
        // Find on which edge vertex is lie and add to this edge.
        for(TopExp_Explorer
            aResultExp(aResultShape, TopAbs_EDGE); aResultExp.More(); aResultExp.Next()) {
          TopoDS_Shape anEdge = aResultExp.Current();
          BRepExtrema_DistShapeShape aDist(anEdge, aShapeToAdd);
          aDist.Perform();
          if(aDist.IsDone() && aDist.Value() <= Precision::Confusion()) {
            aShapeToAdd.Orientation(TopAbs_INTERNAL);
            Standard_Boolean aFreeFlag = anEdge.Free();
            anEdge.Free(Standard_True);
            aBuilder.Add(anEdge, aShapeToAdd);
            anEdge.Free(aFreeFlag);
            break;
          }
        }
      }
    } else if(aBaseShapeType == GeomAPI_Shape::FACE) {
      if(aShapeToAddType == GeomAPI_Shape::EDGE) {
        aShapeToAdd.Orientation(TopAbs_INTERNAL);
        TopoDS_Wire aWire;
        aBuilder.MakeWire(aWire);
        aBuilder.Add(aWire, aShapeToAdd);
        aShapeToAdd = aWire;
        aShapeToAdd.Orientation(TopAbs_INTERNAL);
      }
      aBuilder.Add(aResultShape, aShapeToAdd);
    }
  }
  this->appendAlgo(aMakeShapeCustom);

  // Set result.
  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResultShape));
  setShape(aShape);
  setDone(true);
}
