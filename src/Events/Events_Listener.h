// File:	Events_Listener.hxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#ifndef Events_Listener_H_
#define Events_Listener_H_

#include <Events.h>
class Events_Message;

/**\class Events_Listener
 * \ingroup EventsLoop
 * \brief Base interface for any event listener.
 *
 * If some object wants to listen some events it must inherit
 * this class and register in the Loop.
 */
class EVENTS_EXPORT Events_Listener {

public:
  //! This method is called by loop when the event is started to process.
  virtual void processEvent(const Events_Message* theMessage) = 0;
};

#endif
