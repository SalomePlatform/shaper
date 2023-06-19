// Copyright (C) 2014-2023  CEA, EDF
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

#include "Config_PluginMessage.h"

Config_PluginMessage::Config_PluginMessage(const Events_ID theId,
  const std::string& thePluginId, const void* theParent)
    : Events_Message(theId, theParent)
{
  myPluginId = thePluginId;
}

Config_PluginMessage::~Config_PluginMessage()
{

}

const std::string& Config_PluginMessage::pluginId() const
{
  return myPluginId;
}

const std::string& Config_PluginMessage::uses() const
{
  return myUses;
}

void Config_PluginMessage::setUses(const std::string& theUses)
{
  myUses = theUses;
}
