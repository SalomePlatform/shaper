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

//=================================================================================================
GeomAlgoAPI_PaveFiller::GeomAlgoAPI_PaveFiller(const ListOfShape& theListOfShape, const bool theIsMakeCompSolids)
{
  build(theListOfShape, theIsMakeCompSolids);
}


//=================================================================================================
void GeomAlgoAPI_PaveFiller::build(const ListOfShape& theListOfShape, const bool theIsMakeCompSolids)
{
  BOPAlgo_PaveFiller aPaveFiller;
  BOPCol_ListOfShape aListOfShape;
  for(ListOfShape::const_iterator anIt = theListOfShape.cbegin(); anIt != theListOfShape.cend(); anIt++) {
    aListOfShape.Append((*anIt)->impl<TopoDS_Shape>());
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
