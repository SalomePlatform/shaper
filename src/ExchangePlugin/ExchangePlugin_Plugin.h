// File:        ExchangePlugin_Plugin.hxx
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef EXCHANGEPLUGIN_PLUGIN_H_
#define EXCHANGEPLUGIN_PLUGIN_H_

#include <ExchangePlugin.h>
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

class EXCHANGEPLUGIN_EXPORT ExchangePlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  ExchangePlugin_Plugin();
};

#endif
