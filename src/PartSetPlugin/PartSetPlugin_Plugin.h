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

#ifndef PartSetPlugin_Plugin_H_
#define PartSetPlugin_Plugin_H_

#include <PartSetPlugin.h>
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Events.h>

#include <Events_Listener.h>

#include <memory>

/**\class PartSetPlugin_Plugin
 * \ingroup Plugins
 * \brief The main class for management the part set operations as plugin.
 */
class PartSetPlugin_Plugin : public ModelAPI_Plugin,
                                                  public Events_Listener
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  PARTSETPLUGIN_EXPORT virtual FeaturePtr createFeature(std::string theFeatureID);

  /// Is needed for python wrapping by swig
  PARTSETPLUGIN_EXPORT PartSetPlugin_Plugin();

  //! Redefinition of Events_Listener method
  PARTSETPLUGIN_EXPORT virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);
  //! Performs the chenges of enabled/disabled state in the toolbar
  //! due to the current state of the application.
  PARTSETPLUGIN_EXPORT std::shared_ptr<ModelAPI_FeatureStateMessage> getFeaturesState();
};

#endif
