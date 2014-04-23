// File:        SketchPlugin_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Plugin_HeaderFile
#define SketchPlugin_Plugin_HeaderFile


#include "SketchPlugin.h"
#include "ModelAPI_Plugin.h"

class SKETCHPLUGIN_EXPORT SketchPlugin_Plugin: public ModelAPI_Plugin
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual std::shared_ptr<ModelAPI_Feature> createFeature(std::string theFeatureID);

public:
  /// Is needed for python wrapping by swig
  SketchPlugin_Plugin();
};

#endif
