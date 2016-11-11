// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        CollectionPlugin_Plugin.hxx
// Created:     07 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef CollectionPlugin_Plugin_H_
#define CollectionPlugin_Plugin_H_

#include "CollectionPlugin.h"
#include <ModelAPI_Plugin.h>
#include <ModelAPI_Feature.h>

/**\class CollectionPlugin_Plugin
 * \ingroup Plugins
 * \brief The main class for management the 3D features as plugin.
 */
class COLLECTIONPLUGIN_EXPORT CollectionPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  /// Default constructor
  CollectionPlugin_Plugin();
};

#endif
