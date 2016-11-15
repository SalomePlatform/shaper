// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeStringArray.cpp
// Created:     14 Nov 2016
// Author:      Mikhail Ponikarov

#include "ModelAPI_AttributeStringArray.h"

//==================================================================================================
std::string ModelAPI_AttributeStringArray::attributeType()
{
  return typeId();
}

//==================================================================================================
ModelAPI_AttributeStringArray::~ModelAPI_AttributeStringArray()
{
}

//==================================================================================================
ModelAPI_AttributeStringArray::ModelAPI_AttributeStringArray()
{
}
