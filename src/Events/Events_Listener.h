// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:	Events_Listener.hxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#ifndef Events_Listener_H_
#define Events_Listener_H_

#include <Events.h>
#include <memory>
#include <map>

class Events_Message;
class Events_ID;

/**\class Events_Listener
 * \ingroup EventsLoop
 * \brief Base interface for any event listener.
 *
 * If some object wants to listen some events it must inherit
 * this class and register in the Loop.
 */
class Events_Listener {
  /// map from event ID to groupped messages (for flush for groupMessages=true listeners)
  std::map<char*, std::shared_ptr<Events_Message> > myGroups;

 public:
  //! This method is called by loop when the event is started to process.
  EVENTS_EXPORT virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage) = 0;

  //! Listener that needs mostly grouped messages received returns true in this method.
  //! In this case during the message is flushed, all the new messages are grouped, not sended
  //! immideately and then sent in the end of flush.
  EVENTS_EXPORT virtual bool groupMessages() {return false;}

protected:
  //! Allows to group messages while they are flushed (for flush for groupMessages=true listeners)
  void groupWhileFlush(const std::shared_ptr<Events_Message>& theMessage);
  //! Sends myGroups on flush finish
  EVENTS_EXPORT void flushGrouped(const Events_ID& theID);

  friend class Events_Loop;
};

#endif
