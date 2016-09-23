// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SketchPlugin_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

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
