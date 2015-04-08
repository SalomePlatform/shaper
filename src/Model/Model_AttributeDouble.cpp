// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeDouble.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeDouble.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

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
    myText = TDataStd_Name::Set(theLabel, TCollection_ExtendedString());
  }
}

void Model_AttributeDouble::setText(const std::string& theValue)
{
  TCollection_ExtendedString aValue(theValue.c_str());
  if (myText->Get() != aValue) {
    myText->Set(aValue);
    //owner()->data()->sendAttributeUpdated(this); ?
  }
}

string Model_AttributeDouble::text()
{
  return TCollection_AsciiString(myText->Get()).ToCString();
}
