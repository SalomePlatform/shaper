// File:        Model_Session.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Session_H_
#define Model_Session_H_

#include "Model.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Feature.h>

#include <Events_Listener.h>
#include <map>

class Model_Document;

/**\class Model_Session
 * \ingroup DataModel
 * \brief Object that knows (from the initial XML file) which
 * plugin contains which feature, loads and stores reference to loaded plugins by
 * the feature functionality request.
 */
class Model_Session : public ModelAPI_Session, public Events_Listener
{
  bool myPluginsInfoLoaded;  ///< it true if plugins information is loaded
  /// map of feature IDs to plugin name and document kind
  std::map<std::string, std::pair<std::string, std::string> > myPlugins; 
  std::map<std::string, ModelAPI_Plugin*> myPluginObjs;  ///< instances of the already plugins
  std::string myCurrentPluginName;  ///< name of the plugin that must be loaded currently
  boost::shared_ptr<ModelAPI_Document> myCurrentDoc;  ///< current working document
  bool myCheckTransactions;  ///< if true, generates error if document is updated outside of transaction
 public:

  //! Loads the OCAF document from the file.
  //! \param theFileName full name of the file to load
  //! \param theStudyID identifier of the SALOME study to associate with loaded file
  //! \returns true if file was loaded successfully
  MODEL_EXPORT virtual bool load(const char* theFileName);

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \param theResults the result full file names that were stored by "save"
  //! \returns true if file was stored successfully
  MODEL_EXPORT virtual bool save(const char* theFileName, std::list<std::string>& theResults);

  //! Starts a new operation (opens a tansaction)
  MODEL_EXPORT virtual void startOperation();
  //! Finishes the previously started operation (closes the transaction)
  MODEL_EXPORT virtual void finishOperation();
  //! Aborts the operation 
  MODEL_EXPORT virtual void abortOperation();
  //! Returns true if operation has been started, but not yet finished or aborted
  MODEL_EXPORT virtual bool isOperation();
  //! Returns true if document was modified (since creation/opening)
  MODEL_EXPORT virtual bool isModified();

  //! Returns True if there are available Undos
  MODEL_EXPORT virtual bool canUndo();
  //! Undoes last operation
  MODEL_EXPORT virtual void undo();
  //! Returns True if there are available Redos
  MODEL_EXPORT virtual bool canRedo();
  //! Redoes last operation
  MODEL_EXPORT virtual void redo();

  /// Returns the root document of the application (that may contains sub-documents)
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Document> moduleDocument();

  /// Return true if root document has been already created
  MODEL_EXPORT virtual bool hasModuleDocument();

  /// Returns the current document that used for current work in the application
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Document> activeDocument();

  /// Defines the current document that used for current work in the application
  MODEL_EXPORT virtual void setActiveDocument(boost::shared_ptr<ModelAPI_Document> theDoc);

  /// Registers the plugin that creates features.
  /// It is obligatory for each plugin to call this function on loading to be found by 
  /// the plugin manager on call of the feature)
  MODEL_EXPORT virtual void registerPlugin(ModelAPI_Plugin* thePlugin);

  /// Processes the configuration file reading
  MODEL_EXPORT virtual void processEvent(const boost::shared_ptr<Events_Message>& theMessage);

  /// Copies the document to the new one wit hthe given id
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Document> copy(
      boost::shared_ptr<ModelAPI_Document> theSource, std::string theID);

  /// Returns the validators factory: the only one instance per application
  MODEL_EXPORT virtual ModelAPI_ValidatorsFactory* validators();

  void setCheckTransactions(const bool theCheck)
  {
    myCheckTransactions = theCheck;
  }

  /// Is called only once, on startup of the application
  Model_Session();

 protected:
  /// Loads (if not done yet) the information about the features and plugins
  void LoadPluginsInfo();

  /// Creates the feature object using plugins functionality
  virtual FeaturePtr createFeature(std::string theFeatureID);
};

#endif
