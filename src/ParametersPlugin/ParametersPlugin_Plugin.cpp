// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <ParametersPlugin_Plugin.h>
#include <ParametersPlugin_Parameter.h>

#include <ModelAPI_Session.h>

#include <memory>

// the only created instance of this plugin
static ParametersPlugin_Plugin* MY_PARAMETERSPLUGIN_INSTANCE = new ParametersPlugin_Plugin();

ParametersPlugin_Plugin::ParametersPlugin_Plugin()
{
  // register this plugin
  SessionPtr aSession = ModelAPI_Session::get();
  aSession->registerPlugin(this);
}

FeaturePtr ParametersPlugin_Plugin::createFeature(std::string theFeatureID)
{
  // TODO: register some features
  if (theFeatureID == ParametersPlugin_Parameter::ID()) {
    return FeaturePtr(new ParametersPlugin_Parameter);
  }
  return FeaturePtr();
}

