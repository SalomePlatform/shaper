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

class ModelAPI_Feature;
class ModelAPI_Object;
class ModelAPI_Result;
class ModelAPI_ResultConstruction;
class ModelAPI_ResultBody;
class ModelAPI_ResultPart;
class ModelAPI_ResultGroup;
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

  //! Removes the feature from the document
  virtual void removeFeature(std::shared_ptr<ModelAPI_Feature> theFeature,
                             const bool theCheck = true) = 0;

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

  //! Returns the number of objects in the group of objects
  //! If theHidden is true, it counts also the features that are not in tree
  virtual int size(const std::string& theGroupID, const bool theHidden = false) = 0;

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

  //! Returns a feature by result (owner of result)
  virtual std::shared_ptr<ModelAPI_Feature> feature(
      const std::shared_ptr<ModelAPI_Result>& theResult) = 0;

protected:
  /// Only for SWIG wrapping it is here
  MODELAPI_EXPORT ModelAPI_Document();
};

//! Pointer on document object
typedef std::shared_ptr<ModelAPI_Document> DocumentPtr;

#endif
