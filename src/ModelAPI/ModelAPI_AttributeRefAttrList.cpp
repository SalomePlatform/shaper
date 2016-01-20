// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefAttrList.cpp
// Created:     20 Jan 2016
// Author:      Mikhail PONIKAROV


#include "ModelAPI_AttributeRefAttrList.h"

std::string ModelAPI_AttributeRefAttrList::attributeType()
{
  return typeId();
}

ModelAPI_AttributeRefAttrList::~ModelAPI_AttributeRefAttrList()
{

}

ModelAPI_AttributeRefAttrList::ModelAPI_AttributeRefAttrList()
{
}

