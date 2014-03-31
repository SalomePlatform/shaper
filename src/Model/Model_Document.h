// File:        Model_Document.hxx
// Created:     28 Dec 2011
// Author:      Mikhail PONIKAROV
// Copyright:   CEA 2011

#ifndef Model_Document_HeaderFile
#define Model_Document_HeaderFile

#include <Model.h>
#include <ModelAPI_Document.h>
#include <TDocStd_Document.hxx>

class Handle_Model_Document;

/**\class Model_Document
 * \ingroup DataModel
 * \brief Document for internal data structure of any object storage. Corresponds to the SALOME study.
 * Document contains all data of te SALOME Study specific to this module
 * that must be written into the HDF file.
 * Also it provides acces to this data: open/save, transactions management etc.
 * to provide access to all stored data.
 */

class Model_Document: public TDocStd_Document, public ModelAPI_Document
{
public:

  DEFINE_STANDARD_RTTI(Model_Document)
  ;

  //! Creates new document by the format string of a storage
  Model_Document(const TCollection_ExtendedString& theStorageFormat);
  //! Deletes all high-level data, managed this document
  ~Model_Document();

  //! Loads the OCAF document from the file.
  //! \param theFileName full name of the file to load
  //! \param theStudyID identifier of the SALOME study to associate with loaded file
  //! \returns true if file was loaded successfully
  MODEL_EXPORT bool Load(const char* theFileName);

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \returns true if file was stored successfully
  MODEL_EXPORT bool Save(const char* theFileName);

  //! Removes document data
  MODEL_EXPORT void Close();

  //! Starts a new operation (opens a tansaction)
  MODEL_EXPORT void StartOperation();
  //! Finishes the previously started operation (closes the transaction)
  MODEL_EXPORT void FinishOperation();
  //! Aborts the operation 
  MODEL_EXPORT void AbortOperation();
  //! Returns true if operation has been started, but not yet finished or aborted
  MODEL_EXPORT bool IsOperation();
  //! Returns true if document was modified (since creation/opening)
  MODEL_EXPORT bool IsModified();

  //! Returns True if there are available Undos
  MODEL_EXPORT bool CanUndo();
  //! Undoes last operation
  MODEL_EXPORT void Undo();
  //! Returns True if there are available Redos
  MODEL_EXPORT bool CanRedo();
  //! Redoes last operation
  MODEL_EXPORT void Redo();

protected:

private:
  int myTransactionsAfterSave; ///< number of transactions after the last "save" call, used for "IsModified" method
};

// Define handle class 
DEFINE_STANDARD_HANDLE(Model_Document, TDocStd_Document)

#endif
