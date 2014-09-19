// File:        ModelAPI_Session.hxx
// Created:     20 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Session_H_
#define ModelAPI_Session_H_

#include "ModelAPI.h"
#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

class ModelAPI_Feature;
class ModelAPI_Plugin;
class ModelAPI_Document;
class ModelAPI_ValidatorsFactory;

/**\class ModelAPI_Session
 * \ingroup DataModel
 * \brief Object that knows (from the initial XML file) which
 * plugin contains which feature, loads and stores reference to loaded plugins by
 * the feature functionality request.
 */

class MODELAPI_EXPORT ModelAPI_Session
{
 public:
  /// Returns the real implementation (the alone instance per application) of the plugin manager
  static boost::shared_ptr<ModelAPI_Session> get();

  //! Loads the OCAF document from the file.
  //! \param theFileName full name of the file to load
  //! \param theStudyID identifier of the SALOME study to associate with loaded file
  //! \returns true if file was loaded successfully
  virtual bool load(const char* theFileName) = 0;

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \param theResults the result full file names that were stored by "save"
  //! \returns true if file was stored successfully
  virtual bool save(const char* theFileName, std::list<std::string>& theResults) = 0;

  //! Starts a new operation (opens a tansaction)
  virtual void startOperation() = 0;
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

  /// Registers the plugin that creates features.
  /// It is obligatory for each plugin to call this function on loading to be found by 
  /// the plugin manager on call of the feature)
  virtual void registerPlugin(ModelAPI_Plugin* thePlugin) = 0;

  /// Returns the root document of the application (that may contains sub-documents)
  virtual boost::shared_ptr<ModelAPI_Document> moduleDocument() = 0;

  /// Return true if root document has been already created
  virtual bool hasModuleDocument() = 0;

  /// Returns the current document that used for current work in the application
  virtual boost::shared_ptr<ModelAPI_Document> activeDocument() = 0;

  /// Defines the current document that used for current work in the application
  virtual void setActiveDocument(boost::shared_ptr<ModelAPI_Document> theDoc) = 0;

  /// Returns all the opened documents of the session (without postponed)
  virtual std::list<boost::shared_ptr<ModelAPI_Document> > allOpenedDocuments() = 0;

  /// Copies the document to the new one with the given id
  virtual boost::shared_ptr<ModelAPI_Document> copy(boost::shared_ptr<ModelAPI_Document> theSource,
                                                    std::string theID) = 0;

  /// Returns the validators factory: the only one instance per application
  virtual ModelAPI_ValidatorsFactory* validators() = 0;

  /// Is needed for python wrapping by swig, call Get to get an instance
  ModelAPI_Session();

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Session()
  {
  }

 protected:
  /// Creates the feature object using plugins functionality
  virtual boost::shared_ptr<ModelAPI_Feature> createFeature(std::string theFeatureID) = 0;

  static void setSession(boost::shared_ptr<ModelAPI_Session> theManager);

  friend class Model_Document;
};

typedef boost::shared_ptr<ModelAPI_Session> SessionPtr;

#endif
