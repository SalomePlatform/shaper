// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
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

#include <Events_Listener.h>
#include <Events_MessageGroup.h>
#include <Events_Loop.h>

void Events_Listener::groupWhileFlush(const std::shared_ptr<Events_Message>& theMessage) {
  std::shared_ptr<Events_MessageGroup> aGroup =
    std::dynamic_pointer_cast<Events_MessageGroup>(theMessage);
  if (aGroup) {
    std::map<char*, std::shared_ptr<Events_Message> >::iterator aMyGroup = myGroups.find(
      aGroup->eventID().eventText());
    if (aMyGroup == myGroups.end()) {  // create a new group of messages for accumulation
      myGroups[aGroup->eventID().eventText()] = aGroup->newEmpty();
      aMyGroup = myGroups.find(aGroup->eventID().eventText());
    }
    std::shared_ptr<Events_MessageGroup> aStored =
      std::dynamic_pointer_cast<Events_MessageGroup>(aMyGroup->second);
    aStored->Join(aGroup);
    //std::cout<<"Add to group "<<theMessage->eventID().eventText()<<std::endl;
    return;
  }
}

void Events_Listener::flushGrouped(const Events_ID& theID) {
  std::map<char*, std::shared_ptr<Events_Message> >::iterator aMyGroup =
    myGroups.find(theID.eventText());
  if (aMyGroup != myGroups.end()) {
    std::shared_ptr<Events_Message> aMessage = aMyGroup->second;
    myGroups.erase(aMyGroup);
    processEvent(aMessage);
  }
}
