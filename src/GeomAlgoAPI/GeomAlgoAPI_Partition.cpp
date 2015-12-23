// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Partition.cpp
// Created:     21 Aug 2015
// Author:      Sergey POKHODENKO

#include "GeomAlgoAPI_Partition.h"

#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <GEOMAlgo_Splitter.hxx>

#include <TopExp_Explorer.hxx>
#include <TopoDS_Builder.hxx>

//=================================================================================================
std::shared_ptr<GeomAPI_Shape> GeomAlgoAPI_Partition::make(const ListOfShape& theObjects,
                                                           const ListOfShape& theTools)
{
  GeomAlgoAPI_Partition aPartitionAlgo(theObjects, theTools);
  if(aPartitionAlgo.isDone() && !aPartitionAlgo.shape()->isNull() && aPartitionAlgo.isValid()) {
    return aPartitionAlgo.shape();
  }
  return std::shared_ptr<GeomAPI_Shape>();
}

//=================================================================================================
GeomAlgoAPI_Partition::GeomAlgoAPI_Partition(const ListOfShape& theObjects,
                                             const ListOfShape& theTools)
{
  build(theObjects, theTools);
}


//=================================================================================================
void GeomAlgoAPI_Partition::build(const ListOfShape& theObjects,
                                  const ListOfShape& theTools)
{
  if (theObjects.empty()) {
    return;
  }

  // Creating partition operation.
  GEOMAlgo_Splitter* anOperation = new GEOMAlgo_Splitter;
  this->setImpl(anOperation);
  this->setBuilderType(OCCT_BOPAlgo_Builder);

  // Getting objects.
  for (ListOfShape::const_iterator anObjectsIt = theObjects.begin(); anObjectsIt != theObjects.end(); anObjectsIt++) {
    const TopoDS_Shape& aShape = (*anObjectsIt)->impl<TopoDS_Shape>();
    anOperation->AddArgument(aShape);
  }

  // Getting tools.
  for (ListOfShape::const_iterator aToolsIt = theTools.begin(); aToolsIt != theTools.end(); aToolsIt++) {
    const TopoDS_Shape& aShape = (*aToolsIt)->impl<TopoDS_Shape>();
    anOperation->AddTool(aShape);
  }

  // Building and getting result.
  anOperation->Perform();
  if(anOperation->ErrorStatus() != 0) {
    return;
  }
  TopoDS_Shape aResult = anOperation->Shape();

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    std::shared_ptr<GeomAPI_Shape> aCompound(new GeomAPI_Shape);
    aCompound->setImpl(new TopoDS_Shape(aResult));
    ListOfShape aCompSolids, aFreeSolids;
    GeomAlgoAPI_ShapeTools::combineShapes(aCompound, GeomAPI_Shape::COMPSOLID, aCompSolids, aFreeSolids);
    if(aCompSolids.size() == 1 && aFreeSolids.size() == 0) {
      aResult = aCompSolids.front()->impl<TopoDS_Shape>();
    } else if (aCompSolids.size() > 1 || (aCompSolids.size() >= 1 && aFreeSolids.size() >= 1)) {
      TopoDS_Compound aResultComp;
      TopoDS_Builder aBuilder;
      aBuilder.MakeCompound(aResultComp);
      for(ListOfShape::const_iterator anIter = aCompSolids.cbegin(); anIter != aCompSolids.cend(); anIter++) {
        aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
      }
      for(ListOfShape::const_iterator anIter = aFreeSolids.cbegin(); anIter != aFreeSolids.cend(); anIter++) {
        aBuilder.Add(aResultComp, (*anIter)->impl<TopoDS_Shape>());
      }
      aResult = aResultComp;
    }
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
