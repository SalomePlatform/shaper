#include "SketchPlugin_Plugin.h"
#include "SketchPlugin_Sketch.h"
#include "SketchPlugin_Line.h"
#include "SketchPlugin_Point.h"
#include "SketchPlugin_Circle.h"
#include "SketchPlugin_Arc.h"
#include "SketchPlugin_ConstraintCoincidence.h"
#include "SketchPlugin_ConstraintDistance.h"
#include "SketchPlugin_ConstraintLength.h"
#include "SketchPlugin_ConstraintParallel.h"
#include "SketchPlugin_ConstraintPerpendicular.h"
#include "SketchPlugin_ConstraintRadius.h"
#include "SketchPlugin_Validators.h"
#include "SketchPlugin_ResultValidators.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Validator.h>

#include <Config_PropManager.h>

using namespace std;

// the only created instance of this plugin
static SketchPlugin_Plugin* MY_INSTANCE = new SketchPlugin_Plugin();

SketchPlugin_Plugin::SketchPlugin_Plugin()
{
  SessionPtr aMgr = ModelAPI_Session::get();
  ModelAPI_ValidatorsFactory* aFactory = aMgr->validators();
  aFactory->registerValidator("SketchPlugin_DistanceAttrValidator",
                              new SketchPlugin_DistanceAttrValidator);
  aFactory->registerValidator("SketchPlugin_ResultPointValidator", new SketchPlugin_ResultPointValidator);
  aFactory->registerValidator("SketchPlugin_ResultLineValidator", new SketchPlugin_ResultLineValidator);
  aFactory->registerValidator("SketchPlugin_ResultArcValidator", new SketchPlugin_ResultArcValidator);

  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);

  // register sketcher properties
  Config_PropManager::registerProp("Sketch planes", "planes_color", "Color", Config_Prop::Color,
                                   SKETCH_PLANE_COLOR);
  Config_PropManager::registerProp("Sketch planes", "planes_size", "Size", Config_Prop::Double,
                                   PLANE_SIZE);
  Config_PropManager::registerProp("Sketch planes", "planes_thikness", "Thickness",
                                   Config_Prop::Integer, SKETCH_WIDTH);

  Config_PropManager::registerProp("Visualization", "parallel_color", "Parallel constraint color",
                                   Config_Prop::Color, PARALLEL_COLOR);
  Config_PropManager::registerProp("Visualization", "perpendicular_color",
                                   "Perpendicular constraint color", Config_Prop::Color,
                                   PERPENDICULAR_COLOR);
  Config_PropManager::registerProp("Visualization", "distance_color", "Distance color",
                                   Config_Prop::Color, DISTANCE_COLOR);
  Config_PropManager::registerProp("Visualization", "length_color", "Length color",
                                   Config_Prop::Color, LENGTH_COLOR);
  Config_PropManager::registerProp("Visualization", "radius_color", "Radius color",
                                   Config_Prop::Color, RADIUS_COLOR);
}

FeaturePtr SketchPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == SketchPlugin_Sketch::ID()) {
    return FeaturePtr(new SketchPlugin_Sketch);
  } else if (theFeatureID == SketchPlugin_Point::ID()) {
    return FeaturePtr(new SketchPlugin_Point);
  } else if (theFeatureID == SketchPlugin_Line::ID()) {
    return FeaturePtr(new SketchPlugin_Line);
  } else if (theFeatureID == SketchPlugin_Circle::ID()) {
    return FeaturePtr(new SketchPlugin_Circle);
  } else if (theFeatureID == SketchPlugin_Arc::ID()) {
    return FeaturePtr(new SketchPlugin_Arc);
  } else if (theFeatureID == SketchPlugin_ConstraintCoincidence::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintCoincidence);
  } else if (theFeatureID == SketchPlugin_ConstraintDistance::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintDistance);
  } else if (theFeatureID == SketchPlugin_ConstraintLength::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintLength);
  } else if (theFeatureID == SketchPlugin_ConstraintParallel::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintParallel);
  } else if (theFeatureID == SketchPlugin_ConstraintPerpendicular::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintPerpendicular);
  } else if (theFeatureID == SketchPlugin_ConstraintRadius::ID()) {
    return FeaturePtr(new SketchPlugin_ConstraintRadius);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
