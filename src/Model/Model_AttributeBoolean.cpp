// File:        Model_AttributeBoolean.cpp
// Created:     2 june 2014
// Author:      Vitaly Smetannikov

#include "Model_AttributeBoolean.h"
#include "Model_Events.h"
#include <Events_Loop.h>

using namespace std;

void Model_AttributeBoolean::setValue(bool theValue)
{
  Standard_Boolean aValue = theValue ? Standard_True : Standard_False;
  if (myBool->Get() != aValue) {
    myBool->Set(aValue);
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
    Model_FeatureUpdatedMessage aMsg(owner(), anEvent);
    Events_Loop::loop()->send(aMsg);
  }
}

bool Model_AttributeBoolean::value()
{
  return myBool->Get() == Standard_True;
}

Model_AttributeBoolean::Model_AttributeBoolean(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_Integer::GetID(), myBool)) {
    // create attribute: not initialized by value yet, just zero
    myBool = TDataStd_Integer::Set(theLabel, 0);
  }
}
