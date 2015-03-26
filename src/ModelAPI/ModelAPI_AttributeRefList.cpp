// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefList.cpp
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV


#include "ModelAPI_AttributeRefList.h"

std::string ModelAPI_AttributeRefList::attributeType()
{
  return typeId();
}

ModelAPI_AttributeRefList::~ModelAPI_AttributeRefList()
{

}

ModelAPI_AttributeRefList::ModelAPI_AttributeRefList()
{
}

