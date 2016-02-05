// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_Boolean.cpp
// Created:     02 Sept 2014
// Author:      Vitaly Smetannikov

#include "GeomAlgoAPI_Boolean.h"

#include <GeomAlgoAPI_DFLoader.h>

#include <BOPAlgo_BOP.hxx>
#include <TopTools_ListOfShape.hxx>

//=================================================================================================
GeomAlgoAPI_Boolean::GeomAlgoAPI_Boolean(const ListOfShape& theObjects,
                                         const ListOfShape& theTools,
                                         const OperationType theOperationType)
{
  build(theObjects, theTools, theOperationType);
}


//=================================================================================================
void GeomAlgoAPI_Boolean::build(const ListOfShape& theObjects,
                                const ListOfShape& theTools,
                                const OperationType theOperationType)
{
  if(theObjects.empty() || theTools.empty()) {
    return;
  }

  // Getting objects.
  TopTools_ListOfShape anObjects;
  for(ListOfShape::const_iterator anObjectsIt = theObjects.begin(); anObjectsIt != theObjects.end(); anObjectsIt++)
  {
    anObjects.Append((*anObjectsIt)->impl<TopoDS_Shape>());
  }

  // Getting tools.
  TopTools_ListOfShape aTools;
  for(ListOfShape::const_iterator aToolsIt = theTools.begin(); aToolsIt != theTools.end(); aToolsIt++)
  {
    aTools.Append((*aToolsIt)->impl<TopoDS_Shape>());
  }

  // Creating boolean operation.
  BOPAlgo_BOP* aBuilder = new BOPAlgo_BOP();
  switch (theOperationType) {
    case BOOL_CUT: {
      aBuilder->SetOperation(BOPAlgo_CUT);
      break;
    }
    case BOOL_FUSE: {
      aBuilder->SetOperation(BOPAlgo_FUSE);
      break;
    }
    case BOOL_COMMON: {
      aBuilder->SetOperation(BOPAlgo_COMMON);
      break;
    }
    default: {
      return;
    }
  }
  this->setImpl(aBuilder);
  this->setBuilderType(OCCT_BOPAlgo_Builder);
  aBuilder->SetArguments(anObjects);
  aBuilder->SetTools(aTools);

  // Building and getting result.
  aBuilder->Perform();
  if(aBuilder->ErrorStatus() != 0) {
    return;
  }
  TopoDS_Shape aResult = aBuilder->Shape();

  if(aResult.ShapeType() == TopAbs_COMPOUND) {
    aResult = GeomAlgoAPI_DFLoader::refineResult(aResult);
  }

  std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape());
  aShape->setImpl(new TopoDS_Shape(aResult));
  this->setShape(aShape);
  this->setDone(true);
}
