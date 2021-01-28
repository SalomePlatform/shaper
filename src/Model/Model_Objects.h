// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#ifndef Model_Objects_H_
#define Model_Objects_H_

#include <Model.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Folder.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>

#include <NCollection_DataMap.hxx>
#include <TDF_Label.hxx>
#include <TDF_LabelList.hxx>
#include <map>
#include <set>
#include <vector>

extern int kUNDEFINED_FEATURE_INDEX;

/**\class Model_Objects
 * \ingroup DataModel
 * \brief Manager of objects of the document. Normally one this class corresponds to
 * one document and just helper to manage objects (ModelAPI_Objects) inside of the document
 * on the level of data storage.
 */
class Model_Objects
{
 public:
  //! Registers the feature in the data structure
  //! \param theFeature feature that must be added to the data structure
  //! \param theAfterThis the feature will be added after this feature;
  //!        if it is null, the added feature will be the first
  void addFeature(FeaturePtr theFeature, const FeaturePtr theAfterThis);

  //! Return a list of features, which refers to the feature
  //! \param theFeature a feature
  //! \param theRefs a list of reference features
  //! \param isSendError a flag whether the error message should be send
  void refsToFeature(FeaturePtr theFeature,
                     std::set<FeaturePtr>& theRefs,
                     const bool isSendError = true);

  //! Removes the feature from the document (with result)
  //! \param theFeature a removed feature
  void removeFeature(FeaturePtr theFeature);

  //! Moves the feature to make it after the given one in the history.
  void moveFeature(FeaturePtr theMoved, FeaturePtr theAfterThis);

  //! Returns the existing feature by the label
  //! \param theLabel base label of the feature
  FeaturePtr feature(TDF_Label theLabel) const;

  //! Returns the existing object: result or feature
  //! \param theLabel base label of the object
  ObjectPtr object(TDF_Label theLabel);

  //! Returns the first found object in the group by the object name
  //! \param theGroupID group that contains an object
  //! \param theName name of the object to search
  //! \returns null if such object is not found
  std::shared_ptr<ModelAPI_Object> objectByName(
    const std::string& theGroupID, const std::wstring& theName);

  //! Returns the result by the result name
  ResultPtr findByName(const std::wstring theName);


  //! Returns the object index in the group. Object must be visible. Otherwise returns -1.
  //! \param theObject object of this document
  //! \param theAllowFolder take into account grouping feature by folders
  //! \returns index started from zero, or -1 if object is invisible or belongs to another document
  const int index(std::shared_ptr<ModelAPI_Object> theObject,
                  const bool theAllowFolder = false);

  //! Returns the feature in the group by the index (started from zero)
  //! \param theGroupID group that contains a feature
  //! \param theIndex zero-based index of feature in the group
  //! \param theAllowFolder take into account grouping feature by folders
  ObjectPtr object(const std::string& theGroupID,
                   const int theIndex,
                   const bool theAllowFolder = false);

  //! Returns the number of features in the group
  //! \param theGroupID group of objects
  //! \param theAllowFolder take into account grouping feature by folders
  int size(const std::string& theGroupID, const bool theAllowFolder = false);

  //! Returns the parent object of this child. This may be result or feature, parent of a
  //! top result. Fast method, that uses internal data structure specifics.
  std::shared_ptr<ModelAPI_Object> parent(const std::shared_ptr<ModelAPI_Object> theChild);

  //! Returns all (and disabled) results of the given type.
  //! Not fast method (iterates all features).
  void allResults(const std::string& theGroupID, std::list<ResultPtr>& theResults);

  //! Returns the number of all features: in the history or not
  int numInternalFeatures();
  //! Returns the feature by zero-based index: features in the history or not
  std::shared_ptr<ModelAPI_Feature> internalFeature(const int theIndex);

