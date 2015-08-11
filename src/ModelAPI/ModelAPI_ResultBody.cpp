// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultBody.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_ResultBody.h"
#include <ModelAPI_BodyBuilder.h>

ModelAPI_ResultBody::ModelAPI_ResultBody()
: myBuilder(0)
{
}

ModelAPI_ResultBody::~ModelAPI_ResultBody()
{
  if (myBuilder)
  delete myBuilder;
}

std::string ModelAPI_ResultBody::groupName()
{
  return group();
}

