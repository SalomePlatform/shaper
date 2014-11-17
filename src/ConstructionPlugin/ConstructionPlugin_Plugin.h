// File:        ConstructionPlugin_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ConstructionPlugin_Plugin_H_
#define ConstructionPlugin_Plugin_H_

#include "ConstructionPlugin.h"
#include "ModelAPI_Plugin.h"
#include "ModelAPI_Feature.h"

class CONSTRUCTIONPLUGIN_EXPORT ConstructionPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  ConstructionPlugin_Plugin();
};

#endif
