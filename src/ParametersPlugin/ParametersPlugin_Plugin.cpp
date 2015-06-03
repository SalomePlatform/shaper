// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <pyconfig.h>

#include <ParametersPlugin_Plugin.h>
#include <ParametersPlugin_Parameter.h>
#include <ParametersPlugin_Validators.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <memory>

// the only created instance of this plugin
static ParametersPlugin_Plugin* MY_PARAMETERSPLUGIN_INSTANCE = new ParametersPlugin_Plugin();

ParametersPlugin_Plugin::ParametersPlugin_Plugin()
{
  // register this plugin
  SessionPtr aSession = ModelAPI_Session::get();
  aSession->registerPlugin(this);

  ModelAPI_ValidatorsFactory* aFactory = aSession->validators();
  aFactory->registerValidator("Parameters_VariableValidator",
                              new ParametersPlugin_VariableValidator);
  aFactory->registerValidator("Parameters_ExpressionValidator",
                              new ParametersPlugin_ExpressionValidator);

  myEvalListener = std::shared_ptr<ParametersPlugin_EvalListener>(new ParametersPlugin_EvalListener());
}

FeaturePtr ParametersPlugin_Plugin::createFeature(std::string theFeatureID)
{
  // TODO: register some features
  if (theFeatureID == ParametersPlugin_Parameter::ID()) {
    return FeaturePtr(new ParametersPlugin_Parameter);
  }
  return FeaturePtr();
}

