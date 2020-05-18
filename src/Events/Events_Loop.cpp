// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Events_Loop.h>
#include <Events_MessageGroup.h>

#include <string>
#include <cstring>

Events_Loop* Events_Loop::loop()
{
  // initialized on initialization of the application
  static Events_Loop MAIN_LOOP;
  return &MAIN_LOOP;
}

Events_ID Events_Loop::eventByName(const char* theName)
{
  ///! All events created in this session, uniquely identified by the text and char pointer
  static std::map<std::string, char*> CREATED_EVENTS;
  char* aResult;
  std::string aName(theName);
  std::map<std::string, char*>::iterator aFound = CREATED_EVENTS.find(aName);
  if (aFound == CREATED_EVENTS.end()) {  //not created yet
#ifdef WIN32
    aResult = _strdup(theName);  // copy to make unique internal pointer
#else
    aResult = strdup(theName);  // copy to make unique internal pointer
#endif
    CREATED_EVENTS[aName] = aResult;
  } else
    aResult = aFound->second;

  return Events_ID(aResult);
}

void Events_Loop::sendProcessEvent(const std::shared_ptr<Events_Message>& theMessage,
  std::list<Events_Listener*>& theListeners, const bool theFlushedNow)
{
  std::list<Events_Listener*>::iterator aL = theListeners.begin();
  for (; aL != theListeners.end(); aL++) {
    if (theFlushedNow && (*aL)->groupMessages()) {
      (*aL)->groupWhileFlush(theMessage);
    } else {
      (*aL)->processEvent(theMessage);
    }
  }
}

void Events_Loop::send(const std::shared_ptr<Events_Message>& theMessage, bool isGroup)
{
  if (myImmediateListeners.find(theMessage->eventID().eventText()) != myImmediateListeners.end()) {
    myImmediateListeners[theMessage->eventID().eventText()]->processEvent(theMessage);
  }
  // if it is grouped message, just accumulate it
  bool isFlushedNow = myFlushed.find(theMessage->eventID().myID) != myFlushed.end();
  if (isGroup && !isFlushedNow) {
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
      return;
    }
  }
  // send
  std::map<char*, std::map<void*, std::list<Events_Listener*> > >::iterator aFindID =
    myListeners.find(theMessage->eventID().eventText());
  if (aFindID != myListeners.end()) {
    std::map<void*, std::list<Events_Listener*> >::iterator aFindSender = aFindID->second.find(
        theMessage->sender());
    if (aFindSender != aFindID->second.end()) {
      sendProcessEvent(theMessage, aFindSender->second, isFlushedNow && isGroup);
    }
    if (theMessage->sender()) {  // also call for NULL senders registered
      aFindSender = aFindID->second.find(NULL);
      if (aFindSender != aFindID->second.end()) {
        sendProcessEvent(theMessage, aFindSender->second, isFlushedNow && isGroup);
      }
    }
  }
}

void Events_Loop::registerListener(Events_Listener* theListener, const Events_ID theID,
                                   void* theSender, bool theImmediate)
{
  if (theImmediate) { // just register as an immediate
    myImmediateListeners[theID.eventText()] = theListener;
    return;
  }
  std::map<char*, std::map<void*, std::list<Events_Listener*> > >::iterator aFindID =
    myListeners.find(theID.eventText());
  if (aFindID == myListeners.end()) {  // create container associated with ID
    myListeners[theID.eventText()] = std::map<void*, std::list<Events_Listener*> >();
    aFindID = myListeners.find(theID.eventText());
  }

  std::map<void*, std::list<Events_Listener*> >::iterator aFindSender =
    aFindID->second.find(theSender);
  if (aFindSender == aFindID->second.end()) {  // create container associated with sender
    aFindID->second[theSender] = std::list<Events_Listener*>();
    aFindSender = aFindID->second.find(theSender);
  }
  // check that listener was not registered with such parameters before
  std::list<Events_Listener*>& aListeners = aFindSender->second;
  for (std::list<Events_Listener*>::iterator aL = aListeners.begin(); aL != aListeners.end(); aL++)
    if (*aL == theListener)
      return;  // avoid duplicates

  aListeners.push_back(theListener);
}

