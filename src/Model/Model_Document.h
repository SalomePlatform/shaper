// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Document.h
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Document_H_
#define Model_Document_H_

#include <Model.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>

#include <TDocStd_Document.hxx>
#include <map>
#include <set>

class Handle_Model_Document;
class Model_Objects;
class ModelAPI_AttributeSelectionList;

/**\class Model_Document
 * \ingroup DataModel
 * \brief Document for internal data structure of any object storage.
 * Document contains all data that must be stored/retrived in the file.
 * Also it provides acces to this data: open/save, transactions management etc.
 */
class Model_Document : public ModelAPI_Document
{
 public:
  //! Returns the kind of the document: "PartSet", "Part", or something else.
  //! This kind is used for feature buttons enable/disable depending on active document
  //! (it uses workbench "document" identifier in XML configuration file for this)
  MODEL_EXPORT virtual const std::string& kind() const {return myKind;}

  //! Loads the OCAF document from the file.
  //! \param theFileName full name of the file to load
  //! \param theThis the common shared pointer to the document to manage with it later
  //! \returns true if file was loaded successfully
  MODEL_EXPORT virtual bool load(const char* theFileName, DocumentPtr theThis);

  //! Saves the OCAF document to the file.
  //! \param theFileName full name of the file to store
  //! \param theResults the result full file names that were stored by "save"
  //! \returns true if file was stored successfully
  MODEL_EXPORT virtual bool save(const char* theFileName, std::list<std::string>& theResults);

  //! Removes document data
  //! \param theForever if it is false, document is just hiden (to keep possibility make it back on Undo/Redo)
  MODEL_EXPORT virtual void close(const bool theForever = false);

  //! Starts a new operation (opens a tansaction)
  MODEL_EXPORT virtual void startOperation();
  //! Finishes the previously started operation (closes the transaction)
  //! \returns true if transaction in this document is not empty and really was performed
  MODEL_EXPORT virtual bool finishOperation();
  //! Aborts the operation 
  MODEL_EXPORT virtual void abortOperation();
  //! Returns true if operation has been started, but not yet finished or aborted
  MODEL_EXPORT virtual bool isOperation() const;
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
  //! \param theID creates feature and puts it in the document
  //! \param theMakeCurrent to make current this new feature in this document
  MODEL_EXPORT virtual FeaturePtr addFeature(std::string theID, const bool theMakeCurrent = true);

  //! Return a list of features, which refers to the feature
  //! \param theFeature a feature
  //! \param theRefs a list of reference features
  //! \param isSendError a flag whether the error message should be send
  MODEL_EXPORT virtual void refsToFeature(FeaturePtr theFeature,
                                          std::set<FeaturePtr>& theRefs,
                                          const bool isSendError = true);

  //! Removes the feature from the document (with result)
  //! \param theFeature a removed feature
  MODEL_EXPORT virtual void removeFeature(FeaturePtr theFeature);

  //! Moves the feature to make it after the given one in the history.
  MODEL_EXPORT virtual void moveFeature(FeaturePtr theMoved, FeaturePtr theAfterThis);

  //! Returns the first found object in the group by the object name
  //! \param theGroupID group that contains an object
  //! \param theName name of the object to search
  //! \returns null if such object is not found
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Object> objectByName(
    const std::string& theGroupID, const std::string& theName);

  //! Returns the object index in the group. Object must be visible. Otherwise returns -1.
  //! \param theObject object of this document
  //! \returns index started from zero, or -1 if object is invisible or belongs to another document
  MODEL_EXPORT virtual const int index(std::shared_ptr<ModelAPI_Object> theObject);

  //! Adds a new sub-document by the identifier, or returns existing one if it is already exist
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Document> subDocument(std::string theDocID);

  //! Internal sub-document by ID
  MODEL_EXPORT virtual std::shared_ptr<Model_Document> subDoc(std::string theDocID);

  ///! Returns the id of hte document
  MODEL_EXPORT virtual const std::string& id() const
  {
    return myID;
  }

  //! Returns the feature in the group by the index (started from zero)
  //! \param theGroupID group that contains a feature
  //! \param theIndex zero-based index of feature in the group
  MODEL_EXPORT virtual ObjectPtr object(const std::string& theGroupID, const int theIndex);

  //! Returns the number of features in the group
  MODEL_EXPORT virtual int size(const std::string& theGroupID);

  //! Returns the feature that is currently edited in this document, normally
  //! this is the latest created feature
  //! \param theVisible use visible features only: flag is true for Object Browser functionality
  //! \returns null if next created feature must be the first
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature> currentFeature(const bool theVisible);

  //! Sets the current feature: all features below will be disabled, new features
  //! will be appended after this one.
  //! \param theCurrent the selected feature as current: blow it everythin become disabled
  //! \param theVisible use visible features only: flag is true for Object Browser functionality
  MODEL_EXPORT virtual void setCurrentFeature(std::shared_ptr<ModelAPI_Feature> theCurrent,
    const bool theVisible);

  //! Makes the current feature one feature upper
  MODEL_EXPORT virtual void setCurrentFeatureUp();

  //! Returns the number of all features: in the history or not
  MODEL_EXPORT virtual int numInternalFeatures();
  //! Returns the feature by zero-based index: features in the history or not
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature> internalFeature(const int theIndex);

