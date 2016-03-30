// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_Feature.h
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef SAMPLEPANELPLUGIN_FEATURE_H_
#define SAMPLEPANELPLUGIN_FEATURE_H_

#include "SamplePanelPlugin.h"

#include <ModelAPI_Feature.h>

/**\class SamplePanelPlugin_Feature
 * \ingroup Plugins
 * \brief Sample feature to be filled by the panel.
 */
class SamplePanelPlugin_Feature : public ModelAPI_Feature
{
 public:
  /// SketchShape feature kind
  inline static const std::string& ID()
  {
    static const std::string MY_SAMPLE_PANEL_FEATURE_ID("QtPanelFeature");
    return MY_SAMPLE_PANEL_FEATURE_ID;
  }

  /// Request for initialization of data model of the object: adding all attributes
  virtual void initAttributes() {};

  /// Returns the unique kind of a feature
  virtual const std::string& getKind() {
    static std::string MY_KIND = SamplePanelPlugin_Feature::ID();
    return MY_KIND;
  };

  /// Computes or recomputes the results
  virtual void execute() {}

  /// Use plugin manager for features creation
  SamplePanelPlugin_Feature();
};

typedef std::shared_ptr<SamplePanelPlugin_Feature> SamplePanelFeaturePtr;

#endif
