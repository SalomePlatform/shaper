// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * Events_Error.cpp
 *
 *  Created on: Apr 28, 2014
 *      Author: sbh
 */

#include <Events_Error.h>
#include <Events_Loop.h>

Events_Error::Events_Error(const std::string& theDescription, const void* theSender)
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

const char* Events_Error::description() const
{
  return myDescription.c_str();
}

void Events_Error::send(const std::string& theDescription, const void* theSender)
{
  std::shared_ptr<Events_Message> aNewOne = 
    std::shared_ptr<Events_Message>(new Events_Error(theDescription, theSender));
  Events_Loop::loop()->send(aNewOne);
}
