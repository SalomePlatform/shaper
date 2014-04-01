/*
 * PartSet_Message.cpp
 *
 *  Created on: Mar 21, 2014
 *      Author: sbh
 */

#include <PartSet_Message.h>

PartSet_Message::PartSet_Message(const Event_ID theId, const void* theParent)
    : Event_Message(theId, theParent)
{

}

PartSet_Message::~PartSet_Message()
{
}

