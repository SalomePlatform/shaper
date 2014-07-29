// File:        Events_LongOp.h
// Created:     29 Jul 2014
// Author:      Mikhail PONIKAROV

#ifndef EVENTS_LONGOP_H_
#define EVENTS_LONGOP_H_

#include <Events.h>
#include <Events_Message.h>

/**
 * Informs the application that the long operation is performed.
 * Causes waiting coursor in GUI.
 */
class EVENTS_EXPORT Events_LongOp: public Events_Message
{
public:
  virtual ~Events_LongOp();
  /// Returns the identifier of this event
  static Events_ID errorID();
  /// Starts the long operation
  static void start(void* theSender = 0);
  /// Stops the long operation
  static void end(void* theSender = 0);
  /// Returns true if the long operation is performed
  static bool isPerformed(); 

protected:
  Events_LongOp(void* theSender = 0);
};

#endif /* EVENTS_ERROR_H_ */
