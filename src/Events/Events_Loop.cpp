// File:	Events_Loop.hxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#include <Events_Loop.h>

#include <string>
#include <cstring>

using namespace std;

Events_Loop* Events_Loop::loop()
{
  // initialized on initialization of the application
  static Events_Loop MAIN_LOOP;
  return &MAIN_LOOP;
}

Events_ID Events_Loop::eventByName(const char* theName)
{
  ///! All events created in this session, uniquely identified by the text and char pointer
  static map<string, char*> CREATED_EVENTS;
  char* aResult;
  string aName(theName);
  map<string, char*>::iterator aFound = CREATED_EVENTS.find(aName);
  if (aFound == CREATED_EVENTS.end()) { //not created yet
    aResult = strdup(theName); // copy to make unique internal pointer
    CREATED_EVENTS[aName] = aResult;
  } else
    aResult = aFound->second;

  return Events_ID(aResult);
}

void Events_Loop::send(Events_Message& theMessage)
{
  // TO DO: make it in thread and wit husage of semaphores

  map<char*, map<void*, list<Events_Listener*> > >::iterator aFindID = myListeners.find(
      theMessage.eventID().eventText());
  if (aFindID != myListeners.end()) {
    map<void*, list<Events_Listener*> >::iterator aFindSender = aFindID->second.find(
        theMessage.sender());
    if (aFindSender != aFindID->second.end()) {
      list<Events_Listener*>& aListeners = aFindSender->second;
      for(list<Events_Listener*>::iterator aL = aListeners.begin(); aL != aListeners.end(); aL++)
        (*aL)->processEvent(&theMessage);
    }
    if (theMessage.sender()) { // also call for NULL senders registered
      aFindSender = aFindID->second.find(NULL);
      if (aFindSender != aFindID->second.end()) {
        list<Events_Listener*>& aListeners = aFindSender->second;
        for(list<Events_Listener*>::iterator aL = aListeners.begin(); aL != aListeners.end(); aL++)
          (*aL)->processEvent(&theMessage);
      }
    }
  }
}

void Events_Loop::registerListener(Events_Listener* theListener, const Events_ID theID,
                                  void* theSender)
{
  map<char*, map<void*, list<Events_Listener*> > >::iterator aFindID = myListeners.find(
      theID.eventText());
  if (aFindID == myListeners.end()) { // create container associated with ID
    myListeners[theID.eventText()] = map<void*, list<Events_Listener*> >();
    aFindID = myListeners.find(theID.eventText());
  }

  map<void*, list<Events_Listener*> >::iterator aFindSender = aFindID->second.find(theSender);
  if (aFindSender == aFindID->second.end()) { // create container associated with sender
    aFindID->second[theSender] = list<Events_Listener*>();
    aFindSender = aFindID->second.find(theSender);
  }
  // check that listener was not registered wit hsuch parameters before
  list<Events_Listener*>& aListeners = aFindSender->second;
  for(list<Events_Listener*>::iterator aL = aListeners.begin(); aL != aListeners.end(); aL++)
    if (*aL == theListener)
      return; // avoid duplicates

  aListeners.push_back(theListener);
}
