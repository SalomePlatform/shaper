// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Objects.h
// Created:     15 May 2015
// Author:      Mikhail PONIKAROV

#ifndef Model_Objects_H_
#define Model_Objects_H_

#include <Model.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultParameter.h>

#include <TDocStd_Document.hxx>
#include <NCollection_DataMap.hxx>
#include <TDF_Label.hxx>
#include <map>
#include <set>
#include <vector>

// for TDF_Label map usage
static Standard_Integer HashCode(const TDF_Label& theLab, const Standard_Integer theUpper);
static Standard_Boolean IsEqual(const TDF_Label& theLab1, const TDF_Label& theLab2);

/**\class Model_Objects
 * \ingroup DataModel
 * \brief Manager of objects of the document. Normally one this class corresponds to
 * one document and just helper to manage objects (ModelAPI_Objects) inside of the document
 * on the level of data storage.
 */
class Model_Objects
{
 public:
  //! Registers the fieature in the data structure
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
    const std::string& theGroupID, const std::string& theName);

  //! Returns the result by the result name
  ResultPtr findByName(const std::string theName);


  //! Returns the object index in the group. Object must be visible. Otherwise returns -1.
  //! \param theObject object of this document
  //! \returns index started from zero, or -1 if object is invisible or belongs to another document
  const int index(std::shared_ptr<ModelAPI_Object> theObject);

  //! Returns the feature in the group by the index (started from zero)
  //! \param theGroupID group that contains a feature
  //! \param theIndex zero-based index of feature in the group
  ObjectPtr object(const std::string& theGroupID, const int theIndex);

  //! Returns the number of features in the group
  int size(const std::string& theGroupID);

  ///! Returns all (and disabled) results of the given type. Not fast method (iterates all features).
  void allResults(const std::string& theGroupID, std::list<ResultPtr>& theResults);

  /// Creates a construction cresults
  std::shared_ptr<ModelAPI_ResultConstruction> createConstruction(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a body results
  std::shared_ptr<ModelAPI_ResultBody> createBody(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a part results
  std::shared_ptr<ModelAPI_ResultPart> createPart(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);
  /// Creates a group results
  std::shared_ptr<ModelAPI_ResultGroup> createGroup(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);

  std::shared_ptr<ModelAPI_ResultParameter> createParameter(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0);

  //! Returns a feature by result (owner of result)
  std::shared_ptr<ModelAPI_Feature>
    feature(const std::shared_ptr<ModelAPI_Result>& theResult);

  //! Sets the owner of this manager
  void setOwner(DocumentPtr theDoc);

  //! Returns the owner of this manager
  DocumentPtr owner() {return myDoc;}

  //! Deletes all managed features wit hemmitting of corresponded signal
  ~Model_Objects();

 protected:

  //! Returns (creates if needed) the features label
  TDF_Label featuresLabel() const;

  //! Initializes feature with a unique name in this group (unique name is generated as 
  //! feature type + "_" + index
  void setUniqueName(FeaturePtr theFeature);

  //! Synchronizes myFeatures list with the updated document
  //! \param theMarkUpdated causes the "update" event for all features
  //! \param theUpdateReferences causes the update of back-references
  //! \param theFlush makes flush all events in the end of all modifications of this method
  void synchronizeFeatures(const bool theMarkUpdated, const bool theUpdateReferences,
    const bool theFlush);
  //! Synchronizes the BackReferences list in Data of Features and Results
  void synchronizeBackRefs();

  //! Creates manager on the OCAF document main label
  Model_Objects(TDF_Label theMainLab);

  //! Initializes the data fields of the feature
  void initData(ObjectPtr theObj, TDF_Label theLab, const int theTag);

  //! Allows to store the result in the data tree of the document (attaches 'data' of result to tree)
  void storeResult(std::shared_ptr<ModelAPI_Data> theFeatureData,
                   std::shared_ptr<ModelAPI_Result> theResult,
                   const int theResultIndex = 0);

  //! returns the label of result by index; creates this label if it was not created before
  TDF_Label resultLabel(const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theResultIndex);

  //! Updates the results list of the feature basing on the current data tree
  void updateResults(FeaturePtr theFeature);

  /// Internally makes document know that feature was removed or added in history after creation
  void updateHistory(const std::shared_ptr<ModelAPI_Object> theObject);

  /// Internally makes document know that feature was removed or added in history after creation
  void updateHistory(const std::string theGroup);

  /// Clears the history arrays related to this object
  void clearHistory(ObjectPtr theObj);

  /// Creates the history: up to date with the current state
  void createHistory(const std::string& theGroupID);

  /// Returns to the next (from the history point of view) feature, any: invisible or disabled
  /// \param theReverse if it is true, iterates in reverced order (next becomes previous)
  FeaturePtr nextFeature(FeaturePtr theCurrent, const bool theReverse = false);
  /// Returns to the first (from the history point of view) feature, any: invisible or disabled
  FeaturePtr firstFeature();
  /// Returns to the last (from the history point of view) feature, any: invisible or disabled
  FeaturePtr lastFeature();

 private:
  TDF_Label myMain; ///< main label of the data storage

  DocumentPtr myDoc; ///< doc,ument, owner of this objects manager: needed for events creation

  /// All managed features (not only in history of OB)
  /// For optimization mapped by labels
  NCollection_DataMap<TDF_Label, FeaturePtr> myFeatures;

  /// Map from group id to the array that contains all objects located in history.
  /// Each array is updated by demand from scratch, by browing all the features in the history.
  std::map<std::string, std::vector<ObjectPtr> > myHistory;

  friend class Model_Document;
  friend class Model_Session;
  friend class Model_Update;
  friend class Model_AttributeReference;
  friend class Model_AttributeRefAttr;
  friend class Model_AttributeRefList;
};

#endif
