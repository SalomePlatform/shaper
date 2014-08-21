/*
 * Events_Error.h
 *
 *  Created on: Apr 28, 2014
 *      Author: sbh
 */

#ifndef EVENTS_ERROR_H_
#define EVENTS_ERROR_H_

#include <Events.h>
#include <Events_Message.h>

#include <string>

class EVENTS_EXPORT Events_Error : public Events_Message
{
  char* myDescription;  ///< pointer to the description of the error

 public:
  virtual ~Events_Error();

  static Events_ID errorID();
  char* description() const;
  static void send(char* theDescription, const void* theSender = 0);
  static void send(std::string theDescription, const void* theSender = 0);

 protected:
  Events_Error(char* theDescription, const void* theSender = 0);
};

#endif /* EVENTS_ERROR_H_ */
