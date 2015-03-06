// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeColor.cpp
// Created:     6 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <ModelAPI_AttributeColor.h>


std::string ModelAPI_AttributeColor::attributeType()
{
  return type();
}

/// To virtually destroy the fields of successors
ModelAPI_AttributeColor::~ModelAPI_AttributeColor()
{
}

ModelAPI_AttributeColor::ModelAPI_AttributeColor()
{
}
