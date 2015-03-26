// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeIntArray.cpp
// Created:     6 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <ModelAPI_AttributeIntArray.h>


std::string ModelAPI_AttributeIntArray::attributeType()
{
  return typeId();
}

/// To virtually destroy the fields of successors
ModelAPI_AttributeIntArray::~ModelAPI_AttributeIntArray()
{
}

ModelAPI_AttributeIntArray::ModelAPI_AttributeIntArray()
{
}
