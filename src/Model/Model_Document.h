// File:        Model_Document.h
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Document_H_
#define Model_Document_H_

#include <Model.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>

#include <TDocStd_Document.hxx>
#include <NCollection_DataMap.hxx>
#include <TDF_Label.hxx>
#include <map>
#include <set>

class Handle_Model_Document;

// for TDF_Label map usage
static Standard_Integer HashCode(const TDF_Label& theLab, const Standard_Integer theUpper);
static Standard_Boolean IsEqual(const TDF_Label& theLab1, const TDF_Label& theLab2);

/**\class Model_Document
 * \ingroup DataModel
 * \brief Document for internal data structure of any object storage.
 * Document contains all data that must be stored/retrived in the file.
 * Also it provides acces to this data: open/save, transactions management etc.
 */
class Model_Document : public ModelAPI_Document
{
 public:

  //! Loads the OCAF document from the file.
  //! \param theFileName full name of the file to load
  //! \param theStudyID identifier of the SALOME study to associate with loaded file
  //! \returns true if file was loaded successfully
  MODEL_EXPORT virtual bool load(const char* theFileName);

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \returns true if file was stored successfully
  MODEL_EXPORT virtual bool save(const char* theFileName);

  //! Removes document data
  MODEL_EXPORT virtual void close();

  //! Starts a new operation (opens a tansaction)
  MODEL_EXPORT virtual void startOperation();
  //! Finishes the previously started operation (closes the transaction)
  //! Returns true if transaction in this document is not empty and really was performed
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

  //! Adds to the document the new feature of the given feature id
  //! \param creates feature and puts it in the document
  MODEL_EXPORT virtual FeaturePtr addFeature(std::string theID);

  //! Removes the feature from the document (with result)
  MODEL_EXPORT virtual void removeFeature(FeaturePtr theFeature);

  //! Returns the existing feature by the label
  //! \param theLabel base label of the feature
  MODEL_EXPORT virtual FeaturePtr feature(TDF_Label& theLabel);

  //! Returns the existing object: result or feature
  //! \param theLabel base label of the object
  MODEL_EXPORT virtual ObjectPtr object(TDF_Label theLabel);

  //! Adds a new sub-document by the identifier, or returns existing one if it is already exist
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Document> subDocument(std::string theDocID);

  ///! Returns the id of hte document
  MODEL_EXPORT virtual const std::string& id() const
  {
    return myID;
  }

  //! Returns the feature in the group by the index (started from zero)
  //! \param theGroupID group that contains a feature
  //! \param theIndex zero-based index of feature in the group
  //! \param isOperation if it is true, returns feature (not Object)
  //! \param theHidden if it is true, it counts also the features that are not in tree
  MODEL_EXPORT virtual ObjectPtr object(const std::string& theGroupID, const int theIndex,
                                        const bool theHidden = false);

  //! Returns the number of features in the group
  //! If theHidden is true, it counts also the features that are not in tree
  MODEL_EXPORT virtual int size(const std::string& theGroupID, const bool theHidden = false);

  /// Creates a construction cresults
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_ResultConstruction> createConstruction(
      const boost::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a body results
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_ResultBody> createBody(
      const boost::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a part results
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_ResultPart> createPart(
      const boost::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);

  //! Returns a feature by result (owner of result)
  MODEL_EXPORT virtual boost::shared_ptr<ModelAPI_Feature>
  feature(const boost::shared_ptr<ModelAPI_Result>& theResult);

 protected:

  //! Returns (creates if needed) the features label
  TDF_Label featuresLabel();

  //! Initializes feature with a unique name in this group (unique name is generated as 
  //! feature type + "_" + index
  void setUniqueName(FeaturePtr theFeature);

  //! Synchronizes myFeatures list with the updated document
  void synchronizeFeatures(const bool theMarkUpdated = false);

  //! Creates new document with binary file format
  Model_Document(const std::string theID);

  Handle_TDocStd_Document document()
  {
    return myDoc;
  }

  //! performas compactification of all nested operations into one
  void compactNested();

  //! Initializes the data fields of the feature
  void initData(ObjectPtr theObj, TDF_Label theLab, const int theTag);

  //! Allows to store the result in the data tree of the document (attaches 'data' of result to tree)
  MODEL_EXPORT virtual void storeResult(boost::shared_ptr<ModelAPI_Data> theFeatureData,
                                        boost::shared_ptr<ModelAPI_Result> theResult,
                                        const int theResultIndex = 0);

  friend class Model_Application;
  friend class Model_PluginManager;
  friend class DFBrowser;

 private:
  std::string myID;  ///< identifier of the document in the application
  Handle_TDocStd_Document myDoc;  ///< OCAF document
  /// number of transactions after the last "save" call, used for "IsModified" method
  int myTransactionsAfterSave;
  /// number of nested transactions performed (or -1 if not nested)
  int myNestedNum;
  /// All features managed by this document (not only in history of OB)
  /// For optimization mapped by labels
  NCollection_DataMap<TDF_Label, FeaturePtr> myObjs;

  ///< set of identifiers of sub-documents of this document
  std::set<std::string> mySubs;
  /// transaction indexes (related to myTransactionsAfterSave) which were empty in this doc
  std::map<int, bool> myIsEmptyTr;
};

#endif
