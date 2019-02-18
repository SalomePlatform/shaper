// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#include "Config_Prop.h"
#include "Events_Loop.h"
#include "Events_Message.h"

void Config_Prop::setValue(const std::string& theValue)
{
  if (theValue != myValue) {
    myValue = theValue;
    static const Events_ID aChangedEvent = Events_Loop::loop()->eventByName("PreferenceChanged");
    Events_Loop::loop()->send(std::shared_ptr<Events_Message>(
      new Events_Message(aChangedEvent, this)));
  }
}

void Config_Prop::setDefaultValue(const std::string& theValue)
{
  if (theValue != myDefaultValue) {
    myDefaultValue = theValue;
  }
}
