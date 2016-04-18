// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_WireBuilder.cpp
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_WireBuilder.h"

#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>
#include <TopExp_Explorer.hxx>

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_WireBuilder::wire(const ListOfShape& theShapes)
{
  TopTools_ListOfShape aListOfEdges;

  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    const TopoDS_Shape& aShape = (*anIt)->impl<TopoDS_Shape>();
    switch(aShape.ShapeType()) {
      case TopAbs_EDGE: {
        aListOfEdges.Append(aShape);
        break;
      }
      case TopAbs_WIRE: {
        for(TopExp_Explorer anExp(aShape, TopAbs_EDGE); anExp.More(); anExp.Next()) {
          aListOfEdges.Append(anExp.Current());
        }
        break;
      }
      default: {
        return GeomShapePtr();
      }
    }
  }

  BRepBuilderAPI_MakeWire aWireBuilder;
  aWireBuilder.Add(aListOfEdges);
  if(aWireBuilder.Error() != BRepBuilderAPI_WireDone) {
    return GeomShapePtr();
  }

  GeomShapePtr aResultShape(new GeomAPI_Shape());
  aResultShape->setImpl(new TopoDS_Shape(aWireBuilder.Wire()));
  return aResultShape;
}
