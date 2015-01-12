// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Result.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_Result.h"

ModelAPI_Result::~ModelAPI_Result()
{

}

std::shared_ptr<GeomAPI_Shape> ModelAPI_Result::shape()
{
  return std::shared_ptr<GeomAPI_Shape>();
}
