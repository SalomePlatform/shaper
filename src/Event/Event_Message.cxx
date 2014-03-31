// File:	Event_Message.cxx
// Created:	Thu Mar 13 2014
// Author:	Mikhail PONIKAROV

#include <Event_Message.hxx>

Event_Message::Event_Message(const Event_ID theID, const void* theSender)
    : myEventId(theID), mySender((void*) theSender)
{
}
