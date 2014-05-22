// File:        Model_PluginManager.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_PluginManager_HeaderFile
#define Model_PluginManager_HeaderFile

#include "Model.h"
#include <ModelAPI_PluginManager.h>
#include <Events_Listener.h>
#include <map>

class Model_Document;

/**\class Model_PluginManager
 * \ingroup DataModel
 * \brief Object that knows (from the initial XML file) which
 * plugin contains which feature, loads and stores reference to loaded plugins by
 * the feature functionality request.
 */
class Model_PluginManager : public ModelAPI_PluginManager, public Events_Listener
{
  bool myPluginsInfoLoaded; ///< it true if plugins information is loaded
  /// map of feature IDs to plugin name
  std::map<std::string, std::string> myPlugins;
  std::map<std::string, ModelAPI_Plugin*> myPluginObjs; ///< instances of the already plugins
  std::string myCurrentPluginName; ///< name of the plugin that must be loaded currently
  boost::shared_ptr<ModelAPI_Document> myCurrentDoc; ///< current working document
public:
  /// Returns the root document of the application (that may contains sub-documents)
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Document> rootDocument();

  /// Return true if root document has been already created
  MODEL_EXPORT virtual bool hasRootDocument();

  /// Returns the current document that used for current work in the application
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Document> currentDocument();

  /// Defines the current document that used for current work in the application
  MODEL_EXPORT virtual void setCurrentDocument(boost::shared_ptr<ModelAPI_Document> theDoc);

  /// Registers the plugin that creates features.
  /// It is obligatory for each plugin to call this function on loading to be found by 
  /// the plugin manager on call of the feature)
  MODEL_EXPORT virtual void registerPlugin(ModelAPI_Plugin* thePlugin);

  /// Processes the configuration file reading
  MODEL_EXPORT virtual void processEvent(const Events_Message* theMessage);

  /// Copies the document to the new one wit hthe given id
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Document> copy(
    boost::shared_ptr<ModelAPI_Document> theSource, std::string theID);

  /// Is called only once, on startup of the application
  Model_PluginManager();

protected:
  /// Loads (if not done yet) the information about the features and plugins
  void LoadPluginsInfo();

  /// Creates the feature object using plugins functionality
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature(std::string theFeatureID);
};

#endif
