// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefAttr.cpp
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV


#include "ModelAPI_AttributeRefAttr.h"


std::string ModelAPI_AttributeRefAttr::attributeType()
{
  return typeId();
}

ModelAPI_AttributeRefAttr::~ModelAPI_AttributeRefAttr()
{
}

ModelAPI_AttributeRefAttr::ModelAPI_AttributeRefAttr()
{
}
