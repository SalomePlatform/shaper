// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:    ExchangePlugin_Plugin.h
// Created: Aug 28, 2014
// Author:  Sergey BELASH

#ifndef EXCHANGEPLUGIN_PLUGIN_H_
#define EXCHANGEPLUGIN_PLUGIN_H_

#include <ExchangePlugin.h>
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**
 * \class ExchangePlugin_Plugin
 * \ingroup Plugins
 * \brief The main class for management the import and export operations as plugin.
 */
class EXCHANGEPLUGIN_EXPORT ExchangePlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  ExchangePlugin_Plugin();
};

#endif
