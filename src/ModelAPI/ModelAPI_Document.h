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
  //! Returns the kind of the document: "PartSet", "Part", or something else.
  //! This kind is used for feature buttons enable/disable depending on active document
  //! (it uses workbench "document" identifier in XML configuration file for this)
  virtual const std::string& kind() const = 0;

  //! Removes document data
  virtual void close() = 0;

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
