// File:        PartSetPlugin_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Plugin_H_
#define PartSetPlugin_Plugin_H_

#include "PartSetPlugin.h"
#include "ModelAPI_Plugin.h"
#include "ModelAPI_Feature.h"

class PARTSETPLUGIN_EXPORT PartSetPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  PartSetPlugin_Plugin();
};

#endif
