// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_PaveFiller.cpp
// Created:     27 August 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_PaveFiller.h"

#include <GeomAlgoAPI_DFLoader.h>
#include <GeomAlgoAPI_ShapeTools.h>

#include <BOPAlgo_Builder.hxx>
#include <BOPAlgo_PaveFiller.hxx>
#include <TopoDS_Builder.hxx>
#include <TopoDS_Iterator.hxx>

//=================================================================================================
GeomAlgoAPI_PaveFiller::GeomAlgoAPI_PaveFiller(const ListOfShape& theListOfShape,
                                               const bool theIsMakeCompSolids)
{
  build(theListOfShape, theIsMakeCompSolids);
}


//=================================================================================================
void GeomAlgoAPI_PaveFiller::build(const ListOfShape& theListOfShape,
                                   const bool theIsMakeCompSolids)
{
  BOPAlgo_PaveFiller aPaveFiller;
  BOPCol_ListOfShape aListOfShape;
  for(ListOfShape::const_iterator
    anIt = theListOfShape.cbegin(); anIt != theListOfShape.cend(); anIt++) {
    const TopoDS_Shape& aShape = (*anIt)->impl<TopoDS_Shape>();
    if(aShape.ShapeType() == TopAbs_COMPOUND) {
      for(TopoDS_Iterator anIter(aShape); anIter.More(); anIter.Next()) {
        aListOfShape.Append(anIter.Value());
      }
    } else {
      aListOfShape.Append(aShape);
    }
  }
  aPaveFiller.SetArguments(aListOfShape);
  aPaveFiller.Perform();
  Standard_Integer iErr = aPaveFiller.ErrorStatus();
  if(iErr) {
    return;
  }

  BOPAlgo_Builder* aBuilder = new BOPAlgo_Builder();
  this->setImpl(aBuilder);
  this->setBuilderType(OCCT_BOPAlgo_Builder);
  aBuilder->SetArguments(aListOfShape);
  aBuilder->PerformWithFiller(aPaveFiller);
  iErr = aBuilder->ErrorStatus();
  if(iErr) {
    return;
  }

  TopoDS_Shape aResult = aBuilder->Shape();
  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }
  if(theIsMakeCompSolids && aResult.ShapeType() == TopAbs_COMPOUND) {
    std::shared_ptr<GeomAPI_Shape> aGeomShape(new GeomAPI_Shape);
    aGeomShape->setImpl(new TopoDS_Shape(aResult));
    ListOfShape aCompSolids, aFreeSolids;
    aGeomShape = GeomAlgoAPI_ShapeTools::combineShapes(aGeomShape,
                                                       GeomAPI_Shape::COMPSOLID,
                                                       aCompSolids,
                                                       aFreeSolids);
    aResult = aGeomShape->impl<TopoDS_Shape>();
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
