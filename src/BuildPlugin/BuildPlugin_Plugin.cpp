// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        BuildPlugin_Plugin.cpp
// Created:     18 April 2016
// Author:      Dmitry Bobylev

#include "BuildPlugin_Plugin.h"

#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <BuildPlugin_Vertex.h>
#include <BuildPlugin_Edge.h>
#include <BuildPlugin_Wire.h>
#include <BuildPlugin_Face.h>
#include <BuildPlugin_Validators.h>

// the only created instance of this plugin
static BuildPlugin_Plugin* MY_INSTANCE = new BuildPlugin_Plugin();

//=================================================================================================
BuildPlugin_Plugin::BuildPlugin_Plugin()
{
  // Register validators.
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("BuildPlugin_ValidatorBaseForBuild",
                              new BuildPlugin_ValidatorBaseForBuild());
  aFactory->registerValidator("BuildPlugin_ValidatorBaseForWire",
                              new BuildPlugin_ValidatorBaseForWire());
  aFactory->registerValidator("BuildPlugin_ValidatorBaseForFace",
                              new BuildPlugin_ValidatorBaseForFace());

  // Register this plugin.
  ModelAPI_Session::get()->registerPlugin(this);
}

//=================================================================================================
FeaturePtr BuildPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if(theFeatureID == BuildPlugin_Vertex::ID()) {
    return FeaturePtr(new BuildPlugin_Vertex());
  } else if(theFeatureID == BuildPlugin_Edge::ID()) {
    return FeaturePtr(new BuildPlugin_Edge());
  } else if(theFeatureID == BuildPlugin_Wire::ID()) {
    return FeaturePtr(new BuildPlugin_Wire());
  } else if(theFeatureID == BuildPlugin_Face::ID()) {
    return FeaturePtr(new BuildPlugin_Face());
  }

  // Feature of such kind is not found.
  return FeaturePtr();
}
