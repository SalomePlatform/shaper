// File:        Model_PluginManager.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_PluginManager_HeaderFile
#define Model_PluginManager_HeaderFile

#include "Model.h"
#include <ModelAPI_PluginManager.h>
#include <Event_Listener.h>
#include <map>

/**\class Model_PluginManager
 * \ingroup DataModel
 * \brief Object that knows (from the initial XML file) which
 * plugin contains which feature, loads and stores reference to loaded plugins by
 * the feature functionality request.
 */

class Model_PluginManager : public ModelAPI_PluginManager, public Event_Listener
{
  bool myPluginsInfoLoaded; ///< it true if plugins information is loaded
  std::map<std::string, std::string> myPlugins; ///< map of feature IDs to plugin name
public:
  /// Creates the feature object using plugins functionality
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Feature> CreateFeature(std::string theFeatureID);

  /// Processes the configuration file reading
  MODEL_EXPORT virtual void ProcessEvent(const Event_Message* theMessage);

  /// Is called only once, on startup of the application
  Model_PluginManager();

private:
  /// Loads (if not done yet) the information about the features and plugins
  void LoadPluginsInfo();
};

#endif
