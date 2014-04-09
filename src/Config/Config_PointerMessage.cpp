/*
 * Config_PointerMessage.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: sbh
 */

#include <Config_PointerMessage.h>

Config_PointerMessage::Config_PointerMessage(const Event_ID theId, const void* theParent)
    : Event_Message(theId, theParent), myPointer(0)
{

}

Config_PointerMessage::~Config_PointerMessage()
{

}

void* Config_PointerMessage::pointer() const
{
  return myPointer;
}

void Config_PointerMessage::setPointer(void* pointer)
{
  myPointer = pointer;
}
