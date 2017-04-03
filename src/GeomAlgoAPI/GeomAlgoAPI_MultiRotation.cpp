// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        GeomAlgoAPI_MultiRotation.cpp
// Created:     30 Mar 2017
// Author:      Clarisse Genrault (CEA)

#include "GeomAlgoAPI_MultiRotation.h"

//=================================================================================================
GeomAlgoAPI_MultiRotation::GeomAlgoAPI_MultiRotation(std::shared_ptr<GeomAPI_Shape> theSourceShape,
                                           std::shared_ptr<GeomAPI_Ax1>   theAxis,
                                           double                         theAngle)
{
}

//=================================================================================================
bool GeomAlgoAPI_MultiRotation::check()
{
  return true;
}

//=================================================================================================
void GeomAlgoAPI_MultiRotation::build()
{
  setDone(true);
}
