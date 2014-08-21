#include "FeaturesPlugin_Plugin.h"
#include "FeaturesPlugin_Extrusion.h"

#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>

using namespace std;

// the only created instance of this plugin
static FeaturesPlugin_Plugin* MY_INSTANCE = new FeaturesPlugin_Plugin();

FeaturesPlugin_Plugin::FeaturesPlugin_Plugin()
{
  // register this plugin
  ModelAPI_PluginManager::get()->registerPlugin(this);
}

FeaturePtr FeaturesPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == FeaturesPlugin_Extrusion::ID()) {
    return FeaturePtr(new FeaturesPlugin_Extrusion);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
