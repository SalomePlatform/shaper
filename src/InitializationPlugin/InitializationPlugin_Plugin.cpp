
// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <InitializationPlugin_Plugin.h>
#include <InitializationPlugin_OriginPlanesFeature.h>

#include <ModelAPI_Session.h>

#include <memory>

// the only created instance of this plugin
static InitializationPlugin_Plugin* MY_INITIALIZATIONPLUGIN_INSTANCE = new InitializationPlugin_Plugin();

InitializationPlugin_Plugin::InitializationPlugin_Plugin()
{
  // register this plugin
  SessionPtr aSession = ModelAPI_Session::get();
  aSession->registerPlugin(this);
}

FeaturePtr InitializationPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if(InitializationPlugin_OriginPlanesFeature::ID() == theFeatureID) {
    return FeaturePtr(new InitializationPlugin_OriginPlanesFeature);
  }
  return FeaturePtr();
}

