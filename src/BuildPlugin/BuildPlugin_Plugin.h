// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildPlugin_Plugin.h
// Created:     18 April 2016
// Author:      Dmitry Bobylev

#ifndef BuildPlugin_Plugin_H_
#define BuildPlugin_Plugin_H_

#include "BuildPlugin.h"

#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/// \class BuildPlugin_Plugin
/// \ingroup Plugins
/// \brief The main class for management the build features as plugin.
class BUILDPLUGIN_EXPORT BuildPlugin_Plugin: public ModelAPI_Plugin
{
public:
  /// Default constructor
  BuildPlugin_Plugin();

  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);
};

#endif
