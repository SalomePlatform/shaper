// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeReference.cpp
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_AttributeReference.h"

std::string ModelAPI_AttributeReference::attributeType()
{
  return type();
}

ModelAPI_AttributeReference::~ModelAPI_AttributeReference()
{
}

ModelAPI_AttributeReference::ModelAPI_AttributeReference()
{
}
