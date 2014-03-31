// File:        PartSetPlugin_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef PartSetPlugin_Plugin_HeaderFile
#define PartSetPlugin_Plugin_HeaderFile


#include "PartSetPlugin.h"
#include "ModelAPI_Plugin.h"

class PARTSETPLUGIN_EXPORT PartSetPlugin_Plugin: public ModelAPI_Plugin
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature(std::string theFeatureID);

public:
  /// Is needed for python wrapping by swig
  PartSetPlugin_Plugin();
};

#endif
