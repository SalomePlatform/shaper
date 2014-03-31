// File:        Model_Document.hxx
// Created:     28 Dec 2011
// Author:      Mikhail PONIKAROV
// Copyright:   CEA 2011

#ifndef ModelAPI_Document_HeaderFile
#define ModelAPI_Document_HeaderFile

#include <ModelAPI.h>

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
  MODELAPI_EXPORT virtual bool Load(const char* theFileName) = 0;

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \returns true if file was stored successfully
  MODELAPI_EXPORT virtual bool Save(const char* theFileName) = 0;

  //! Removes document data
  MODELAPI_EXPORT virtual void Close() = 0;

  //! Starts a new operation (opens a tansaction)
  MODELAPI_EXPORT virtual void StartOperation() = 0;
  //! Finishes the previously started operation (closes the transaction)
  MODELAPI_EXPORT virtual void FinishOperation() = 0;
  //! Aborts the operation 
  MODELAPI_EXPORT virtual void AbortOperation() = 0;
  //! Returns true if operation has been started, but not yet finished or aborted
  MODELAPI_EXPORT virtual bool IsOperation() = 0;
  //! Returns true if document was modified (since creation/opening)
  MODELAPI_EXPORT virtual bool IsModified() = 0;

  //! Returns True if there are available Undos
  MODELAPI_EXPORT virtual bool CanUndo() = 0;
  //! Undoes last operation
  MODELAPI_EXPORT virtual void Undo() = 0;
  //! Returns True if there are available Redos
  MODELAPI_EXPORT virtual bool CanRedo() = 0;
  //! Redoes last operation
  MODELAPI_EXPORT virtual void Redo() = 0;

  /// Only for SWIG wrapping it is here
  MODELAPI_EXPORT ModelAPI_Document()
  {
  }
  ;
};

#endif
