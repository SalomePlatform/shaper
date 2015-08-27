// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Partition.cpp
// Created:     21 Aug 2015
// Author:      Sergey POKHODENKO

#include "GeomAlgoAPI_Partition.h"

#include <GeomAlgoAPI_DFLoader.h>

#include <GEOMAlgo_Splitter.hxx>

#include <BRepCheck_Analyzer.hxx>
#include <TopExp_Explorer.hxx>
#include <TopTools_ListOfShape.hxx>

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Partition::make(const ListOfShape& theObjects,
                                                           const ListOfShape& theTools)
{
  GeomAlgoAPI_Partition aBoolAlgo(theObjects, theTools);
  if(aBoolAlgo.isDone() && !aBoolAlgo.shape()->isNull() && aBoolAlgo.isValid()) {
    return aBoolAlgo.shape();
  }
  return std::shared_ptr<GeomAPI_Shape>();
}

//=================================================================================================
GeomAlgoAPI_Partition::GeomAlgoAPI_Partition(const ListOfShape& theObjects,
                                             const ListOfShape& theTools)
: myDone(false)
{
  build(theObjects, theTools);
}


//=================================================================================================
void GeomAlgoAPI_Partition::build(const ListOfShape& theObjects,
                                  const ListOfShape& theTools)
{
  if (theObjects.empty() || theTools.empty()) {
    return;
  }

  // Creating partition operation.
  GEOMAlgo_Splitter* anOperation = new GEOMAlgo_Splitter;
  myMkShape.reset(new GeomAlgoAPI_MakeShape(anOperation, GeomAlgoAPI_MakeShape::BOPAlgoBuilder));

  // Getting objects.
  TopTools_ListOfShape anObjects;
  for (ListOfShape::const_iterator anObjectsIt = theObjects.begin(); anObjectsIt != theObjects.end(); anObjectsIt++) {
    const TopoDS_Shape& aShape = (*anObjectsIt)->impl<TopoDS_Shape>();
    anOperation->AddArgument(aShape);
  }

  // Getting tools.
  TopTools_ListOfShape aTools;
  for (ListOfShape::const_iterator aToolsIt = theTools.begin(); aToolsIt != theTools.end(); aToolsIt++) {
    const TopoDS_Shape& aShape = (*aToolsIt)->impl<TopoDS_Shape>();
    anOperation->AddTool(aShape);
  }

  // Building and getting result.
  anOperation->Perform();
  TopoDS_Shape aResult = anOperation->Shape();
  myDone = !aResult.IsNull();
  if (!myDone) {
    return;
  }

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }

  // fill data map to keep correct orientation of sub-shapes
  myMap.reset(new GeomAPI_DataMapOfShapeShape());
  for (TopExp_Explorer Exp(aResult,TopAbs_FACE); Exp.More(); Exp.Next()) {
    std::shared_ptr<GeomAPI_Shape> aCurrentShape(new GeomAPI_Shape());
    aCurrentShape->setImpl(new TopoDS_Shape(Exp.Current()));
    myMap->bind(aCurrentShape, aCurrentShape);
  }
  myShape.reset(new GeomAPI_Shape());
  myShape->setImpl(new TopoDS_Shape(aResult));

}

//=================================================================================================
const bool GeomAlgoAPI_Partition::isDone() const
{
  return myDone;
}

//=================================================================================================
const bool GeomAlgoAPI_Partition::isValid() const
{
  BRepCheck_Analyzer aChecker(myShape->impl<TopoDS_Shape>());
  return (aChecker.IsValid() == Standard_True);
}

//=================================================================================================
const std::shared_ptr<GeomAPI_Shape>& GeomAlgoAPI_Partition::shape() const
{
  return myShape;
}

//=================================================================================================
std::shared_ptr<GeomAPI_DataMapOfShapeShape> GeomAlgoAPI_Partition::mapOfShapes() const
{
  return myMap;
}

//=================================================================================================
std::shared_ptr<GeomAlgoAPI_MakeShape> GeomAlgoAPI_Partition::makeShape() const
{
  return myMkShape;
}
