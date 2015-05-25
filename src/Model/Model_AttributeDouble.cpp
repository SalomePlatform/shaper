// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeDouble.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeDouble.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Events.h>

#include <TCollection_AsciiString.hxx>
#include <TCollection_ExtendedString.hxx>
#include <TDataStd_UAttribute.hxx>

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
  if (!theLabel.FindAttribute(TDataStd_Name::GetID(), myText)) {
    myText = TDataStd_Name::Set(theLabel, TCollection_ExtendedString());
  }
}

void Model_AttributeDouble::setText(const std::string& theValue)
{
  TCollection_ExtendedString aValue(theValue.c_str());
  if (myText->Get() != aValue) {
    myText->Set(aValue);
    owner()->data()->sendAttributeUpdated(this);
    // Send it to evaluator to convert into the double and store in the attribute
    static Events_ID anId = ModelAPI_AttributeEvalMessage::eventId();
    std::shared_ptr<ModelAPI_AttributeEvalMessage> aMessage =
      std::shared_ptr<ModelAPI_AttributeEvalMessage>(new ModelAPI_AttributeEvalMessage(anId, this));
    aMessage->setAttribute(owner()->data()->attribute(id())); // to get shared pointer to this
    Events_Loop::loop()->send(aMessage);
  }
}

string Model_AttributeDouble::text()
{
  return TCollection_AsciiString(myText->Get()).ToCString();
}

Standard_GUID kInvalidGUID("caee5ce4-34b1-4b29-abcb-685287d18096");

void Model_AttributeDouble::setExpressionInvalid(const bool theFlag)
{
  if (theFlag) {
    TDataStd_UAttribute::Set(myReal->Label(), kInvalidGUID);
  } else {
    myReal->Label().ForgetAttribute(kInvalidGUID);
  }
}

bool Model_AttributeDouble::expressionInvalid()
{
  return myReal->Label().IsAttribute(kInvalidGUID) == Standard_True;
}
