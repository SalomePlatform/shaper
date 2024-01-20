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

#ifndef ModelAPI_Session_H_
#define ModelAPI_Session_H_

#include "ModelAPI.h"
#include <string>
#include <list>
#include <memory>

class ModelAPI_Feature;
class ModelAPI_Plugin;
class ModelAPI_Document;
class ModelAPI_ValidatorsFactory;
class ModelAPI_FiltersFactory;

/**\class ModelAPI_Session
 * \ingroup DataModel
 * \brief Object that knows (from the initial XML file) which
 * plugin contains which feature, loads and stores reference to loaded plugins by
 * the feature functionality request.
 */

class MODELAPI_EXPORT ModelAPI_Session
{
protected:
  bool myIsLoading; ///< keeps the state of the loading of the document

 public:
  /// Returns the real implementation (the alone instance per application) of the plugin manager
  static std::shared_ptr<ModelAPI_Session> get();

  //! Loads the OCAF document from the file.
  //! \param theFileName full name of the file to load
  //! \returns true if file was loaded successfully
  virtual bool load(const char* theFileName) = 0;

  //! Returns true if a loading process is performed (so, no need to react on a new part creation)
  virtual bool isLoading() { return myIsLoading; };

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \param theResults the result full file names that were stored by "save"
  //! \returns true if file was stored successfully
  virtual bool save(const char* theFileName, std::list<std::string>& theResults) = 0;

  //! Closes all documents
  virtual void closeAll() = 0;

  //! Starts a new operation (opens a transaction)
  //! \param theId of operation for history (optional)
  //! \param theAttachedToNested if it is true,
  //!          it means that this transaction is attached to the nested
  //!          where it is located and will be committed on the next commit with the nested
  virtual void startOperation(
    const std::string& theId = "", const bool theAttachedToNested = false) = 0;
  //! Finishes the previously started operation (closes the transaction)
  virtual void finishOperation() = 0;
  //! Aborts the operation
  virtual void abortOperation() = 0;
  //! Returns true if operation has been started, but not yet finished or aborted
  virtual bool isOperation() = 0;
  //! Returns true if document was modified (since creation/opening)
  virtual bool isModified() = 0;

  //! Returns True if there are available Undos
  virtual bool canUndo() = 0;
  //! Undoes last operation
  virtual void undo() = 0;
  //! Returns True if there are available Redos
  virtual bool canRedo() = 0;
  //! Redoes last operation
  virtual void redo() = 0;
  //! Returns stack of performed operations (from last to first)
  virtual std::list<std::string> undoList() = 0;
  //! Returns stack of rolled back operations (from last rolled back to first)
  virtual std::list<std::string> redoList() = 0;
  //! Clears undo and redo lists of all documents in the session
  virtual void clearUndoRedo() = 0;

  /// Registers the plugin that creates features.
  /// It is obligatory for each plugin to call this function on loading to be found by
  /// the plugin manager on call of the feature)
  virtual void registerPlugin(ModelAPI_Plugin* thePlugin) = 0;

  /// Verifies the license for the plugin is valid
  virtual bool checkLicense(const std::string& thePluginName) = 0;

  /// Returns the root document of the application (that may contains sub-documents)
  virtual std::shared_ptr<ModelAPI_Document> moduleDocument() = 0;

  /// Returns the document by ID. Returns null if no such document.
  virtual std::shared_ptr<ModelAPI_Document> document(int theDocID) = 0;

  /// Return true if root document has been already created
  virtual bool hasModuleDocument() = 0;

  /// Returns the current document that used for current work in the application
  virtual std::shared_ptr<ModelAPI_Document> activeDocument() = 0;

  /// Defines the current document that used for current work in the application
  virtual void setActiveDocument(
    std::shared_ptr<ModelAPI_Document> theDoc, bool theSendSignal = true) = 0;

  /// Returns all the opened documents of the session (without postponed)
  virtual std::list<std::shared_ptr<ModelAPI_Document> > allOpenedDocuments() = 0;

  /// Returns true if document is not loaded yet
  virtual bool isLoadByDemand(const std::wstring theDocID, const int theDocIndex) = 0;

  /// Copies the document to the new one with the given id
  virtual std::shared_ptr<ModelAPI_Document> copy(
    std::shared_ptr<ModelAPI_Document> theSource, const int theDestID) =0;

  /// Returns the validators factory: the only one instance per application
  virtual ModelAPI_ValidatorsFactory* validators() = 0;

  /// Returns the filters factory: the only one instance per application
  virtual ModelAPI_FiltersFactory* filters() = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Session()
  {
  }

  /// Returns the global identifier of the current transaction (needed for the update algo)
  virtual int transactionID() = 0;

  /// Returns true if auto-update in the application is blocked
  virtual bool isAutoUpdateBlocked() = 0;

  /// Set state of the auto-update of features result in the application
  virtual void blockAutoUpdate(const bool theBlock) = 0;

 protected:
  /// Sets the session interface implementation (once per application launch)
  static void setSession(std::shared_ptr<ModelAPI_Session> theManager);
};

typedef std::shared_ptr<ModelAPI_Session> SessionPtr;

#endif
