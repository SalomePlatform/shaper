// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_CompoundBuilder.cpp
// Created:     24 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <GeomAlgoAPI_CompoundBuilder.h>
#include <BRep_Builder.hxx>
#include <TopoDS_Compound.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopExp.hxx>

std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_CompoundBuilder::compound(
    std::list<std::shared_ptr<GeomAPI_Shape> > theShapes)
{
  BRep_Builder aBuilder;
  TopoDS_Compound aComp;
  aBuilder.MakeCompound(aComp);

  std::list<std::shared_ptr<GeomAPI_Shape> >::const_iterator anIt = theShapes.begin(), aLast =
      theShapes.end();
  for (; anIt != aLast; anIt++) {
    aBuilder.Add(aComp, (*anIt)->impl<TopoDS_Shape>());
  }

  std::shared_ptr<GeomAPI_Shape> aRes(new GeomAPI_Shape);
  aRes->setImpl(new TopoDS_Shape(aComp));
  return aRes;
}

int GeomAlgoAPI_CompoundBuilder::id(
      std::shared_ptr<GeomAPI_Shape> theContext, std::shared_ptr<GeomAPI_Shape> theSub)
{
  int anID = 0;
  TopoDS_Shape aMainShape = theContext->impl<TopoDS_Shape>();
  const TopoDS_Shape& aSubShape = theSub->impl<TopoDS_Shape>();
  if (!aMainShape.IsNull() && !aSubShape.IsNull()) {
    TopTools_IndexedMapOfShape aSubShapesMap;
    TopExp::MapShapes(aMainShape, aSubShapesMap);
    anID = aSubShapesMap.FindIndex(aSubShape);
  }

  return anID;
}
