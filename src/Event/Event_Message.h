// File:	Event_Message.hxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#ifndef Event_Message_HeaderFile
#define Event_Message_HeaderFile

#include <Event.h>

/**\class Event_ID
 * \ingroup EventLoop
 * \brief Identifier of the event kind.
 *
 * Each event ID is created in main Envent_Loop class
 * that stores correspondance between the string-name of the
 * identifier and the pointer to the static string that is really
 * used as an identifier (this is usefull for debugging of the events
 * with log files and in debugger).
 */
class EVENT_EXPORT Event_ID {
  char* myID; ///< pointer to the text-identifier of the event, unique pointer for all events of such type

  Event_ID(char* theID) {myID = theID;}

  friend class Event_Loop;
public:
  /// Returns the text-identifier of the event (for debugging reasons)
  char* EventText() const {return myID;}
  /// Allows to compare identifiers
  bool operator==(const Event_ID& theID) const {return myID == theID.myID;}
};

/**\class Event_Message
 * \ingroup EventLoop
 * \brief Message for communication between sender and listener of event.
 * Normally it is inherited by the higher-level 
 */
class EVENT_EXPORT Event_Message {
  Event_ID myEventId; ///< identifier of the event
  void* mySender; ///< the sender object

public:

  //! Creates the message
  Event_Message(const Event_ID theID, const void* theSender = 0);
  virtual ~Event_Message() {}

  //! Returns identifier of the message
  const Event_ID& EventID() const {return myEventId;}

  //! Returns sender of the message or NULL if it is anonymous message
  void* Sender() {return mySender;}
};

#endif
