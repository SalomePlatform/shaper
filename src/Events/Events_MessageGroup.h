// File:	Events_MessageGroup.hxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#ifndef Events_MessageGroup_H_
#define Events_MessageGroup_H_

#include <Events_Message.h>

/**\class Events_Message
 * \ingroup EventsLoop
 * \brief Message that allows to group messages and send them later as a group of messages.
 *
 * Loop detects such messages and accumulates them without sending. On "flush" loop sends it
 * as a group-message.
 */
class EVENTS_EXPORT Events_MessageGroup : public Events_Message {

public:

  //! Creates the message
  Events_MessageGroup(const Events_ID theID, const void* theSender = 0)
    : Events_Message(theID, theSender) {}
  //! do nothing in the destructor yet
  virtual ~Events_MessageGroup() {}

  //! Creates a new empty group (to store it in the loop before flush)
  virtual Events_MessageGroup* newEmpty() = 0;
  //! Allows to join the given message with the current one
  virtual void Join(Events_MessageGroup& theJoined) = 0;
};

#endif
