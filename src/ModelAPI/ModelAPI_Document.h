// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Document_H_
#define ModelAPI_Document_H_

#include <ModelAPI.h>
#include <string>
#include <memory>
#include <vector>
#include <list>
#include <set>

class ModelAPI_Feature;
class ModelAPI_Object;
class ModelAPI_Result;
class ModelAPI_ResultConstruction;
class ModelAPI_ResultBody;
class ModelAPI_ResultPart;
class ModelAPI_ResultGroup;
class ModelAPI_ResultParameter;
class ModelAPI_Data;

/**\class ModelAPI_Document
 * \ingroup DataModel
 * \brief Document for internal data structure of any object storage.
 * Document contains all data that must be stored/retrived in the file.
 * Also it provides acces to this data: open/save, transactions management etc.
 */
class ModelAPI_Document
{
public:
  //! Returns the kind of the document: "PartSet", "Part", or something else.
  //! This kind is used for feature buttons enable/disable depending on active document
  //! (it uses workbench "document" identifier in XML configuration file for this)
  virtual const std::string& kind() const = 0;

  //! Removes document data
  //! \param theForever if it is false, document is just hiden (to keep possibility make it back on Undo/Redo)
  virtual void close(const bool theForever = false) = 0;

  //! Adds to the document the new feature of the given feature id
  //! \param theID creates feature and puts it in the document (if it is not action)
  virtual std::shared_ptr<ModelAPI_Feature> addFeature(std::string theID) = 0;

  //! Return a list of features, which refers to the feature
  //! \param theFeature a feature
  //! \param theRefs a list of features
  //! \param isSendError a flag whether the error message should be send
  virtual void refsToFeature(std::shared_ptr<ModelAPI_Feature> theFeature,
                             std::set<std::shared_ptr<ModelAPI_Feature> >& theRefs,
                             const bool isSendError = true) = 0;

  //! Removes the feature from the document
  //! \param theFeature a feature to be removed
  virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature) = 0;

  ///! Adds a new sub-document by the identifier, or returns existing one if it is already exist
  virtual std::shared_ptr<ModelAPI_Document> subDocument(std::string theDocID) = 0;

  ///! Returns the id of the document
  virtual const std::string& id() const = 0;

  //! Returns the object in the group by the index (started from zero)
  //! \param theGroupID group that contains an object
  //! \param theIndex zero-based index of feature in the group
  //! \param theHidden if it is true, it counts also the features that are not in tree
  virtual std::shared_ptr<ModelAPI_Object> object(const std::string& theGroupID,
                                                    const int theIndex,
                                                    const bool theHidden = false) = 0;

  //! Returns the first found object in the group by the object name
  //! \param theGroupID group that contains an object
  //! \param theName name of the object to search
  //! \returns null if such object is not found
  virtual std::shared_ptr<ModelAPI_Object> objectByName(const std::string& theGroupID,
                                                    const std::string& theName) = 0;

  //! Returns the object index in the group. Object must be visible. Otherwise returns -1.
  //! \param theObject object of this document
  //! \returns index started from zero, or -1 if object is invisible or belongs to another document
  virtual const int index(std::shared_ptr<ModelAPI_Object> theObject) = 0;

  //! Returns the number of objects in the group of objects
  //! If theHidden is true, it counts also the features that are not in tree
  virtual int size(const std::string& theGroupID, const bool theHidden = false) = 0;

  //! Returns the feature that is currently edited in this document, normally
  //! this is the latest created feature
  //! \returns null if next created feature must be the first
  virtual std::shared_ptr<ModelAPI_Feature> currentFeature() = 0;

  //! Sets the current feature: all features below will be disabled, new features
  //! will be appended after this one.
  virtual void setCurrentFeature(std::shared_ptr<ModelAPI_Feature> theCurrent) = 0;

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Document();

  /// Creates a construction cresults
  virtual std::shared_ptr<ModelAPI_ResultConstruction> createConstruction(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  /// Creates a body results
  virtual std::shared_ptr<ModelAPI_ResultBody> createBody(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  /// Creates a part results
  virtual std::shared_ptr<ModelAPI_ResultPart> createPart(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;
  /// Creates a group results
  virtual std::shared_ptr<ModelAPI_ResultGroup> createGroup(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;

  virtual std::shared_ptr<ModelAPI_ResultParameter> createParameter(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex = 0) = 0;

  //! Returns a feature by result (owner of result)
  virtual std::shared_ptr<ModelAPI_Feature> feature(
      const std::shared_ptr<ModelAPI_Result>& theResult) = 0;

protected:
  /// Only for SWIG wrapping it is here
  MODELAPI_EXPORT ModelAPI_Document();

  /// Internally makes document know that feature was removed or added in history after creation
  MODELAPI_EXPORT virtual void addToHistory(const std::shared_ptr<ModelAPI_Object> theObject) = 0;

  friend class ModelAPI_Object; // to add or remove from the history
};

//! Pointer on document object
typedef std::shared_ptr<ModelAPI_Document> DocumentPtr;

#endif
