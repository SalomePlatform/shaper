// Copyright (C) 2014-2023  CEA, EDF
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
 * Document contains all data that must be stored/retrieved in the file.
 * Also it provides access to this data: open/save, transactions management etc.
 */
class Model_Document : public ModelAPI_Document
{
 public:
  //! Returns the kind of the document: "PartSet", "Part", or something else.
  //! This kind is used for feature buttons enable/disable depending on active document
  //! (it uses workbench "document" identifier in XML configuration file for this)
  MODEL_EXPORT virtual const std::string& kind() const {return myKind;}

  //! Loads the OCAF document from the file.
  //! \param theDirName directory of the loaded file
  //! \param theFileName a name of the file to load
  //! \param theThis the common shared pointer to the document to manage with it later
  //! \returns true if file was loaded successfully
  MODEL_EXPORT virtual bool load(
    const char* theDirName, const char* theFileName, DocumentPtr theThis);

  //! Loads the OCAF document from the file into the current document.
  //! All the features are added after the active feature.
  //! \param theFileName name of the file to import
  //! \param theImported list of features imported from the file
  //! \param theCheckOnly verify the document does not contain unappropriate features
  //!                     (useful for import to PartSet), but do not import it
  //! \returns true if file was loaded successfully
  MODEL_EXPORT virtual bool importPart(const char* theFileName,
                                   std::list<std::shared_ptr<ModelAPI_Feature> >& theImported,
                                   bool theCheckOnly = false);

  //! Saves the OCAF document to the file.
  //! \param theDirName directory where the document will be saved
  //! \param theFileName a name of the document file to store
  //! \param theResults the result full file names that were stored by "save"
  //! \returns true if file was stored successfully
  MODEL_EXPORT virtual bool save(
    const char* theDirName, const char* theFileName, std::list<std::string>& theResults);

  //! Export the list of features to the file
  //! \param theFilename path to save the file
  //! \param theExportFeatures list of features to export
  MODEL_EXPORT virtual bool save(const char* theFilename,
    const std::list<std::shared_ptr<ModelAPI_Feature> >& theExportFeatures) const;

  //! Removes document data
  //! \param theForever if it is false, document is just hidden
  //!                   (to keep possibility make it back on Undo/Redo)
  MODEL_EXPORT virtual void close(const bool theForever = false);

  //! Starts a new operation (opens a transaction)
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

  //! Returns True if there are available Undo-s
  MODEL_EXPORT virtual bool canUndo();
  //! Undoes last operation
  MODEL_EXPORT virtual void undo();
  //! Returns True if there are available Redo-s
  MODEL_EXPORT virtual bool canRedo();
  //! Redoes last operation
  MODEL_EXPORT virtual void redo();
  //! Clears undo/redo lists
  MODEL_EXPORT virtual void clearUndoRedo();

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
  //! It is necessary to flush REDISPLAY signal manually after this method because
  //! the method sends it, but for the performance purpose does not flush it
  //! \param theFeature a removed feature
  MODEL_EXPORT virtual void removeFeature(FeaturePtr theFeature);

  //! Moves the feature to make it after the given one in the history.
  MODEL_EXPORT virtual void moveFeature(
    FeaturePtr theMoved, FeaturePtr theAfterThis, const bool theSplit = false);

  //! Returns the first found object in the group by the object name
  //! \param theGroupID group that contains an object
  //! \param theName name of the object to search
  //! \returns null if such object is not found
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Object> objectByName(
    const std::string& theGroupID, const std::wstring& theName);

  //! Returns the object index in the group. Object must be visible. Otherwise returns -1.
  //! \param theObject object of this document
  //! \param theAllowFolder take into account grouping feature by folders
  //! \returns index started from zero, or -1 if object is invisible or belongs to another document
  MODEL_EXPORT virtual const int index(std::shared_ptr<ModelAPI_Object> theObject,
                                       const bool theAllowFolder = false);

