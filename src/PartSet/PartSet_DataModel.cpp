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

#include "PartSet_DataModel.h"
#include "PartSet_TreeNodes.h"

#include <ModelAPI_Events.h>

#include <Events_Loop.h>

PartSet_DataModel::PartSet_DataModel()
{
//  Events_Loop* aLoop = Events_Loop::loop();
//  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
//  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
//  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
//  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_ORDER_UPDATED));
//  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_DOCUMENT_CHANGED));
}


void PartSet_DataModel::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{

}
