// File:        SketchPlugin_Plugin.hxx
// Created:     31 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef SketchPlugin_Plugin_H_
#define SketchPlugin_Plugin_H_

#include "SketchPlugin.h"
#include "ModelAPI_Plugin.h"
#include "ModelAPI_Feature.h"

class SKETCHPLUGIN_EXPORT SketchPlugin_Plugin : public ModelAPI_Plugin
{
 public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
  /// Is needed for python wrapping by swig
  SketchPlugin_Plugin();
};

#endif
