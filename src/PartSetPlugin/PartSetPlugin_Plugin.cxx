#include "PartSetPlugin_Plugin.h"
#include "PartSetPlugin_NewPart.h"
#include <ModelAPI_PluginManager.h>
#include <ModelAPI_Document.h>

using namespace std;

// group identification of the newly created parts in the Part Set document
const int MY_PARTS_GROUP = 1;

// the only created instance of this plugin
static PartSetPlugin_Plugin* MY_INSTANCE = new PartSetPlugin_Plugin();

PartSetPlugin_Plugin::PartSetPlugin_Plugin() 
{
  // register this plugin
  ModelAPI_PluginManager::get()->registerPlugin(this);
}

boost::shared_ptr<ModelAPI_Feature> PartSetPlugin_Plugin::createFeature(string theFeatureID)
{
  boost::shared_ptr<ModelAPI_Feature> aCreated;
  if (theFeatureID == "new_part") {
    aCreated = boost::shared_ptr<ModelAPI_Feature>(new PartSetPlugin_NewPart());
  }
  // add to a root document for the current moment
  if (aCreated)
    ModelAPI_PluginManager::get()->rootDocument()->AddObject(aCreated, MY_PARTS_GROUP);
  // feature of such kind is not found
  return aCreated;
}
