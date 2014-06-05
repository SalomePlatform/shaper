// File:        ConstructionPlugin_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ConstructionPlugin_Plugin_HeaderFile
#define ConstructionPlugin_Plugin_HeaderFile


#include "ConstructionPlugin.h"
#include "ModelAPI_Plugin.h"
#include "ModelAPI_Feature.h"

class CONSTRUCTIONPLUGIN_EXPORT ConstructionPlugin_Plugin: public ModelAPI_Plugin
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

public:
  /// Is needed for python wrapping by swig
  ConstructionPlugin_Plugin();
};

#endif
