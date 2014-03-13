// File:	Event_Loop.hxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#ifndef Event_Loop_HeaderFile
#define Event_Loop_HeaderFile

#include <Event_Message.hxx>
#include <Event_Listener.hxx>

#include <map>
#include <list>

/**\class Event_Lopp
 * \ingroup EventLoop
 * \brief Base class that manages the receiving and sending of all
 * not Qt-events in the application.
 *
 * One per application, initialized on start. Listeners must register in this loop
 * to get events, called by senders. Sending of events is very fast (just adding to container).
 * Performing of events is processed in separated thread, so, sender takes 
 * control back immideately.
 */
class Event_Loop {
  std::map<char*, std::map<void*, std::list<Event_Listener*> > >
    myListeners; ///< map from event ID to sender pointer to listeners that must be called for this

  //! The empty constructor, will be called at startup of the application, only once
  Event_Loop() {};
public:
  ///! Returns the main object of the loop, one per application.
  EVENT_EXPORT static Event_Loop* Loop();
  //! Returns the unique event by the given name. Call this method only on initialization of object
  //! to speedup the events processing without parsing of the string.
  EVENT_EXPORT static Event_ID EventByName(const char* theName);

  //! Allows to send an event
  EVENT_EXPORT void Send(Event_Message theMessage);

  //! Registers (or adds if such listener is already registered) a listener 
  //! that will be called on the event and from the defined sender
  EVENT_EXPORT void RegisterListener(Event_Listener* theListener, const Event_ID theID, 
    void* theSender = 0);
};

#endif
