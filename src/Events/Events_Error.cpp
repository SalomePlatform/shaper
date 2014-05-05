/*
 * Events_Error.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: sbh
 */

#include <Events_Error.h>
#include <Events_Loop.h>

Events_Error::Events_Error(char* theDescription, const void* theSender)
 : Events_Message(Events_Error::errorID(), theSender)
{
  myDescription = theDescription;
}

Events_Error::~Events_Error()
{
}

Events_ID Events_Error::errorID()
{
  Events_Loop* aLoop = Events_Loop::loop();
  return aLoop->eventByName("ApplicationError");
}

char* Events_Error::description() const
{
  return myDescription;
}

void Events_Error::send(char* theDescription, const void* theSender)
{
  Events_Error anError(theDescription, theSender);
  Events_Loop::loop()->send(anError);
}

void Events_Error::send(std::string theDescription, const void* theSender)
{
  Events_Error::send(theDescription.c_str(), theSender);
}
