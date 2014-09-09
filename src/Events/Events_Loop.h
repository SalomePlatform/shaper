// File:	Events_Loop.hxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#ifndef Events_Loop_H_
#define Events_Loop_H_

#include <Events_Message.h>
#include <Events_Listener.h>

#include <map>
#include <list>

class Events_MessageGroup;

/**\class Events_Lopp
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

  /// map from event ID to groupped messages (accumulated on flush)
  std::map<char*, Events_MessageGroup*> myGroups;

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
  //! \param isGroup is true for grouping messages if possible
  EVENTS_EXPORT void send(Events_Message& theMessage, bool isGroup = true);

  //! Registers (or adds if such listener is already registered) a listener 
  //! that will be called on the event and from the defined sender
  EVENTS_EXPORT void registerListener(Events_Listener* theListener, const Events_ID theID,
                                      void* theSender = 0);

  //! Initializes sending of a group-message by the given ID
  EVENTS_EXPORT void flush(const Events_ID& theID);

  //! Allows to disable flushes: needed in synchronization of document mechanism 
  //! (to synchronize all and only then flush create, update, etc in correct order)
  EVENTS_EXPORT void activateFlushes(const bool theActivate);
};

#endif
