#include "PartSetPlugin_Plugin.h"
#include "PartSetPlugin_Part.h"
#include "PartSetPlugin_Duplicate.h"
#include "PartSetPlugin_Remove.h"
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

FeaturePtr PartSetPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == PartSetPlugin_Part::ID()) {
    return FeaturePtr(new PartSetPlugin_Part);
  }
  if (theFeatureID == "duplicate") {
    return FeaturePtr(new PartSetPlugin_Duplicate);
  }
  if (theFeatureID == "remove") {
    return FeaturePtr(new PartSetPlugin_Remove);
  }
    // feature of such kind is not found
  return FeaturePtr();
}
