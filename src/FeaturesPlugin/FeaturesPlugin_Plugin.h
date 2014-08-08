// File:        FeaturesPlugin_Plugin.hxx
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef FeaturesPlugin_Plugin_H_
#define FeaturesPlugin_Plugin_H_


#include "FeaturesPlugin.h"
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

class FEATURESPLUGIN_EXPORT FeaturesPlugin_Plugin: public ModelAPI_Plugin
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

public:
  /// Is needed for python wrapping by swig
  FeaturesPlugin_Plugin();
};

#endif
