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

#ifndef CONFIG_PLUGINMESSAGE_H
#define CONFIG_PLUGINMESSAGE_H

#include <Config_def.h>
#include <Events_Message.h>

#include <string>

/*!
 * \class Config_PluginMessage
 * \ingroup Config
 * \brief Information that plugin is started to load.
 *
 * Contains the plugin name. Event is posted just before load of the plugin
 * before registerPlugin call.
 */
class Config_PluginMessage : public Events_Message
{
  std::string myPluginId;  ///< Plugin unique id
  std::string myUses;  ///< Which plugins are used by this one Id

 public:
  /// Event ID that plugin will be loaded
  inline static const char* EVENT_ID()
  {
    static const char * MY_EVENT_ID("BeforePluginLoad");
    return MY_EVENT_ID;
  }

  /// Constructs Config_PluginMessage
  CONFIG_EXPORT Config_PluginMessage(const Events_ID theId,
    const std::string& thePluginId, const void* theParent = 0);
  /// Deletes Config_PluginMessage
  CONFIG_EXPORT virtual ~Config_PluginMessage();

  /// Plugin Id
  CONFIG_EXPORT const std::string& pluginId() const;
  /// Which plugins are used by this one Id
  CONFIG_EXPORT const std::string& uses() const;
  /// Sets which plugins are used by this one Id
  CONFIG_EXPORT void setUses(const std::string& theUses);
};

#endif // CONFIG_MESSAGE_H
