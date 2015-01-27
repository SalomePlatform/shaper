// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        FeaturesPlugin_Plugin.hxx
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef FeaturesPlugin_Plugin_H_
#define FeaturesPlugin_Plugin_H_

#include "FeaturesPlugin.h"
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**\class FeaturesPlugin_Plugin
 * \ingroup Plugins
 * \brief The main class for management the 3D features as plugin.
 */
class FEATURESPLUGIN_EXPORT FeaturesPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  /// Default constructor
  FeaturesPlugin_Plugin();
};

#endif
