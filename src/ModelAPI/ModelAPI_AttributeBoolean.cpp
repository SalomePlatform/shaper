// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeBoolean.cpp
// Created:     2 june 2014
// Author:      Vitaly Smetannikov

#include "ModelAPI_AttributeBoolean.h"

std::string ModelAPI_AttributeBoolean::attributeType()
{
  return type();
}

/// To virtually destroy the fields of successors
ModelAPI_AttributeBoolean::~ModelAPI_AttributeBoolean()
{
}

ModelAPI_AttributeBoolean::ModelAPI_AttributeBoolean()
{
}