void Events_Loop::removeListener(Events_Listener* theListener)
{
  // remove the listener in myListeners map
  std::map<char*, std::map<void*, std::list<Events_Listener*> > >::const_reverse_iterator
                                                          anIt = myListeners.rbegin();
  while(anIt != myListeners.rend()) {
    std::map<void*, std::list<Events_Listener*> > aLMap = anIt->second;
    std::map<void*, std::list<Events_Listener*> >::const_reverse_iterator aLIt = aLMap.rbegin();
    while (aLIt != aLMap.rend()) {
      std::list<Events_Listener*> aListeners = aLIt->second;
      std::list<Events_Listener*>::const_reverse_iterator aLsIt = aListeners.rbegin();
      for (; aLsIt != aListeners.rend(); aLsIt++) {
        if (*aLsIt == theListener) {
          aListeners.remove(theListener);
          aLMap[aLIt->first] = aListeners;
          myListeners[anIt->first] = aLMap;
          break;
        }
      }
      if (aListeners.empty()) {
        aLMap.erase(aLIt->first);
        myListeners[anIt->first] = aLMap;
        if (aLMap.empty())
          break; // avoid incrementation of the iterator if the container is empty
      }
      aLIt++;
    }
    if (anIt->second.empty()) {
      myListeners.erase(anIt->first);
      if (myListeners.empty())
        break; // avoid incrementation of the iterator if the container is empty
    }
    anIt++;
  }

  // remove the listener in myImmediateListeners map
  std::map<char*, Events_Listener*>::const_reverse_iterator anImIt = myImmediateListeners.rbegin();
  while(anImIt != myImmediateListeners.rend()) {
    if (anImIt->second == theListener) {
      myImmediateListeners.erase(anImIt->first);
      if (myImmediateListeners.empty())
        break; // avoid incrementation of the iterator if the container is empty
    }
    anImIt++;
  }
}

void Events_Loop::flush(const Events_ID& theID)
{
  if (!myFlushActive)
    return;
  bool hasEventsToFlush = !myGroups.empty();
  std::map<char*, std::shared_ptr<Events_Message> >::iterator aMyGroup;
  for(aMyGroup = myGroups.find(theID.eventText());
    aMyGroup != myGroups.end(); aMyGroup = myGroups.find(theID.eventText()))
  {  // really sends
    bool aWasFlushed = myFlushed.find(theID.myID) != myFlushed.end();
    if (!aWasFlushed)
      myFlushed.insert(theID.myID);
    std::shared_ptr<Events_Message> aGroup = aMyGroup->second;
    myGroups.erase(aMyGroup);
    send(aGroup, false);

    if (!aWasFlushed) {
      // TODO: Stabilization fix. Check later.
      if(myFlushed.find(theID.myID) != myFlushed.end()) {
        myFlushed.erase(myFlushed.find(theID.myID));
      }
#ifdef _DEBUG
      else {
        bool aProblem = true;
      }
#endif
    }
    // send accumulated messages to "groupListeners"
    std::map<char*, std::map<void*, std::list<Events_Listener*> > >::iterator aFindID =
      myListeners.find(theID.eventText());
    if (aFindID != myListeners.end()) {
      std::map<void*, std::list<Events_Listener*> >::iterator aFindSender =
        aFindID->second.begin();
      for(; aFindSender != aFindID->second.end(); aFindSender++) {
        std::list<Events_Listener*>::iterator aListener = aFindSender->second.begin();
        for(; aListener != aFindSender->second.end(); aListener++) {
          if ((*aListener)->groupMessages()) {
            (*aListener)->flushGrouped(theID);
          }
        }
      }
    }
  }
  if (hasEventsToFlush && myGroups.empty() && myFlushed.empty()) {
    // no more messages left in the queue, so, finalize the sketch processing
    static Events_ID anID = Events_Loop::eventByName("SketchPrepared");
    std::shared_ptr<Events_Message> aMsg(new Events_Message(anID, this));
    Events_Loop::loop()->send(aMsg, false);
  }
}

void Events_Loop::eraseMessages(const Events_ID& theID)
{
  std::map<char*, std::shared_ptr<Events_Message> >::iterator aMyGroup =
    myGroups.find(theID.eventText());
  if (aMyGroup != myGroups.end()) {
    myGroups.erase(aMyGroup);
  }
}


bool Events_Loop::activateFlushes(const bool theActivate)
{
  bool isActive = myFlushActive;
  myFlushActive = theActivate;
  return isActive;
}

void Events_Loop::clear(const Events_ID& theID)
{
  std::map<char*, std::shared_ptr<Events_Message>>::iterator aMyGroup =
    myGroups.find(theID.eventText());
  if (aMyGroup != myGroups.end()) {  // really sends
    myGroups.erase(aMyGroup);
  }
}

bool Events_Loop::isFlushed(const Events_ID& theID)
{
  return myFlushed.find(theID.myID) != myFlushed.end();
}

void Events_Loop::setFlushed(const Events_ID& theID, const bool theValue)
{
  if (theValue)
    myFlushed.insert(theID.myID);
  else
    myFlushed.erase(myFlushed.find(theID.myID));
}

bool Events_Loop::hasGrouppedEvent(const Events_ID& theID)
{
  return myGroups.find(theID.myID) != myGroups.end();
}