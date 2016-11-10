// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:	Events_Listener.cxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#include <Events_Listener.h>
#include <Events_MessageGroup.h>
#include <Events_Loop.h>

void Events_Listener::groupWhileFlush(const std::shared_ptr<Events_Message>& theMessage) {
  std::shared_ptr<Events_MessageGroup> aGroup =
    std::dynamic_pointer_cast<Events_MessageGroup>(theMessage);
  if (aGroup) {
    std::map<char*, std::shared_ptr<Events_Message> >::iterator aMyGroup = myGroups.find(
      aGroup->eventID().eventText());
    if (aMyGroup == myGroups.end()) {  // create a new group of messages for accumulation
      myGroups[aGroup->eventID().eventText()] = aGroup->newEmpty();
      aMyGroup = myGroups.find(aGroup->eventID().eventText());
    }
    std::shared_ptr<Events_MessageGroup> aStored =
      std::dynamic_pointer_cast<Events_MessageGroup>(aMyGroup->second);
    aStored->Join(aGroup);
    //std::cout<<"Add to group "<<theMessage->eventID().eventText()<<std::endl;
    return;
  }
}

void Events_Listener::flushGrouped(const Events_ID& theID) {
  std::map<char*, std::shared_ptr<Events_Message> >::iterator aMyGroup =
    myGroups.find(theID.eventText());
  if (aMyGroup != myGroups.end()) {
    std::shared_ptr<Events_Message> aMessage = aMyGroup->second;
    myGroups.erase(aMyGroup);
    processEvent(aMessage);
  }
}
