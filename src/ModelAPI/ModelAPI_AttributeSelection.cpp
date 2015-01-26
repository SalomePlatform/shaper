// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeSelection.cpp
// Created:     2 Oct 2014
// Author:      Mikhail PONIKAROV


#include "ModelAPI_AttributeSelection.h"


std::string ModelAPI_AttributeSelection::attributeType()
{
  return type();
}

ModelAPI_AttributeSelection::~ModelAPI_AttributeSelection()
{
}

ModelAPI_AttributeSelection::ModelAPI_AttributeSelection()
{
}
