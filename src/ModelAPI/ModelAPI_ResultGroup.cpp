// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultGroup.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_ResultGroup.h"

ModelAPI_ResultGroup::~ModelAPI_ResultGroup()
{

}

std::string ModelAPI_ResultGroup::groupName()
{
  return group();
}

