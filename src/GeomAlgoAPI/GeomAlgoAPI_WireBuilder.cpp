// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_WireBuilder.cpp
// Created:     14 April 2016
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_WireBuilder.h"

#include <BRepBuilderAPI_MakeWire.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Wire.hxx>

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_WireBuilder::wire(const ListOfShape& theShapes)
{
  BRepBuilderAPI_MakeWire aWireBuilder;

  for(ListOfShape::const_iterator anIt = theShapes.cbegin(); anIt != theShapes.cend(); ++anIt) {
    const TopoDS_Shape& aShape = (*anIt)->impl<TopoDS_Shape>();
    switch(aShape.ShapeType()) {
      case TopAbs_EDGE: {
        aWireBuilder.Add(TopoDS::Edge(aShape));
        break;
      }
      case TopAbs_WIRE: {
        aWireBuilder.Add(TopoDS::Wire(aShape));
        break;
      }
      default: {
        return GeomShapePtr();
      }
    }
  }

  if(aWireBuilder.Error() != BRepBuilderAPI_WireDone) {
    return GeomShapePtr();
  }

  GeomShapePtr aResultShape(new GeomAPI_Shape());
  aResultShape->setImpl(new TopoDS_Shape(aWireBuilder.Wire()));
  return aResultShape;
}
