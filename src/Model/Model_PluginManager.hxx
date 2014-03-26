// File:        Model_PluginManager.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_PluginManager_HeaderFile
#define Model_PluginManager_HeaderFile

#include "Model.hxx"
#include <ModelAPI_PluginManager.hxx>
#include <Event_Listener.hxx>

/**\class Model_PluginManager
 * \ingroup DataModel
 * \brief Object that knows (from the initial XML file) which
 * plugin contains which feature, loads and stores reference to loaded plugins by
 * the feature functionality request.
 */

class MODEL_EXPORT Model_PluginManager : public ModelAPI_PluginManager, public Event_Listener
{
public:
  /// Creates the feature object using plugins functionality
  virtual boost::shared_ptr<ModelAPI_Feature> CreateFeature(std::string theFeatureID);

  /// Processes the configuration file reading
  virtual void ProcessEvent(const Event_Message* theMessage);

private:
  /// Is called only once, on startup of the application
  Model_PluginManager();
};

#endif
