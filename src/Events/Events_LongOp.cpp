// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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
    std::shared_ptr<Events_Message> aNewOne =
      std::shared_ptr<Events_Message>(new Events_LongOp(theSender));
    Events_Loop::loop()->send(aNewOne);
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
    std::shared_ptr<Events_Message> aNewOne =
      std::shared_ptr<Events_Message>(new Events_LongOp(theSender));
    Events_Loop::loop()->send(aNewOne);
  }
}

bool Events_LongOp::isPerformed()
{
  return !MY_SENDERS.empty();
}
