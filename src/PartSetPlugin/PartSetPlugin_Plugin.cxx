#include "PartSetPlugin_Plugin.h"
#include "PartSetPlugin_NewPart.h"
#include <ModelAPI_PluginManager.h>

using namespace std;

// the only created instance of this plugin
static PartSetPlugin_Plugin* MY_INSTANCE = new PartSetPlugin_Plugin();

PartSetPlugin_Plugin::PartSetPlugin_Plugin() 
{
  // register this plugin
  ModelAPI_PluginManager::get()->registerPlugin(this);
}

boost::shared_ptr<ModelAPI_Feature> PartSetPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == "new_part") {
    return boost::shared_ptr<ModelAPI_Feature>(new PartSetPlugin_NewPart());
  }
  // feature of such kind is not found
  return boost::shared_ptr<ModelAPI_Feature>();
}
