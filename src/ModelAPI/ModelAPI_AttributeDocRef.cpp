// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeDocRef.cpp
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_AttributeDocRef.h"

std::string ModelAPI_AttributeDocRef::attributeType()
{
  return typeId();
}

ModelAPI_AttributeDocRef::~ModelAPI_AttributeDocRef()
{
}

ModelAPI_AttributeDocRef::ModelAPI_AttributeDocRef()
{
}
