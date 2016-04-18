// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildPlugin_Plugin.cpp
// Created:     18 April 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Plugin.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <BuildPlugin_Wire.h>
#include <BuildPlugin_Validators.h>

// the only created instance of this plugin
static BuildPlugin_Plugin* MY_INSTANCE = new BuildPlugin_Plugin();

//=================================================================================================
BuildPlugin_Plugin::BuildPlugin_Plugin()
{
  // Register validators.
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("BuildPlugin_ValidatorBaseForWire",
                              new BuildPlugin_ValidatorBaseForWire());

  // Register this plugin.
  ModelAPI_Session::get()->registerPlugin(this);
}

//=================================================================================================
FeaturePtr BuildPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == BuildPlugin_Wire::ID()) {
    return FeaturePtr(new BuildPlugin_Wire());
  }

  // Feature of such kind is not found.
  return FeaturePtr();
}
