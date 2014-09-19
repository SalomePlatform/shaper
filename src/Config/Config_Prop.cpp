// File:        Config_Prop.cpp
// Created:     18 Sep 2014
// Author:      Mikhail PONIKAROV


#include "Config_Prop.h"
#include "Events_Loop.h"
#include "Events_Message.h"

void Config_Prop::setValue(const std::string& theValue)
{
  if (theValue != myValue) {
    myValue = theValue;
    static const Events_ID aChangedEvent = Events_Loop::loop()->eventByName("PreferenceChanged");
    Events_Loop::loop()->send(boost::shared_ptr<Events_Message>(
      new Events_Message(aChangedEvent, this)));
  }
}
