#include "PartSetPlugin_Plugin.h"
#include "PartSetPlugin_Part.h"
#include "PartSetPlugin_Duplicate.h"
#include "PartSetPlugin_Remove.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>

using namespace std;

// the only created instance of this plugin
static PartSetPlugin_Plugin* MY_PARTSET_INSTANCE = new PartSetPlugin_Plugin();

PartSetPlugin_Plugin::PartSetPlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr PartSetPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == PartSetPlugin_Part::ID()) {
    return FeaturePtr(new PartSetPlugin_Part);
  }
  if (theFeatureID == PartSetPlugin_Duplicate::ID()) {
    return FeaturePtr(new PartSetPlugin_Duplicate);
  }
  if (theFeatureID == PartSetPlugin_Remove::ID()) {
    return FeaturePtr(new PartSetPlugin_Remove);
  }
  // feature of such kind is not found
  return FeaturePtr();
}
