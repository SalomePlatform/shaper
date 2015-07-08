// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultPart.cpp
// Created:     12 Jan 2015
// Author:      Sergey BELASH

#include <ModelAPI_ResultPart.h>

#include <string>

std::string ModelAPI_ResultPart::groupName()
{
  return ModelAPI_ResultPart::group();
}

bool ModelAPI_ResultPart::isConcealed()
{
  //return false;
  return ModelAPI_Result::isConcealed(); // the way with different results is applied
}
