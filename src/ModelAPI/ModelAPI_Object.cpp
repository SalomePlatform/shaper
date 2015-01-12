// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Object.cpp
// Created:     19 May 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_Object.h"


bool ModelAPI_Object::isInHistory()
{
  return true;
}

std::shared_ptr<ModelAPI_Data> ModelAPI_Object::data() const
{
  return myData;
}

bool ModelAPI_Object::isSame(const std::shared_ptr<ModelAPI_Object>& theObject)
{
  return theObject.get() == this;
}

std::shared_ptr<ModelAPI_Document> ModelAPI_Object::document() const
{
  return myDoc;
}

void ModelAPI_Object::attributeChanged(const std::string& theID)
{
}

ModelAPI_Object::~ModelAPI_Object()
{
}

void ModelAPI_Object::setData(std::shared_ptr<ModelAPI_Data> theData)
{
  myData = theData;
}

void ModelAPI_Object::setDoc(std::shared_ptr<ModelAPI_Document> theDoc)
{
  myDoc = theDoc;
}

void ModelAPI_Object::erase()
{
  if (myData) myData->erase();
  setData(DataPtr());
}
