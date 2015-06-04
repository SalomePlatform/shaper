// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#include <FeaturesPlugin_Plugin.h>

#include <FeaturesPlugin_Boolean.h>
#include <FeaturesPlugin_Extrusion.h>
#include <FeaturesPlugin_ExtrusionCut.h>
#include <FeaturesPlugin_Group.h>
#include <FeaturesPlugin_Placement.h>
#include <FeaturesPlugin_Revolution.h>
#include <FeaturesPlugin_Rotation.h>

#include <ModelAPI_Session.h>

#include <string>

#include <memory>

using namespace std;

// the only created instance of this plugin
static FeaturesPlugin_Plugin* MY_FEATURES_INSTANCE = new FeaturesPlugin_Plugin();

FeaturesPlugin_Plugin::FeaturesPlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr FeaturesPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == FeaturesPlugin_Extrusion::ID()) {
    return FeaturePtr(new FeaturesPlugin_Extrusion);
  } else if (theFeatureID == FeaturesPlugin_Revolution::ID()) {
   return FeaturePtr(new FeaturesPlugin_Revolution);
  } else if (theFeatureID == FeaturesPlugin_Rotation::ID()) {
    return FeaturePtr(new FeaturesPlugin_Rotation);
  } else if (theFeatureID == FeaturesPlugin_Boolean::ID()) {
    return FeaturePtr(new FeaturesPlugin_Boolean);
  } else if (theFeatureID == FeaturesPlugin_Group::ID()) {
    return FeaturePtr(new FeaturesPlugin_Group);
  } else if (theFeatureID == FeaturesPlugin_Placement::ID()) {
    return FeaturePtr(new FeaturesPlugin_Placement);
  } else if (theFeatureID == FeaturesPlugin_ExtrusionCut::ID()) {
    return FeaturePtr(new FeaturesPlugin_ExtrusionCut);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
