// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultField.cpp
// Created:     16 Nov 2016
// Author:      Mikhail PONIKAROV

#include "ModelAPI_ResultField.h"

ModelAPI_ResultField::~ModelAPI_ResultField()
{

}

std::string ModelAPI_ResultField::groupName()
{
  return group();
}
