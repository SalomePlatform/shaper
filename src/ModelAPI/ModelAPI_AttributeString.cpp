// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeString.cpp
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV


#include "ModelAPI_AttributeString.h"

std::string ModelAPI_AttributeString::attributeType()
{
  return type();
}

ModelAPI_AttributeString::~ModelAPI_AttributeString()
{
}

ModelAPI_AttributeString::ModelAPI_AttributeString()
{
}
