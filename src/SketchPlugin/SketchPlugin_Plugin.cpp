#include "SketchPlugin_Plugin.h"
#include "SketchPlugin_Sketch.h"
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

boost::shared_ptr<ModelAPI_Feature> SketchPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == "Sketch") {
    return boost::shared_ptr<ModelAPI_Feature>(new SketchPlugin_Sketch);
  }
  /*else if (theFeatureID == "Point") {
    return shared_ptr<ModelAPI_Feature>(new SketchPlugin_Point);
  }*/
  // feature of such kind is not found
  return boost::shared_ptr<ModelAPI_Feature>();
}
