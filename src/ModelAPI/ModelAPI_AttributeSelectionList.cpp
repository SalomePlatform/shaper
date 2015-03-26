// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeSelectionList.cpp
// Created:     22 Oct 2014
// Author:      Mikhail PONIKAROV


#include "ModelAPI_AttributeSelectionList.h"

std::string ModelAPI_AttributeSelectionList::attributeType()
{
  return typeId();
}

ModelAPI_AttributeSelectionList::~ModelAPI_AttributeSelectionList()
{
}

MODELAPI_EXPORT ModelAPI_AttributeSelectionList::ModelAPI_AttributeSelectionList()
{
}