  /// Creates a construction result
  std::shared_ptr<ModelAPI_ResultConstruction> createConstruction(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a body result
  std::shared_ptr<ModelAPI_ResultBody> createBody(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0,
      const std::wstring& theNameShape = L"");
  /// Creates a part result
  std::shared_ptr<ModelAPI_ResultPart> createPart(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Copies a part result, keeping the reference to origin
  std::shared_ptr<ModelAPI_ResultPart> copyPart(
      const std::shared_ptr<ModelAPI_ResultPart>& theOrigin,
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a group result
  std::shared_ptr<ModelAPI_ResultGroup> createGroup(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a field result
  std::shared_ptr<ModelAPI_ResultField> createField(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a parameter result
  std::shared_ptr<ModelAPI_ResultParameter> createParameter(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);

  //! Returns a feature by result (owner of result)
  std::shared_ptr<ModelAPI_Feature>
    feature(const std::shared_ptr<ModelAPI_Result>& theResult);

  /// Creates a folder (group of the features in the object browser)
  std::shared_ptr<ModelAPI_Folder> createFolder(
      const std::shared_ptr<ModelAPI_Feature>& theBeforeThis);
  //! Removes the folder from the document (all features in the folder will be kept).
  void removeFolder(std::shared_ptr<ModelAPI_Folder> theFolder);
  //! Search a folder applicable for the list of features
  //! (it means the list of features stored in the folder should be consequential)
  //! \param theFeatures list of features to be added to folder
  //! \param theBelow    search the folder below the features (if \c false, search above)
  //! \return Empty pointer if there is no applicable folder
  std::shared_ptr<ModelAPI_Folder> findFolder(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
      const bool theBelow);
  //! Search a folder containing the given feature.
  //! Additionally calculates a zero-based index of the feature in this folder.
  //! \param theFeature feature to search
  //! \param theIndexInFolder zero-based index in the folder or -1 if the feature is top-level.
  //! \return the folder containing the feature or empty pointer if the feature is top-level.
  std::shared_ptr<ModelAPI_Folder> findContainingFolder(
      const std::shared_ptr<ModelAPI_Feature>& theFeature,
      int& theIndexInFolder);
  //! Add a list of features to the folder. The correctness of the adding is not performed
  //! (such checks have been done in corresponding find.. method).
  //! \return \c true if the movement is successful
  bool moveToFolder(const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
                    const std::shared_ptr<ModelAPI_Folder>& theFolder);
  //! Remove features from the folder
  //! \param theFeatures list of features to be removed
  //! \param theBefore   extract features before the folder (this parameter is applicable only
  //!                    when all features in the folder are taking out,
  //!                    in other cases the direction is taken automatically)
  //! \return \c true if the features have been moved out
  bool removeFromFolder(const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
                        const bool theBefore = true);

  //! Sets the owner of this manager
  void setOwner(DocumentPtr theDoc);

  //! Returns the owner of this manager
  DocumentPtr owner() {return myDoc;}

  //! Deletes all managed features with emitting of corresponded signal
  virtual ~Model_Objects();

 protected:

  //! Returns (creates if needed) the features label
  TDF_Label featuresLabel() const;

  //! Initializes feature with a unique name in this group (unique name is generated as
  //! feature type + "_" + index
  void setUniqueName(FeaturePtr theFeature);

  //! Initializes the folder with an unique name ("Folder_" + index)
  void setUniqueName(FolderPtr theFolder);

  //! Synchronizes myFeatures list with the updated document
  //! \param theUpdated list of labels that are marked as modified, so features must be also
  //! \param theUpdateReferences causes the update of back-references
  //! \param theExecuteFeatures requires re-execute modified persistent features
  //!            (not needed on undo/redo/abort/open)
  //! \param theOpen - on open nothing must be re-executed, except not persistent results
  //! \param theFlush makes flush all events in the end of all modifications of this method
  void synchronizeFeatures(const TDF_LabelList& theUpdated, const bool theUpdateReferences,
    const bool theOpen, const bool theExecuteFeatures, const bool theFlush);
  //! Synchronizes the BackReferences list in Data of Features and Results
  void synchronizeBackRefs();

  //! Creates manager on the OCAF document main label
  Model_Objects(TDF_Label theMainLab);

  //! Initializes the data fields of the feature
  void initData(ObjectPtr theObj, TDF_Label theLab, const int theTag);

  //! Allows to store the result in the data tree of the document
  //! (attaches 'data' of result to tree)
  void storeResult(std::shared_ptr<ModelAPI_Data> theFeatureData,
                   std::shared_ptr<ModelAPI_Result> theResult,
                   const int theResultIndex = 0,
                   const std::wstring& theNameShape = L"");

  //! returns the label of result by index; creates this label if it was not created before
  TDF_Label resultLabel(const std::shared_ptr<ModelAPI_Data>& theFeatureData,
                        const int theResultIndex);

  //! Updates the results list of the feature basing on the current data tree
  //! theProcessed is used to avoid update twice (since the function is recursive)
  void updateResults(FeaturePtr theFeature, std::set<FeaturePtr>& theProcessed);

  /// Internally makes document know that feature was removed or added in history after creation
  void updateHistory(const std::shared_ptr<ModelAPI_Object> theObject);

  /// Internally makes document know that feature was removed or added in history after creation
  void updateHistory(const std::string theGroup);

  /// Clears the history arrays related to this object
  void clearHistory(ObjectPtr theObj);

  /// Creates the history: up to date with the current state
  void createHistory(const std::string& theGroupID);

  /// Returns the next (from the history point of view) feature, any: invisible or disabled
  /// \param theCurrent previous to the resulting feature
  /// \param theReverse if it is true, iterates in reversed order (next becomes previous)
  /// \param theIndex may be used for optimization: index of theCurrent in references array
  FeaturePtr nextFeature(FeaturePtr theCurrent, int& theIndex, const bool theReverse = false);
  /// Returns to the first (from the history point of view) feature, any: invisible or disabled
  FeaturePtr firstFeature();
  /// Returns to the last (from the history point of view) feature, any: invisible or disabled
  FeaturePtr lastFeature();
  /// Returns true if theLater is in history of features creation later than theCurrent
  bool isLater(FeaturePtr theLater, FeaturePtr theCurrent) const;

  /// Returns the next or previous label
  /// \param theCurrent given label
  /// \param theReverse if it is true, iterates in reversed order (next becomes previous)
  TDF_Label nextLabel(TDF_Label theCurrent, int& theIndex, const bool theReverse = false);

  /// Returns the result group identifier of the given feature (for this at least one result must
  /// be created before)
  std::string featureResultGroup(FeaturePtr theFeature);

  //! Returns all features of the document including the hidden features which are not in
  //! history. Not very fast method, for calling once, not in big cycles.
  std::list<std::shared_ptr<ModelAPI_Feature> > allFeatures();

  //! Returns all objects of the document including the hidden features which are not in
  //! history. Not very fast method, for calling once, not in big cycles.
  std::list<std::shared_ptr<ModelAPI_Object> > allObjects();

  //! synchronizes back references for the given object basing on the collected data
  void synchronizeBackRefsForObject(
    const std::set<std::shared_ptr<ModelAPI_Attribute>>& theNewRefs, ObjectPtr theObject);

  /// Just removes all features without touching the document data (to be able undo)
  virtual void eraseAllFeatures();

  // Check whether the predefined-image of the result had user-defined name.
  // If yes, return this name.
  bool hasCustomName(DataPtr theFeatureData,
                     ResultPtr theResult,
                     int theResultIndex,
                     std::wstring& theParentName) const;

  /// Return object representing a folder or empty pointer
  const ObjectPtr& folder(TDF_Label theLabel) const;

 private:
  TDF_Label myMain; ///< main label of the data storage

  DocumentPtr myDoc; ///< document, owner of this objects manager: needed for events creation

  /// All managed features (not only in history of OB)
  /// For optimization mapped by labels
  NCollection_DataMap<TDF_Label, FeaturePtr> myFeatures;

  /// Managed folders
  NCollection_DataMap<TDF_Label, ObjectPtr> myFolders;

  /// Map from group id to the array that contains all objects located in history.
  /// Each array is updated by demand from scratch, by browsing all the features in the history.
  std::map<std::string, std::vector<ObjectPtr> > myHistory;

  friend class Model_Document;
  friend class Model_Session;
  friend class Model_Update;
  friend class Model_AttributeReference;
  friend class Model_AttributeRefAttr;
  friend class Model_AttributeRefList;
  friend class Model_AttributeSelection;
  friend class Model_SelectionNaming;
};

#endif
