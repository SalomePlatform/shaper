// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeDouble.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeDouble.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

using namespace std;

void Model_AttributeDouble::setValue(const double theValue)
{
  if (!myIsInitialized || myReal->Get() != theValue) {
    myReal->Set(theValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

double Model_AttributeDouble::value()
{
  return myReal->Get();
}

Model_AttributeDouble::Model_AttributeDouble(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = theLabel.FindAttribute(TDataStd_Real::GetID(), myReal) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet, just zero
    myReal = TDataStd_Real::Set(theLabel, 0.);
  }
}
