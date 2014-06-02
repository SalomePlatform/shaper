// File:        Model_AttributeBoolean.cpp
// Created:     2 june 2014
// Author:      Vitaly Smetannikov

#include "Model_AttributeBoolean.h"
#include "Model_Events.h"
#include <Events_Loop.h>

using namespace std;

void Model_AttributeBoolean::setValue(bool theValue)
{
  if (myBool->Get() != theValue) {
    myBool->Set(theValue? 1 : 0);
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
    Model_FeatureUpdatedMessage aMsg(owner(), anEvent);
    Events_Loop::loop()->send(aMsg);
  }
}

bool Model_AttributeBoolean::value()
{
  return (myBool->Get() == 1)? true : false;
}

Model_AttributeBoolean::Model_AttributeBoolean(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_Integer::GetID(), myBool)) {
    // create attribute: not initialized by value yet, just zero
    myBool = TDataStd_Integer::Set(theLabel, 0);
  }
}
