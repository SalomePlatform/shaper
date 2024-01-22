// Copyright (C) 2014-2024  CEA, EDF
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

#ifndef Events_Loop_H_
#define Events_Loop_H_

#include <Events_Message.h>
#include <Events_Listener.h>

#include <map>
#include <set>
#include <list>

class Events_MessageGroup;

/**\class Events_Loop
 * \ingroup EventsLoop
 * \brief Base class that manages the receiving and sending of all
 * not Qt-events in the application.
 *
 * One per application, initialized on start. Listeners must register in this loop
 * to get events, called by senders. Sending of events is very fast (just adding to container).
 * Performing of events is processed in separated thread, so, sender takes 
 * control back immideately.
 */
class Events_Loop
{
  /// map from event ID to sender pointer to listeners that must be called for this
  std::map<char*, std::map<void*, std::list<Events_Listener*> > > myListeners;

  /// map from event ID to listeners which must process message without waiting for flush
  std::map<char*, Events_Listener*> myImmediateListeners;

  /// map from event ID to groupped messages (accumulated for flush)
  std::map<char*, std::shared_ptr<Events_Message> > myGroups;

  ///< set of messages that are flushed right now, so they are not grouped
  std::set<char*> myFlushed;

  /// to process flushes or not
  bool myFlushActive;

  //! The empty constructor, will be called at startup of the application, only once
  Events_Loop() : myFlushActive(true) {}

 public:
  ///! Returns the main object of the loop, one per application.
  EVENTS_EXPORT static Events_Loop* loop();
  //! Returns the unique event by the given name. Call this method only on initialization of object
  //! to speedup the events processing without parsing of the string.
  EVENTS_EXPORT static Events_ID eventByName(const char* theName);

  //! Allows to send an event
  //! \param theMessage the enevt message to send
  //! \param isGroup is true for grouping messages if possible
  EVENTS_EXPORT void send(const std::shared_ptr<Events_Message>& theMessage, bool isGroup = true);

  //! Registers (or adds if such listener is already registered) a listener
  //! that will be called on the event and from the defined sender
  //! \param theListener the object that will listen (process) the event
  //! \param theID listen for messages with this ID
  //! \param theSender listen only for this sender (NULL - listen everybody)
  //! \param theImmediate for listeners who can not wait (no groupping mechanism is used for it)
  EVENTS_EXPORT void registerListener(Events_Listener* theListener, const Events_ID theID,
    void* theSender = 0, bool theImmediate = false);

  //! Remove the listener from internal maps if it was registered there
  //! \param theListener a listener
  EVENTS_EXPORT void removeListener(Events_Listener* theListener);

  //! Initializes sending of a group-message by the given ID
  EVENTS_EXPORT void flush(const Events_ID& theID);

  //! Removes messages with the given ID: they are not needed anymore (UPDATE on close)
  EVENTS_EXPORT void eraseMessages(const Events_ID& theID);

  //! Allows to disable flushes: needed in synchronization of document mechanism
  //! (to synchronize all and only then flush create, update, etc in correct order)
  //! \param theActivate a state about flushe is active. If false, the flush is disabled
  //! \return the previous active flush state
  EVENTS_EXPORT bool activateFlushes(const bool theActivate);

  //! Clears all collected messages
  EVENTS_EXPORT void clear(const Events_ID& theID);

  //! Returns true if the evement is flushed right now
  EVENTS_EXPORT bool isFlushed(const Events_ID& theID);
  //! Sets the flag that the event is flished right now
  EVENTS_EXPORT void setFlushed(const Events_ID& theID, const bool theValue);

  //! Returns true if a loop accumulated events to be flashed
  EVENTS_EXPORT bool hasGrouppedEvent(const Events_ID& theID);

private:
  //! Calls "processEvent" for the given listeners.
  //! If theFlushedNow for grouped listeners is stores message in listeners.
  void sendProcessEvent(const std::shared_ptr<Events_Message>& theMessage,
    std::list<Events_Listener*>& theListeners, const bool theFlushedNow);

};

#endif
