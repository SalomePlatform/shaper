// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 *
 */

#include <ExchangePlugin_Plugin.h>
#include <ExchangePlugin_ImportFeature.h>
#include <ExchangePlugin_Validators.h>

#include <Config_PropManager.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <memory>

using namespace std;

// the only created instance of this plugin
static ExchangePlugin_Plugin* MY_EXCHANGE_INSTANCE = new ExchangePlugin_Plugin();

ExchangePlugin_Plugin::ExchangePlugin_Plugin()
{
  // register this plugin
  SessionPtr aSession = ModelAPI_Session::get();
  aSession->registerPlugin(this);
  ModelAPI_ValidatorsFactory* aFactory = aSession->validators();
  aFactory->registerValidator("ExchangePlugin_ImportFormat",
                              new ExchangePlugin_ImportFormatValidator);

  // register construction properties
  Config_PropManager::registerProp("Visualization", "import_feature_color", "Imported feature color",
                                   Config_Prop::Color, IMPORTED_FEATURE_COLOR);

  // register random result color properties
  Config_PropManager::registerProp("Visualization", "random_result_color", "Use random color for results",
                                   Config_Prop::Bool, "false");
}

FeaturePtr ExchangePlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == ExchangePlugin_ImportFeature::ID()) {
    return FeaturePtr(new ExchangePlugin_ImportFeature);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
