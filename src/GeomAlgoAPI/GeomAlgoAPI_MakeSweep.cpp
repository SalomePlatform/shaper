// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MakeSweep.cpp
// Created:     23 November 2015
// Author:      Dmitry Bobylev

#include <GeomAlgoAPI_MakeSweep.h>

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
