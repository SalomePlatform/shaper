// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeBoolean.cpp
// Created:     2 june 2014
// Author:      Vitaly Smetannikov

#include "Model_AttributeBoolean.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

using namespace std;

void Model_AttributeBoolean::setValue(bool theValue)
{
  Standard_Boolean aValue = theValue ? Standard_True : Standard_False;
  if (!myIsInitialized || myBool->Get() != aValue) {
    myBool->Set(aValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

bool Model_AttributeBoolean::value()
{
  return myBool->Get() == Standard_True ;
}

Model_AttributeBoolean::Model_AttributeBoolean(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = theLabel.FindAttribute(TDataStd_Integer::GetID(), myBool) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet, just zero
    myBool = TDataStd_Integer::Set(theLabel, 0);
  }
}
