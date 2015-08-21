// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultCompSolid.cpp
// Created:     20 Jul 2015
// Author:      Natalia ERMOLAEVA

#include "ModelAPI_ResultCompSolid.h"

ModelAPI_ResultCompSolid::~ModelAPI_ResultCompSolid()
{
}

void ModelAPI_ResultCompSolid::setDisplayed(const bool theDisplay)
{
  ModelAPI_ResultBody::setDisplayed(theDisplay);
  for (int i = 0; i < numberOfSubs(); i++)
    subResult(i)->setDisplayed(theDisplay);
}
