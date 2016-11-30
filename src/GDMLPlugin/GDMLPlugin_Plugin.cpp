// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        GDMLPlugin_Plugin.cpp
// Created:     23 Nov 2016
// Author:      Clarisse Genrault (CEA)

#include <ModelAPI_Session.h>

#include <GDMLPlugin_Plugin.h>

#include <GDMLPlugin_ConeSegment.h>

// the only created instance of this plugin
static GDMLPlugin_Plugin* MY_GDML_INSTANCE = new GDMLPlugin_Plugin();

GDMLPlugin_Plugin::GDMLPlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr GDMLPlugin_Plugin::createFeature(std::string theFeatureID)
{
  if (theFeatureID == GDMLPlugin_ConeSegment::ID()) {
    return FeaturePtr(new GDMLPlugin_ConeSegment);
  }
  // feature of such kind is not found
  return FeaturePtr();
}

