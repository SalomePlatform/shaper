// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_Plugin.cpp
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <SamplePanelPlugin_Plugin.h>
#include <SamplePanelPlugin_Feature.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_WidgetCreatorFactory.h>
#include <SamplePanelPlugin_WidgetCreator.h>

// the only created instance of this plugin
static SamplePanelPlugin_Plugin* MY_SAMPLE_PANEL_PLUGIN_INSTANCE = new SamplePanelPlugin_Plugin();

SamplePanelPlugin_Plugin::SamplePanelPlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();

  WidgetCreatorFactoryPtr aWidgetCreatorFactory = ModuleBase_WidgetCreatorFactory::get();
  aWidgetCreatorFactory->registerCreator(
          std::shared_ptr<SamplePanelPlugin_WidgetCreator>(new SamplePanelPlugin_WidgetCreator()));

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr SamplePanelPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == SamplePanelPlugin_Feature::ID()) {
    return FeaturePtr(new SamplePanelPlugin_Feature);
  }
  // feature of such kind is not found
  return FeaturePtr();
}

