// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Attribute.cpp
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include <ModelAPI_Attribute.h>

ModelAPI_Attribute::~ModelAPI_Attribute()
{
}

  /// Sets the owner of this attribute
void ModelAPI_Attribute::setObject(const std::shared_ptr<ModelAPI_Object>& theObject)
{
  myObject = theObject;
}

  /// Returns the owner of this attribute
const std::shared_ptr<ModelAPI_Object>& ModelAPI_Attribute::owner() const
{
  return myObject;
}

  /// Returns true if attribute was  initialized by some value
bool ModelAPI_Attribute::isInitialized()
{
  return myIsInitialized;
}

void ModelAPI_Attribute::setInitialized()
{
  myIsInitialized = true;
}

void ModelAPI_Attribute::setIsArgument(const bool theFlag)
{
  myIsArgument = theFlag;
}

bool ModelAPI_Attribute::isArgument()
{
  return myIsArgument;
}

bool ModelAPI_Attribute::setImmutable(const bool theFlag)
{
  bool aResult = myIsImmutable;
  myIsImmutable = theFlag;
  return aResult;
}

bool ModelAPI_Attribute::isImmutable()
{
  return myIsImmutable;
}

const std::string& ModelAPI_Attribute::id() const
{
  return myID;
}

ModelAPI_Attribute::ModelAPI_Attribute()
{
  myIsInitialized = false;
  myIsArgument = true;
  myIsImmutable = false;
}

  /// Sets the ID of the attribute in Data (called from Data)
void ModelAPI_Attribute::setID(const std::string theID)
{
  myID = theID;
}