  //! Internal sub-document by ID
  MODEL_EXPORT virtual std::shared_ptr<Model_Document> subDoc(int theDocID);

  ///! Returns the id of the document
  MODEL_EXPORT virtual const int id() const
  {
    return myID;
  }

  //! Returns the feature in the group by the index (started from zero)
  //! \param theGroupID group that contains a feature
  //! \param theIndex zero-based index of feature in the group
  //! \param theAllowFolder take into account grouping feature by folders
  MODEL_EXPORT virtual ObjectPtr object(const std::string& theGroupID,
                                        const int theIndex,
                                        const bool theAllowFolder = false);

  //! Returns the number of features in the group
  //! \param theGroupID group of objects
  //! \param theAllowFolder take into account grouping feature by folders
  MODEL_EXPORT virtual int size(const std::string& theGroupID, const bool theAllowFolder = false);

  //! Returns the parent object of this child. This may be result or feature, parent of a
  //! top result. Fast method, that uses internal data structure specifics.
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Object> parent(
    const std::shared_ptr<ModelAPI_Object> theChild);

  //! Returns the feature that is currently edited in this document, normally
  //! this is the latest created feature
  //! \param theVisible use visible features only: flag is true for Object Browser functionality
  //! \returns null if next created feature must be the first
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature> currentFeature(const bool theVisible);

  //! Sets the current feature: all features below will be disabled, new features
  //! will be appended after this one.
  //! \param theCurrent the selected feature as current: below it everything becomes disabled
  //! \param theVisible use visible features only: flag is true for Object Browser functionality
  MODEL_EXPORT virtual void setCurrentFeature(std::shared_ptr<ModelAPI_Feature> theCurrent,
    const bool theVisible);

  //! Makes the current feature one feature upper
  MODEL_EXPORT virtual void setCurrentFeatureUp();

  //! Returns the number of all features: in the history or not
  MODEL_EXPORT virtual int numInternalFeatures();
  //! Returns the feature by zero-based index: features in the history or not
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature> internalFeature(const int theIndex);
  //! Performs synchronization of transactions with the module document:
  //! If some document is not active (by undo of activation) but in memory,
  //! on activation the transactions must be synchronized because all redo-s performed
  //! without this participation
  MODEL_EXPORT virtual void synchronizeTransactions();

