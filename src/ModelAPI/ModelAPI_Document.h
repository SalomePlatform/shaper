// File:        ModelAPI_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Document_HeaderFile
#define ModelAPI_Document_HeaderFile

#include <ModelAPI.h>
#include <string>
#include <memory>

class ModelAPI_Feature;
class ModelAPI_Iterator;

/// Common groups identifiers
/// Group of parameters
static const std::string PARAMETERS_GROUP = "Parameters";
/// Group of constructions
static const std::string CONSTRUCTIONS_GROUP = "Construction";
/// Group of parts
static const std::string PARTS_GROUP = "Parts";

/**\class Model_Document
 * \ingroup DataModel
 * \brief Document for internal data structure of any object storage. Corresponds to the SALOME study.
 * Document contains all data of te SALOME Study specific to this module
 * that must be written into the HDF file.
 * Also it provides acces to this data: open/save, transactions management etc.
 * to provide access to all stored data.
 */
class ModelAPI_Document
{
public:
  //! Loads the OCAF document from the file.
  //! \param theFileName full name of the file to load
  //! \param theStudyID identifier of the SALOME study to associate with loaded file
  //! \returns true if file was loaded successfully
  MODELAPI_EXPORT virtual bool load(const char* theFileName) = 0;

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \returns true if file was stored successfully
  MODELAPI_EXPORT virtual bool save(const char* theFileName) = 0;

  //! Removes document data
  MODELAPI_EXPORT virtual void close() = 0;

  //! Starts a new operation (opens a tansaction)
  MODELAPI_EXPORT virtual void startOperation() = 0;
  //! Finishes the previously started operation (closes the transaction)
  MODELAPI_EXPORT virtual void finishOperation() = 0;
  //! Aborts the operation 
  MODELAPI_EXPORT virtual void abortOperation() = 0;
  //! Returns true if operation has been started, but not yet finished or aborted
  MODELAPI_EXPORT virtual bool isOperation() = 0;
  //! Returns true if document was modified (since creation/opening)
  MODELAPI_EXPORT virtual bool isModified() = 0;

  //! Returns True if there are available Undos
  MODELAPI_EXPORT virtual bool canUndo() = 0;
  //! Undoes last operation
  MODELAPI_EXPORT virtual void undo() = 0;
  //! Returns True if there are available Redos
  MODELAPI_EXPORT virtual bool canRedo() = 0;
  //! Redoes last operation
  MODELAPI_EXPORT virtual void redo() = 0;

  //! Adds to the document the new feature of the given group id
  //! \param theFeature a feature object that will be connected to the document in this method
  //! \param theGroupID identifier of the groups of object
  MODELAPI_EXPORT virtual void addFeature(std::shared_ptr<ModelAPI_Feature> theFeature,
    const std::string theGroupID) = 0;

  ///! Adds a new sub-document by the identifier, or returns existing one if it is already exist
  MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_Document> subDocument(std::string theDocID) = 0;

  ///! Creates an iterator of the features by the specific groups
  MODELAPI_EXPORT virtual std::shared_ptr<ModelAPI_Iterator> featuresIterator(
    const std::string theGroup) = 0;

  MODELAPI_EXPORT virtual const std::string& id() const = 0;

protected:
  /// Only for SWIG wrapping it is here
  MODELAPI_EXPORT ModelAPI_Document()
  {
  }
  ;
};

#endif
