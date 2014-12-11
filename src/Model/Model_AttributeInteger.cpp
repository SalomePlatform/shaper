// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeInteger.cpp
// Created:     03 sep 2014
// Author:      sbh

#include <Model_AttributeInteger.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <Standard_TypeDef.hxx>
#include <TDataStd_Integer.hxx>

void Model_AttributeInteger::setValue(const int theValue)
{
  if (!myIsInitialized || myInteger->Get() != theValue) {
    myInteger->Set(theValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

int Model_AttributeInteger::value()
{
  return myInteger->Get();
}

Model_AttributeInteger::Model_AttributeInteger(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = theLabel.FindAttribute(TDataStd_Integer::GetID(), myInteger) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet, just zero
    myInteger = TDataStd_Integer::Set(theLabel, 0);
  }
}
