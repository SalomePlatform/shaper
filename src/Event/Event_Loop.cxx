// File:	Event_Loop.hxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#include <Event_Loop.hxx>
#include <string>

using namespace std;

Event_Loop* Event_Loop::Loop()
{
  // initialized on initialization of the application
  static Event_Loop MAIN_LOOP;
  return &MAIN_LOOP;
}

Event_ID Event_Loop::EventByName(const char* theName)
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

  return Event_ID(aResult);
}

void Event_Loop::Send(Event_Message& theMessage)
{
  // TO DO: make it in thread and wit husage of semaphores

  map<char*, map<void*, list<Event_Listener*> > >::iterator aFindID = 
    myListeners.find(theMessage.ID().EventText());
  if (aFindID != myListeners.end()) {
    map<void*, list<Event_Listener*> >::iterator aFindSender = 
      aFindID->second.find(theMessage.Sender());
    if (aFindSender != aFindID->second.end()) {
      list<Event_Listener*>& aListeners = aFindSender->second;
      for(list<Event_Listener*>::iterator aL = aListeners.begin(); aL != aListeners.end(); aL++)
        (*aL)->ProcessEvent(&theMessage);
    }
    if (theMessage.Sender()) { // also call for NULL senders registered
      aFindSender = aFindID->second.find(NULL);
      if (aFindSender != aFindID->second.end()) {
        list<Event_Listener*>& aListeners = aFindSender->second;
        for(list<Event_Listener*>::iterator aL = aListeners.begin(); aL != aListeners.end(); aL++)
          (*aL)->ProcessEvent(&theMessage);
      }
    }
  }
}

void Event_Loop::RegisterListener(
  Event_Listener* theListener, const Event_ID theID, void* theSender)
{
  map<char*, map<void*, list<Event_Listener*> > >::iterator aFindID = 
    myListeners.find(theID.EventText());
  if (aFindID == myListeners.end()) { // create container associated with ID
    myListeners[theID.EventText()] = map<void*, list<Event_Listener*> >();
    aFindID = myListeners.find(theID.EventText());
  }

  map<void*, list<Event_Listener*> >::iterator aFindSender = 
    aFindID->second.find(theSender);
  if (aFindSender == aFindID->second.end()) { // create container associated with sender
    aFindID->second[theSender] = list<Event_Listener*>();
    aFindSender = aFindID->second.find(theSender);
  }
  // check that listener was not registered wit hsuch parameters before
  list<Event_Listener*>& aListeners = aFindSender->second;
  for(list<Event_Listener*>::iterator aL = aListeners.begin(); aL != aListeners.end(); aL++)
    if (*aL == theListener) return; // avoid duplicates

  aListeners.push_back(theListener);
}
