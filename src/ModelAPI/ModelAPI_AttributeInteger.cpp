// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeInteger.cpp
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include <ModelAPI_AttributeInteger.h>


std::string ModelAPI_AttributeInteger::attributeType()
{
  return type();
}

/// To virtually destroy the fields of successors
ModelAPI_AttributeInteger::~ModelAPI_AttributeInteger()
{
}

ModelAPI_AttributeInteger::ModelAPI_AttributeInteger()
{
}
