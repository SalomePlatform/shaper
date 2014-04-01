// File:        ModelAPI_PluginManager.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_PluginManager_HeaderFile
#define ModelAPI_PluginManager_HeaderFile

#include "ModelAPI.h"
#include <string>
#include <boost/shared_ptr.hpp>

class ModelAPI_Feature;
class ModelAPI_Plugin;

/**\class ModelAPI_PluginManager
 * \ingroup DataModel
 * \brief Object that knows (from the initial XML file) which
 * plugin contains which feature, loads and stores reference to loaded plugins by
 * the feature functionality request.
 */

class MODELAPI_EXPORT ModelAPI_PluginManager
{
public:
  /// Creates the feature object using plugins functionality
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature(std::string theFeatureID) = 0;

  /// Returns the real implementation (the alone instance per application) of the plugin manager
  static boost::shared_ptr<ModelAPI_PluginManager> get();

  /// Registers the plugin that creates features.
  /// It is obligatory for each plugin to call this function on loading to be found by 
  /// the plugin manager on call of the feature)
  virtual void registerPlugin(ModelAPI_Plugin* thePlugin) = 0;

  /// loads the library with specific name, appends "lib*.dll" or "*.so" depending on the platform
  static void ModelAPI_PluginManager::loadLibrary(const std::string theLibName);

  /// Is needed for python wrapping by swig, call Get to get an instance
  ModelAPI_PluginManager();

protected:
  static void SetPluginManager(boost::shared_ptr<ModelAPI_PluginManager> theManager);
};

#endif
