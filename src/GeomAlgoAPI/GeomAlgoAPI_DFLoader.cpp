// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

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
    } else {
      /// MPV: store compound anyway: it may be Boolean operation that produces two solids from one
      aResult = theResult;
    }
  }
  return aResult;
}
