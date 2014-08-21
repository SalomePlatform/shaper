// File:        Events_LongOp.cpp
// Created:     29 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Events_LongOp.h>
#include <Events_Loop.h>

/// senders of the long operation collected, ends the long operation event only
/// if all senders are stopped
std::map<void*, int> MY_SENDERS;

Events_LongOp::Events_LongOp(void* theSender)
    : Events_Message(Events_LongOp::eventID(), theSender)
{
}

Events_LongOp::~Events_LongOp()
{
}

Events_ID Events_LongOp::eventID()
{
  Events_Loop* aLoop = Events_Loop::loop();
  return aLoop->eventByName("LongOperation");
}

void Events_LongOp::start(void* theSender)
{
  bool toSend = MY_SENDERS.empty();
  if (MY_SENDERS.find(theSender) == MY_SENDERS.end())
    MY_SENDERS[theSender] = 1;
  else
    MY_SENDERS[theSender]++;

  if (toSend) {
    Events_LongOp anEvent(theSender);
    Events_Loop::loop()->send(anEvent);
  }
}

void Events_LongOp::end(void* theSender)
{
  if (MY_SENDERS.find(theSender) != MY_SENDERS.end()) {
    int aCount = MY_SENDERS[theSender];
    if (aCount <= 1)
      MY_SENDERS.erase(theSender);
    else
      MY_SENDERS[theSender] = aCount - 1;
  }
  if (MY_SENDERS.empty()) {
    Events_LongOp anEvent(theSender);
    Events_Loop::loop()->send(anEvent);
  }
}

bool Events_LongOp::isPerformed()
{
  return !MY_SENDERS.empty();
}
