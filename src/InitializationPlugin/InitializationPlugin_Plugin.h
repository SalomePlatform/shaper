
// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef INITIALIZATIONPLUGIN_PLUGIN_H_
#define INITIALIZATIONPLUGIN_PLUGIN_H_

#include <InitializationPlugin.h>
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**\class InitializationPlugin_Plugin
 * TODO: Add documentation
 */
class INITIALIZATIONPLUGIN_EXPORT InitializationPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  InitializationPlugin_Plugin();
};

#endif

