// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeString.cpp
// Created:     25 august 2014
// Author:      sbh

#include <Model_AttributeString.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>

#include <string>

void Model_AttributeString::setValue(const std::string& theValue)
{
  TCollection_ExtendedString aValue(theValue.c_str());
  if (!myIsInitialized || myString->Get() != aValue) {
    if (myString.IsNull())
      myString = TDataStd_Name::Set(myLab, TCollection_ExtendedString());
    myString->Set(aValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::string Model_AttributeString::value()
{
  if (myString.IsNull())
    return "";  // not initialized
  return TCollection_AsciiString(myString->Get()).ToCString();
}

Model_AttributeString::Model_AttributeString(TDF_Label& theLabel)
{
  myLab = theLabel;
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = theLabel.FindAttribute(TDataStd_Name::GetID(), myString) == Standard_True;
}
