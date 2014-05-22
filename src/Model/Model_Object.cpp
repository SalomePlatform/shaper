// File:        Model_Object.cpp
// Created:     19 May 2014
// Author:      Mikhail PONIKAROV

#include "Model_Object.h"
#include <TCollection_AsciiString.hxx>
#include "Model_Events.h"
#include <Events_Loop.h>

boost::shared_ptr<ModelAPI_Feature> Model_Object::featureRef()
{
  return myRef;
}

std::string Model_Object::getName()
{
  return TCollection_AsciiString(myName->Get()).ToCString();
}

void Model_Object::setName(std::string theName)
{
  if (myName->Get() != theName.c_str()) {
    myName->Set(theName.c_str());
    /*
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
    Model_FeatureUpdatedMessage aMsg(boost::shared_ptr<ModelAPI_Object>(this), anEvent);
    Events_Loop::loop()->send(aMsg, false);
    */
  }
}

Model_Object::Model_Object(boost::shared_ptr<ModelAPI_Feature> theRef,
                           Handle_TDataStd_Name theName)
                           : myRef(theRef), myName(theName)
{
}
