// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#ifndef PARAMETERSPLUGIN_PLUGIN_H_
#define PARAMETERSPLUGIN_PLUGIN_H_

#include <ParametersPlugin.h>
#include <ParametersPlugin_EvalListener.h>

#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**\class ParametersPlugin_Plugin
 * TODO: Add documentation
 */
class ParametersPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  PARAMETERSPLUGIN_EXPORT virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  ParametersPlugin_Plugin();

  std::shared_ptr<ParametersPlugin_EvalListener> myEvalListener;
};

#endif