  /// Creates construction results
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultConstruction> createConstruction(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a body results
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultBody> createBody(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a part results
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultPart> createPart(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  //! Copies a part result, keeping the reference to origin
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultPart> copyPart(
    const std::shared_ptr<ModelAPI_ResultPart>& theOrigin,
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a group result
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultGroup> createGroup(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a field result
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultField> createField(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a parameter result
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_ResultParameter> createParameter(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);

  //! Returns a feature by result (owner of result)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature>
    feature(const std::shared_ptr<ModelAPI_Result>& theResult);

  //! Creates a folder (group of the features in the object browser)
  //! \param theAddBefore a feature, the folder is added before
  //!                     (if empty, the folder is added after the last feature)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Folder> addFolder(
      std::shared_ptr<ModelAPI_Feature> theAddBefore = std::shared_ptr<ModelAPI_Feature>());
  //! Removes the folder from the document (all features in the folder will be kept).
  MODEL_EXPORT virtual void removeFolder(std::shared_ptr<ModelAPI_Folder> theFolder);
  //! Search a folder above the list of features applicable to store them
  //! (it means the list of features stored in the folder should be consequential)
  //! \return Empty pointer if there is no applicable folder
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Folder> findFolderAbove(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures);
  //! Search a folder below the list of features applicable to store them
  //! (it means the list of features stored in the folder should be consequential)
  //! \return Empty pointer if there is no applicable folder
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Folder> findFolderBelow(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures);
  //! Search a folder containing the given feature.
  //! Additionally calculates a zero-based index of the feature in this folder.
  //! \param theFeature feature to search
  //! \param theIndexInFolder zero-based index in the folder or -1 if the feature is top-level.
  //! \return the folder containing the feature or empty pointer if the feature is top-level.
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Folder> findContainingFolder(
      const std::shared_ptr<ModelAPI_Feature>& theFeature,
      int& theIndexInFolder);
  //! Add a list of features to the folder. The correctness of the adding is not performed
  //! (such checks have been done in corresponding find.. method).
  //! \return \c true if the movement is successful
  MODEL_EXPORT virtual bool moveToFolder(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
      const std::shared_ptr<ModelAPI_Folder>& theFolder);
  //! Remove features from the folder
  //! \param theFeatures list of features to be removed
  //! \param theBefore   extract features before the folder (this parameter is applicable only
  //!                    when all features in the folder are taking out,
  //!                    in other cases the direction is taken automatically)
  //! \return \c true if the features have been moved out
  MODEL_EXPORT virtual bool removeFromFolder(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
      const bool theBefore = true);

  ///! Returns true if parametric updater need to execute feature on recomputation
  ///! On abort, undo or redo it is not necessary: results in document are updated automatically
  bool executeFeatures() {return myExecuteFeatures;}

  ///! On abort, undo or redo it is not necessary: results in document are updated automatically
  void setExecuteFeatures(const bool theFlag);

  //! Registers the name of the shape for the topological naming needs
  void addNamingName(const TDF_Label theLabel, std::wstring theName);
  //! Updates the name of some object
  void changeNamingName(std::wstring theOldName, const std::wstring theNewName,
    const TDF_Label& theLabel);
  //! Returns the label, keeper of the name  for the topological naming needs
  TDF_Label findNamingName(std::wstring theName, ResultPtr theContext);
  //! Returns the number of the name in the history relatively to the given object (by label).
  //! Start from 1 (this object).
  int numberOfNameInHistory(const ObjectPtr& theNameObject, const TDF_Label& theStartFrom);
  //! Returns the result by name of the result (names of results must be unique, used for naming
  //! selection by name.
  ResultPtr findByName(std::wstring& theName,
                       std::wstring& theSubShapeName,
                       bool& theUniqueContext);

  ///! Returns all features of the document including the hidden features which are not in
  ///! history. Not very fast method, for calling once, not in big cycles.
  MODEL_EXPORT virtual std::list<std::shared_ptr<ModelAPI_Feature> > allFeatures();

  //! Returns all objects of the document including the hidden features which are not in
  //! history. Not very fast method, for calling once, not in big cycles.
  MODEL_EXPORT virtual std::list<std::shared_ptr<ModelAPI_Object> > allObjects();

  /// Returns the global identifier of the current transaction (needed for the update algo)
  MODEL_EXPORT virtual int transactionID();
  /// Increases the transaction ID
  MODEL_EXPORT virtual void incrementTransactionID();

  /// Returns true if document is opened and valid
  MODEL_EXPORT virtual bool isOpened();

  /// Returns the last feature in the document (even not visible or disabled)
  /// \returns null if there is no features
  FeaturePtr lastFeature();

  /// Returns the feature that produced the given face of the given result.
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature> producedByFeature(
    std::shared_ptr<ModelAPI_Result> theResult,
    const std::shared_ptr<GeomAPI_Shape>& theShape);

  /// Returns true if theLater is in history of features creation later than theCurrent
  MODEL_EXPORT virtual bool isLater(FeaturePtr theLater, FeaturePtr theCurrent) const;

  /// Just removes all features without touching the document data (to be able undo)
  MODEL_EXPORT virtual void eraseAllFeatures();

  /// Returns the next (from the history point of view) feature, any: invisible or disabled
  /// \param theCurrent previous to the resulting feature
  /// \param theReverse if it is true, iterates in reversed order (next becomes previous)
  MODEL_EXPORT virtual std::shared_ptr<ModelAPI_Feature> nextFeature(
    std::shared_ptr<ModelAPI_Feature> theCurrent, const bool theReverse = false) const;

  //! Erases the document structure.
  ~Model_Document();

 protected:
  //! Returns (creates if needed) the general label
  TDF_Label generalLabel() const;

  //! Creates new document with binary file format
  Model_Document(const int theID, const std::string theKind);

  //! Returns the internal OCCT document of this interface
  Handle_TDocStd_Document document()
  {
    return myDoc;
  }

  //! performs compactification of all nested operations into one
  //! \returns true if resulting transaction is not empty and can be undone
  void compactNested();

  //! Returns all loaded sub documents
  const std::set<int> subDocuments() const;

  //! The implementation of undo: with or without recursive calls in the sub-documents
  void undoInternal(const bool theWithSubs, const bool theSynchronize);

  //! Stores the Id of the current operation (normally is called for the root document)
  void operationId(const std::string& theId);

  //! Returns the list of Ids of the operations that can be undone (called for the root document)
  std::list<std::string> undoList() const;

  //! Returns the list of Ids of the operations that can be redone (called for the root document)
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

  //! Returns the selection attribute that is used
  //! for calculation of selection externally from the document
  std::shared_ptr<ModelAPI_AttributeSelectionList> selectionInPartFeature();

  /// Stores in the document boolean flags: states of the nodes in the object browser.
  /// Normally is called outside of the transaction, just before "save".
  virtual void storeNodesState(const std::list<bool>& theStates);

  /// Returns the stored nodes states. Normally it is called just after "open".
  /// Appends the values to theStates list.
  virtual void restoreNodesState(std::list<bool>& theStates) const;

  /// Label that contains structures for selection of constructions of another document
  TDF_Label extConstructionsLabel() const;

  /// searches in this document feature that contains this label
  FeaturePtr featureByLab(const TDF_Label& theLab);
  /// searches in this document result that contains this label
  ResultPtr resultByLab(const TDF_Label& theLab);

  /// returns true if theThis is later in the features tree and dependencies than theOther
  bool isLaterByDep(FeaturePtr theThis, FeaturePtr theOther);

  /// appends the latest transaction to the previous one (used for AutoUpdate enabling transaction)
  void appendTransactionToPrevious();

  /// Sets the automatic recomputation flag: true means enabled
  void setAutoRecomutationState(const bool theState);
  /// Returns the current automatic recomputation flag: true means enabled
  bool autoRecomutationState() const;

  friend class Model_Application;
  friend class Model_Session;
  friend class Model_Update;
  friend class Model_AttributeReference;
  friend class Model_AttributeRefAttr;
  friend class Model_AttributeRefList;
  friend class Model_AttributeRefAttrList;
  friend class Model_AttributeSelection;
  friend class Model_AttributeSelectionList;
  friend class Model_ResultPart;
  friend class Model_ResultBody;
  friend class Model_ResultConstruction;
  friend class Model_SelectionNaming;
  friend class Model_BodyBuilder;
  friend class DFBrowser;

 private:
  int myID;  ///< identifier of the document in the application
  std::string myKind;  ///< kind of the document in the application
  Handle_TDocStd_Document myDoc;  ///< OCAF document

  Model_Objects *myObjs; ///< data manager of this document

  //! counter value of transaction on the last "save" call, used for "IsModified" method
  int myTransactionSave;
  //! number of nested transactions performed (list because may be nested inside of nested)
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
  //! The name -> list of labels where this name is appeared (the last created name is last here)
  std::map<std::wstring, std::list<TDF_Label> > myNamingNames;
  //! If it is true, features are not executed on update (on abort, undo, redo)
  bool myExecuteFeatures;

  bool myIsActive; ///< flag that stores the active/not active state

  //! The selection feature, if needed
  FeaturePtr mySelectionFeature;

  bool myIsSetCurrentFeature; ///< flag that my current feature is changed right now (recursion)
};

#endif
