// File:        ModelAPI_AttributeDouble.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeDouble.h"
#include "Model_Events.h"
#include <Events_Loop.h>

using namespace std;

void Model_AttributeDouble::setValue(const double theValue)
{
  if (myReal->Get() != theValue) {
    myReal->Set(theValue);
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
    Model_FeatureUpdatedMessage aMsg(owner(), anEvent);
    Events_Loop::loop()->send(aMsg);
  }
}

double Model_AttributeDouble::value()
{
  return myReal->Get();
}

Model_AttributeDouble::Model_AttributeDouble(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_Real::GetID(), myReal)) {
    // create attribute: not initialized by value yet, just zero
    myReal = TDataStd_Real::Set(theLabel, 0.);
  }
}
