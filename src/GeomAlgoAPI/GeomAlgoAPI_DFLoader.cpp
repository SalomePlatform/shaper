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
const TopoDS_Shape GeomAlgoAPI_DFLoader::refineResult(const TopoDS_Shape& theResult)
{
  TopoDS_Shape aResult = theResult;
  const TopAbs_ShapeEnum aShType = theResult.ShapeType();
  if(aShType == TopAbs_COMPOUND || aShType == TopAbs_SHELL || aShType == TopAbs_WIRE) {
    Standard_Integer aSubResultsNb = 0;
    TopoDS_Iterator anIt(theResult);
    for(; anIt.More(); anIt.Next()) {
      ++aSubResultsNb;
    }
    if(aSubResultsNb == 1) {
      anIt.Initialize(theResult);
      if(anIt.More()) {
        aResult = anIt.Value();
      }
    }
  }

  return aResult;
}
