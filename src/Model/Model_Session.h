// Copyright (C) 2014-2024  CEA, EDF
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef Model_Session_H_
#define Model_Session_H_

#include "Model.h"
#include <ModelAPI_Session.h>
#include <ModelAPI_Feature.h>

#include <Events_Listener.h>
#include <map>

#ifdef TINSPECTOR
#include <TDocStd_Application.hxx>
#endif

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
  std::shared_ptr<ModelAPI_Document> myCurrentDoc;  ///< current working document
   ///< map from plugin id to plugins which are used by it (must be loaded before this one)
  std::map<std::string, std::string> myUsePlugins;

  /// if true, generates error if document is updated outside of transaction
  bool myCheckTransactions;
  /// if true, the current operation must be committed twice,
  /// with nested (list for any nesting depth)
  std::list<bool> myOperationAttachedToNext;
 public:

  //! Loads the OCAF document from the file.
  //! \param theFileName full name of the file to load
  //! \returns true if file was loaded successfully
  MODEL_EXPORT virtual bool load(const char* theFileName);

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \param theResults the result full file names that were stored by "save"
  //! \returns true if file was stored successfully
  MODEL_EXPORT virtual bool save(const char* theFileName, std::list<std::string>& theResults);

  //! Closes all documents
  MODEL_EXPORT virtual void closeAll();

  //! Starts a new operation (opens a transaction)
  //! \param theId string-identifier of the started transaction
  //! \param theAttachedToNested if it is true,
  //!                            it means that this transaction is attached to the nested
  //!          where it is located and will be committed on the next commit with the nested
  MODEL_EXPORT virtual void startOperation(
    const std::string& theId = "", const bool theAttachedToNested = false);
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
  //! Returns stack of performed operations
  MODEL_EXPORT virtual std::list<std::string> undoList();
  //! Returns stack of rolled back operations
  MODEL_EXPORT virtual std::list<std::string> redoList();
  //! Clears undo and redo lists of all documents in the session
  MODEL_EXPORT virtual void clearUndoRedo();

  /// Returns the root document of the application (that may contains sub-documents)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> moduleDocument();

  /// Returns the document by ID, loads if not loaded yet. Returns null if no such document.
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> document(int theDocID);

  /// Return true if root document has been already created
  MODEL_EXPORT virtual bool hasModuleDocument();

  /// Returns the current document that used for current work in the application
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> activeDocument();

  /// Defines the current document that used for current work in the application
  MODEL_EXPORT virtual void setActiveDocument(
    std::shared_ptr<ModelAPI_Document> theDoc, bool theSendSignal = true);

  /// Returns all the opened documents of the session (without postponed)
  MODEL_EXPORT virtual std::list<std::shared_ptr<ModelAPI_Document> > allOpenedDocuments();

  /// Returns true if document is not loaded yet
  MODEL_EXPORT virtual bool isLoadByDemand(const std::wstring theDocID, const int theDocIndex);

  /// Registers the plugin that creates features.
  /// It is obligatory for each plugin to call this function on loading to be found by
  /// the plugin manager on call of the feature)
  MODEL_EXPORT virtual void registerPlugin(ModelAPI_Plugin* thePlugin);

  /// Verifies the license for the plugin is valid
  MODEL_EXPORT virtual bool checkLicense(const std::string& thePluginName);

  /// Processes the configuration file reading
  MODEL_EXPORT virtual void processEvent(const std::shared_ptr<Events_Message>& theMessage);

  /// Copies the document to the new one
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> copy(
      std::shared_ptr<ModelAPI_Document> theSource, const int theDestID);

  /// Returns the validators factory: the only one instance per application
  MODEL_EXPORT virtual ModelAPI_ValidatorsFactory* validators();

  /// Returns the filters factory: the only one instance per application
  MODEL_EXPORT virtual ModelAPI_FiltersFactory* filters();

  /// Sets the flag to check modifications outside the transaction or not
  void setCheckTransactions(const bool theCheck)
  {
    myCheckTransactions = theCheck;
  }

  /// Is called only once, on startup of the application
  Model_Session();

  /// Returns the global identifier of the current transaction (needed for the update algo)
  MODEL_EXPORT virtual int transactionID();


  /// Returns true if auto-update in the application is blocked
  MODEL_EXPORT virtual bool isAutoUpdateBlocked();

  /// Set state of the auto-update of features result in the application
  MODEL_EXPORT virtual void blockAutoUpdate(const bool theBlock);

#ifdef TINSPECTOR
  MODEL_EXPORT virtual Handle(TDocStd_Application) application();
#endif

 protected:
  /// Loads (if not done yet) the information about the features and plugins
  void LoadPluginsInfo();

  /// Creates the feature object using plugins functionality
  FeaturePtr createFeature(std::string theFeatureID, Model_Document* theDocOwner);

  /// Get the plugin by name. If it is not loaded, load plugin.
  ModelAPI_Plugin* getPlugin(const std::string& thePluginName);

  friend class Model_Document;
  friend class Model_Objects;
};

#endif
