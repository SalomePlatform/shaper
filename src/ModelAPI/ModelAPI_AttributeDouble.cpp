// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeDouble.cpp
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_AttributeDouble.h"

std::string ModelAPI_AttributeDouble::attributeType()
{
  return typeId();
}

ModelAPI_AttributeDouble::~ModelAPI_AttributeDouble()
{
}

ModelAPI_AttributeDouble::ModelAPI_AttributeDouble()
{
}
