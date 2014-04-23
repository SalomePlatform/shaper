#include "PartSetPlugin_Plugin.h"
#include "PartSetPlugin_Part.h"
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>

using namespace std;

// the only created instance of this plugin
static PartSetPlugin_Plugin* MY_INSTANCE = new PartSetPlugin_Plugin();

PartSetPlugin_Plugin::PartSetPlugin_Plugin() 
{
  // register this plugin
  ModelAPI_PluginManager::get()->registerPlugin(this);
}

shared_ptr<ModelAPI_Feature> PartSetPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == "Part") {
    return shared_ptr<ModelAPI_Feature>(new PartSetPlugin_Part);
  }
  // feature of such kind is not found
  return shared_ptr<ModelAPI_Feature>();
}
