// File:        Model_AttributeString.cpp
// Created:     2 june 2014
// Author:      Vitaly Smetannikov

#include <Model_AttributeString.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Attribute.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <Standard_TypeDef.hxx>
#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_Name.hxx>

#include <string>

void Model_AttributeString::setValue(const std::string& theValue)
{
  TCollection_ExtendedString aValue(theValue.c_str());
  if (!myIsInitialized || myString->Get() != aValue) {
    myString->Set(aValue);
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::string Model_AttributeString::value()
{
  return TCollection_AsciiString(myString->Get()).ToCString();
}

Model_AttributeString::Model_AttributeString(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  myIsInitialized = theLabel.FindAttribute(TDataStd_Integer::GetID(), myString) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet, just empty string
    myString = TDataStd_Name::Set(theLabel, TCollection_ExtendedString());
  }
}
