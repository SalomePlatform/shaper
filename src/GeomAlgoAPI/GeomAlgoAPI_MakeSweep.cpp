// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeSweep.cpp
// Created:     23 November 2015
// Author:      Dmitry Bobylev

#include "GeomAlgoAPI_MakeSweep.h"

//=================================================================================================
const ListOfShape& GeomAlgoAPI_MakeSweep::fromFaces() const
{
  return myFromFaces;
}

//=================================================================================================
const ListOfShape& GeomAlgoAPI_MakeSweep::toFaces() const
{
  return myToFaces;
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::addFromFace(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  myFromFaces.push_back(theFace);
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::setFromFaces(const ListOfShape& theListOfFaces)
{
  myFromFaces = theListOfFaces;
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::addToFace(const std::shared_ptr<GeomAPI_Shape> theFace)
{
  myToFaces.push_back(theFace);
}

//=================================================================================================
void GeomAlgoAPI_MakeSweep::setToFaces(const ListOfShape& theListOfFaces)
{
  myToFaces = theListOfFaces;
}
