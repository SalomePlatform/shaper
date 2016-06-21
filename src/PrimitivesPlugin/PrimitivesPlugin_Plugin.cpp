// Copyright (C) 2014-2016 CEA/DEN, EDF R&D -->

// File:        PrimitivesPlugin_Plugin.hxx
// Created:     29 Mar 2016
// Author:      Clarisse Genrault (CEA)

#include <PrimitivesPlugin_Plugin.h>

#include <PrimitivesPlugin_Box.h>

#include <ModelAPI_Session.h>

#include <string>

#include <memory>

#include <iostream>

using namespace std;

// the only created instance of this plugin
static PrimitivesPlugin_Plugin* MY_PRIMITIVES_INSTANCE = new PrimitivesPlugin_Plugin();

PrimitivesPlugin_Plugin::PrimitivesPlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr PrimitivesPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == PrimitivesPlugin_Box::ID()) {
    return FeaturePtr(new PrimitivesPlugin_Box);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