  /// Creates a construction cresults
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultConstruction> createConstruction(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a body results
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultBody> createBody(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a part results
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultPart> createPart(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  //! Copies a part result, keeping the same data
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultPart> copyPart(
      const std::shared_ptr<ModelAPI_Result>& theOldPart, 
      const std::shared_ptr<ModelAPI_ResultPart>& theOrigin, 
      const int theIndex = 0);
  /// Creates a group results
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultGroup> createGroup(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);

  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultParameter> createParameter(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);

  //! Returns a feature by result (owner of result)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature>
    feature(const std::shared_ptr<ModelAPI_Result>& theResult);

  ///! Returns true if parametric updater need to execute feature on recomputartion
  ///! On abort, undo or redo it is not necessary: results in document are updated automatically
  bool& executeFeatures() {return myExecuteFeatures;}

  //! Registers the name of the shape for the topological naming needs
  void addNamingName(const TDF_Label theLabel, std::string theName);
  //! Returns the label, keeper of the name  for the topological naming needs
  TDF_Label findNamingName(std::string theName);
  //! Returns the result by name of the result (names of results must be unique, used for naming
  //! selection by name.
  ResultPtr findByName(const std::string theName);

  ///! Returns all features of the document including the hidden features which are not in
  ///! history. Not very fast method, for calling once, not in big cycles.
  MODEL_EXPORT virtual std::list<std::shared_ptr<ModelAPI_Feature> > allFeatures();

  /// Returns the global identifier of the current transaction (needed for the update algo)
  MODEL_EXPORT virtual int transactionID();
  /// Increases the transaction ID
  MODEL_EXPORT virtual void incrementTransactionID();
  /// Decreases the transaction ID
  MODEL_EXPORT virtual void decrementTransactionID();

  /// Returns true if document is opened and valid
  MODEL_EXPORT virtual bool isOpened();

 protected:
  //! Returns (creates if needed) the general label
  TDF_Label generalLabel() const;

  //! Creates new document with binary file format
  Model_Document(const std::string theID, const std::string theKind);

  //! Returns the internal OCCT document of this interface
  Handle_TDocStd_Document document()
  {
    return myDoc;
  }

  //! performs compactification of all nested operations into one
  //! \returns true if resulting transaction is not empty and can be undoed
  void compactNested();

  //! Returns all sub documents
  const std::set<std::string> subDocuments(const bool theActivatedOnly) const;

  //! The implementation of undo: with or without recoursive calls in the sub-documents
  void undoInternal(const bool theWithSubs, const bool theSynchronize);

  //! Stores the Id of the current operation (normally is called for the root document)
  void operationId(const std::string& theId);

  //! Returns the list of Ids of the operations that can be undoed (called for the root document)
  std::list<std::string> undoList() const;

  //! Returns the list of Ids of the operations that can be redoed (called for the root document)
  std::list<std::string> redoList() const;

  //! Internally makes document know that feature was removed or added in history after creation
  virtual void updateHistory(const std::shared_ptr<ModelAPI_Object> theObject);
  //! Internally makes document know that feature was removed or added in history after creation
  virtual void updateHistory(const std::string theGroup);

  //! Returns true if the document is root module document
  bool isRoot() const;

  //! Sets shared pointer to this
  void setThis(DocumentPtr theDoc);

  //! Returns the objects manager
  Model_Objects* objects() {return myObjs;}

  ///! Informs the document that it becomes active and some actions must be performed
  virtual void setActive(const bool theFlag);
  //! Returns true if this document is currently active
  virtual bool isActive() const;

  //! Returns the selection attribute that is used for calculation of selection externally from the document
  std::shared_ptr<ModelAPI_AttributeSelectionList> selectionInPartFeature();

  friend class Model_Application;
  friend class Model_Session;
  friend class Model_Update;
  friend class Model_AttributeReference;
  friend class Model_AttributeRefAttr;
  friend class Model_AttributeRefList;
  friend class Model_ResultPart;
  friend class DFBrowser;

 private:
  std::string myID;  ///< identifier of the document in the application
  std::string myKind;  ///< kind of the document in the application
  Handle_TDocStd_Document myDoc;  ///< OCAF document

  Model_Objects *myObjs; ///< data manager of this document

  //! counter value of transaction on the last "save" call, used for "IsModified" method
  int myTransactionSave;
  //! number of nested transactions performed (list becasue may be nested inside of nested)
  //! the list is empty if not nested transaction is performed
  std::list<int> myNestedNum;

  //! Information related to the every user-transaction
  struct Transaction {
    int myOCAFNum; ///< number of OCAF transactions related to each "this" transaction, may be 0
    std::string myId; ///< user-identifier string of transaction
    //! default constructor with default Id
    Transaction(): myOCAFNum(0), myId("") {}
  };

  //! transaction indexes (related to myTransactionsAfterSave) and info about the real transactions
  //! in myDocument connected to this operation (may be zero for empty transaction)
  std::list<Transaction> myTransactions;
  //! list of info about transactions undone (first is oldest undone)
  std::list<Transaction> myRedos;

  //! Optimization for finding the shape-label by topological naming names
  std::map<std::string, TDF_Label> myNamingNames;
  //! If it is true, features are not executed on update (on abort, undo, redo)
  bool myExecuteFeatures;

  bool myIsActive; ///< flag that stores the active/not active state

  //! The selection feature, if needed
  FeaturePtr mySelectionFeature;
};

#endif
