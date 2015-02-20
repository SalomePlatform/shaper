// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "ConstructionPlugin_Plugin.h"
#include "ConstructionPlugin_Point.h"
#include "ConstructionPlugin_Axis.h"
#include "ConstructionPlugin_Plane.h"

#include <Config_PropManager.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>

using namespace std;

// the only created instance of this plugin
static ConstructionPlugin_Plugin* MY_CONSTRUCTION_INSTANCE = new ConstructionPlugin_Plugin();

ConstructionPlugin_Plugin::ConstructionPlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);

  // register construction properties
  Config_PropManager::registerProp("Visualization", "construction_point_color", "Construction point color",
                                   Config_Prop::Color, CONSTRUCTION_POINT_COLOR);
  Config_PropManager::registerProp("Visualization", "construction_axis_color", "Construction axis color",
                                   Config_Prop::Color, CONSTRUCTION_AXIS_COLOR);
  Config_PropManager::registerProp("Visualization", "construction_plane_color", "Construction plane color",
                                   Config_Prop::Color, CONSTRUCTION_PLANE_COLOR);
}

FeaturePtr ConstructionPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == CONSTRUCTION_POINT_KIND) {
    return FeaturePtr(new ConstructionPlugin_Point);
  }
  else if (theFeatureID == CONSTRUCTION_AXIS_KIND) {
    return FeaturePtr(new ConstructionPlugin_Axis);
  }
  else if (theFeatureID == CONSTRUCTION_PLANE_KIND) {
    return FeaturePtr(new ConstructionPlugin_Plane);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
