// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultCompSolid.cpp
// Created:     20 Jul 2015
// Author:      Natalia ERMOLAEVA

#include "ModelAPI_ResultCompSolid.h"

ModelAPI_ResultCompSolid::~ModelAPI_ResultCompSolid()
{
}

std::string ModelAPI_ResultCompSolid::groupName()
{
  return group();
}

void ModelAPI_ResultCompSolid::initAttributes()
{
}

bool ModelAPI_ResultCompSolid::isDisabled() const
{
  return false;
}


