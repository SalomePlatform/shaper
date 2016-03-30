// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_Plugin.hxx
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#ifndef SAMPLEPANELPLUGIN_PLUGIN_H_
#define SAMPLEPANELPLUGIN_PLUGIN_H_

#include <SamplePanelPlugin.h>
#include <ModelAPI_Plugin.h>
//#include <ModuleBase_IWidgetCreator.h>

#include <ModelAPI_Feature.h>
#include <ModelAPI_Events.h>

class ModuleBase_ModelWidget;
class QWidget;

/**\class SamplePanelPlugin_Plugin
 * \ingroup Plugins
 * \brief Interface common for any plugin: allows to use plugin by the plugins manager.
 */
//, public ModuleBase_IWidgetCreator
class SamplePanelPlugin_Plugin : public ModelAPI_Plugin
{
public:
  /// Creates the feature object of this plugin by the feature string ID
  virtual FeaturePtr createFeature(std::string theFeatureID);

 public:
   SamplePanelPlugin_Plugin();
};

#endif
