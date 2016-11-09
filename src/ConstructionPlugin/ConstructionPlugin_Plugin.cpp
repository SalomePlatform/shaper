// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "ConstructionPlugin_Plugin.h"
#include "ConstructionPlugin_Point.h"
#include "ConstructionPlugin_Axis.h"
#include "ConstructionPlugin_Plane.h"
#include "ConstructionPlugin_Validators.h"

#include <Config_PropManager.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>

using namespace std;

// the only created instance of this plugin
static ConstructionPlugin_Plugin* MY_CONSTRUCTION_INSTANCE = new ConstructionPlugin_Plugin();

ConstructionPlugin_Plugin::ConstructionPlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("ConstructionPlugin_ValidatorPointLines",
                              new ConstructionPlugin_ValidatorPointLines());
  aFactory->registerValidator("ConstructionPlugin_ValidatorPointLineAndPlaneNotParallel",
                              new ConstructionPlugin_ValidatorPointLineAndPlaneNotParallel());
  aFactory->registerValidator("ConstructionPlugin_ValidatorPlaneThreePoints",
                              new ConstructionPlugin_ValidatorPlaneThreePoints());
  aFactory->registerValidator("ConstructionPlugin_ValidatorPlaneLinePoint",
                              new ConstructionPlugin_ValidatorPlaneLinePoint());
  aFactory->registerValidator("ConstructionPlugin_ValidatorPlaneTwoParallelPlanes",
                              new ConstructionPlugin_ValidatorPlaneTwoParallelPlanes());
  aFactory->registerValidator("ConstructionPlugin_ValidatorAxisTwoNotParallelPlanes",
                              new ConstructionPlugin_ValidatorAxisTwoNotParallelPlanes());

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);

  // register construction properties
  Config_PropManager::registerProp("Visualization", "construction_plane_color",
                                   "Construction plane color",
                                   Config_Prop::Color, ConstructionPlugin_Plane::DEFAULT_COLOR());
}

FeaturePtr ConstructionPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == ConstructionPlugin_Point::ID()) {
    return FeaturePtr(new ConstructionPlugin_Point);
  }
  else if (theFeatureID == ConstructionPlugin_Axis::ID()) {
    return FeaturePtr(new ConstructionPlugin_Axis);
  }
  else if (theFeatureID == ConstructionPlugin_Plane::ID()) {
    return FeaturePtr(new ConstructionPlugin_Plane);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
