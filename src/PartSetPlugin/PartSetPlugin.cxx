#include "PartSetPlugin.h"

#include <PartSetPlugin_NewPart.h>
#include <boost/shared_ptr.hpp>

using namespace std;

/// Standard method of the plugin that creates a specific feature instance by the feature kind
PARTSETPLUGIN_EXPORT boost::shared_ptr<ModelAPI_Feature> CreateFeature(const string& theFeatureKind) 
{
  if (theFeatureKind == "new_part") {
    return boost::shared_ptr<ModelAPI_Feature>(new PartSetPlugin_NewPart());
  }
  // feature of such kind is not found
  return boost::shared_ptr<ModelAPI_Feature>();
}
