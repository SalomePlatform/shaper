// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeStringArray.cpp
// Created:     14 Nov 2016
// Author:      Mikhail Ponikarov

#include "ModelAPI_AttributeTables.h"

//==================================================================================================
std::string ModelAPI_AttributeTables::attributeType()
{
  return typeId();
}

//==================================================================================================
ModelAPI_AttributeTables::~ModelAPI_AttributeTables()
{
}

//==================================================================================================
ModelAPI_AttributeTables::ModelAPI_AttributeTables()
{
}
