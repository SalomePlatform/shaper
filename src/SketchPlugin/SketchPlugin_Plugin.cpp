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
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>

using namespace std;

// the only created instance of this plugin
static SketchPlugin_Plugin* MY_INSTANCE = new SketchPlugin_Plugin();

SketchPlugin_Plugin::SketchPlugin_Plugin() 
{
  // register this plugin
  ModelAPI_PluginManager::get()->registerPlugin(this);
}

FeaturePtr SketchPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == SKETCH_KIND) {
    return FeaturePtr(new SketchPlugin_Sketch);
  }
  else if (theFeatureID == SKETCH_POINT_KIND) {
    return FeaturePtr(new SketchPlugin_Point);
  }
  else if (theFeatureID == SKETCH_LINE_KIND) {
    return FeaturePtr(new SketchPlugin_Line);
  }
  else if (theFeatureID == SKETCH_CIRCLE_KIND) {
    return FeaturePtr(new SketchPlugin_Circle);
  }
  else if (theFeatureID == SKETCH_ARC_KIND) {
    return FeaturePtr(new SketchPlugin_Arc);
  }
  else if (theFeatureID == SKETCH_CONSTRAINT_COINCIDENCE_KIND) {
    return FeaturePtr(new SketchPlugin_ConstraintCoincidence);
  }
  else if (theFeatureID == SKETCH_CONSTRAINT_DISTANCE_KIND) {
    return FeaturePtr(new SketchPlugin_ConstraintDistance);
  }
  else if (theFeatureID == SKETCH_CONSTRAINT_LENGTH_KIND) {
    return FeaturePtr(new SketchPlugin_ConstraintLength);
  }
  else if (theFeatureID == SKETCH_CONSTRAINT_PARALLEL_KIND) {
    return FeaturePtr(new SketchPlugin_ConstraintParallel);
  }
  else if (theFeatureID == SKETCH_CONSTRAINT_PERPENDICULAR_KIND) {
    return FeaturePtr(new SketchPlugin_ConstraintPerpendicular);
  }
  else if (theFeatureID == SKETCH_CONSTRAINT_RADIUS_KIND) {
    return FeaturePtr(new SketchPlugin_ConstraintRadius);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
