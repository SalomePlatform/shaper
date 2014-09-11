// File:        ModelAPI_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Document_H_
#define ModelAPI_Document_H_

#include <ModelAPI.h>
#include <string>
#include <boost/shared_ptr.hpp>
#include <vector>
#include <list>

class ModelAPI_Feature;
class ModelAPI_Object;
class ModelAPI_Result;
class ModelAPI_ResultConstruction;
class ModelAPI_ResultBody;
class ModelAPI_ResultPart;
class ModelAPI_Data;

/**\class Model_Document
 * \ingroup DataModel
 * \brief Document for internal data structure of any object storage.
 * Document contains all data that must be stored/retrived in the file.
 * Also it provides acces to this data: open/save, transactions management etc.
 */
class ModelAPI_Document
{
 public:
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

  //! Removes document data
  virtual void close() = 0;

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

  //! Adds to the document the new feature of the given feature id
  //! \param creates feature and puts it in the document (if it is not action)
  virtual boost::shared_ptr<ModelAPI_Feature> addFeature(std::string theID) = 0;

  //! Removes the feature from the document
  virtual void removeFeature(boost::shared_ptr<ModelAPI_Feature> theFeature) = 0;

  ///! Adds a new sub-document by the identifier, or returns existing one if it is already exist
  virtual boost::shared_ptr<ModelAPI_Document>
  subDocument(std::string theDocID) = 0;

  ///! Returns the id of the document
  virtual const std::string& id() const = 0;

  //! Returns the object in the group by the index (started from zero)
  //! \param theGroupID group that contains an object
  //! \param theIndex zero-based index of feature in the group
  //! \param theHidden if it is true, it counts also the features that are not in tree
  virtual boost::shared_ptr<ModelAPI_Object>
  object(const std::string& theGroupID, const int theIndex, const bool theHidden = false) = 0;

  //! Returns the number of objects in the group of objects
  //! If theHidden is true, it counts also the features that are not in tree
  virtual int size(const std::string& theGroupID, const bool theHidden = false) = 0;

  /// To virtually destroy the fields of successors
  virtual ~ModelAPI_Document()
  {
  }

  /// Creates a construction cresults
  virtual boost::shared_ptr<ModelAPI_ResultConstruction> createConstruction(
      const boost::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  /// Creates a body results
  virtual boost::shared_ptr<ModelAPI_ResultBody> createBody(
      const boost::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  /// Creates a part results
  virtual boost::shared_ptr<ModelAPI_ResultPart> createPart(
      const boost::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;

  //! Returns a feature by result (owner of result)
  virtual boost::shared_ptr<ModelAPI_Feature> feature(
      const boost::shared_ptr<ModelAPI_Result>& theResult) = 0;

 protected:
  /// Only for SWIG wrapping it is here
  MODELAPI_EXPORT ModelAPI_Document()
  {
  }
};

//! Pointer on document object
typedef boost::shared_ptr<ModelAPI_Document> DocumentPtr;

#endif
