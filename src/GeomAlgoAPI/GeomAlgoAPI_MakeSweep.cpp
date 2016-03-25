// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeSweep.cpp
// Created:     23 November 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_MakeSweep.h"

//=================================================================================================
const ListOfShape& GeomAlgoAPI_MakeSweep::fromShapes() const
{
  return myFromShapes;
}

//=================================================================================================
const ListOfShape& GeomAlgoAPI_MakeSweep::toShapes() const
{
  return myToShapes;
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::addFromShape(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  myFromShapes.push_back(theFace);
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::setFromShapes(const ListOfShape& theListOfFaces)
{
  myFromShapes = theListOfFaces;
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::addToShape(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  myToShapes.push_back(theFace);
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::setToShapes(const ListOfShape& theListOfFaces)
{
  myToShapes = theListOfFaces;
}
