// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeDoubleArray.cpp
// Created:     8 July 2016
// Author:      Dmitry Bobylev

#include "ModelAPI_AttributeDoubleArray.h"

//==================================================================================================
std::string ModelAPI_AttributeDoubleArray::attributeType()
{
  return typeId();
}

//==================================================================================================
ModelAPI_AttributeDoubleArray::~ModelAPI_AttributeDoubleArray()
{
}

//==================================================================================================
ModelAPI_AttributeDoubleArray::ModelAPI_AttributeDoubleArray()
{
}
