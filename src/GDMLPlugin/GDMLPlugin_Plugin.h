// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GDMLPlugin_Plugin.h
// Created:     23 Nov 2016
// Author:      Clarisse Genrault (CEA)

#ifndef GDMLPLUGIN_PLUGIN_H_
#define GDMLPLUGIN_PLUGIN_H_

#include "GDMLPlugin.h"
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**\class GDMLPlugin_Plugin
 * \ingroup Plugins
 * \brief The main class managing GDML Features as plugins.
 */
class GDMLPLUGIN_EXPORT GDMLPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  /// Default constructor
  GDMLPlugin_Plugin();
};

#endif // GDMLPLUGIN_PLUGIN_H_
