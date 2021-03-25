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

#ifndef SKETCHPLUGIN_PLUGIN_H_
#define SKETCHPLUGIN_PLUGIN_H_

#include <SketchPlugin.h>
#include <Events_Listener.h>
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Events.h>

/**\class SketchPlugin_Plugin
 * \ingroup Plugins
 * \brief Interface common for any plugin: allows to use plugin by the plugins manager.
 */
class SketchPlugin_Plugin : public ModelAPI_Plugin, public Events_Listener
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  SKETCHPLUGIN_EXPORT virtual FeaturePtr createFeature(std::string theFeatureID);

  /// Constructor that registers features and other plugin elements.
  SKETCHPLUGIN_EXPORT SketchPlugin_Plugin();

  //! Redefinition of Events_Listener method
  SKETCHPLUGIN_EXPORT virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

protected:
  //! Returns the state of the feature in the WorkBench: enabled or disabled for the moment.
  std::shared_ptr<ModelAPI_FeatureStateMessage> getFeaturesState(
      const std::shared_ptr<ModelAPI_Feature>& theFeature) const;
};

#endif
