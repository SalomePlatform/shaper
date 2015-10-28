// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        GeomValidators_Plugin.h
// Created:     28 Oct 2015
// Author:      Sergey POKHODENKO

#ifndef GEOMVALIDATORS_PLUGIN_H_
#define GEOMVALIDATORS_PLUGIN_H_

#include <GeomValidators.h>
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**\class GeomValidators_Plugin
 * \ingroup Plugins
 * \brief Interface common for any plugin: allows to use plugin by the plugins manager.
 */
class GEOMVALIDATORS_EXPORT GeomValidators_Plugin : public ModelAPI_Plugin
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

public:
  GeomValidators_Plugin();
};

#endif
