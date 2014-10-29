// File:        ModelAPI_Feature.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Feature_H_
#define ModelAPI_Feature_H_

#include <ModelAPI.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Result.h>

#include <boost/shared_ptr.hpp>

#include <list>
#include <string>

class ModelAPI_Data;
class ModelAPI_Document;
class ModelAPI_Result;

/**\class ModelAPI_Feature
 * \ingroup DataModel
 * \brief Feature function that represents the particular functionality
 * of this operation. Produces results by the arguments.
 */
class ModelAPI_Feature : public ModelAPI_Object
{
  ///< list of current results of this feature
  std::list<boost::shared_ptr<ModelAPI_Result> > myResults;
 public:
  /// Returns the unique kind of a feature (like "Point")
  virtual const std::string& getKind() = 0;

  /// Returns the group identifier of all features
  static std::string group()
  {
    static std::string MY_GROUP = "Features";
    return MY_GROUP;
  }

  /// Returns document this feature belongs to
  virtual boost::shared_ptr<ModelAPI_Document> document() const
  {
    return ModelAPI_Object::document();
  }

  /// Returns the group identifier of this result
  virtual std::string groupName()
  {
    return group();
  }

  /// Request for initialization of data model of the feature: adding all attributes
  virtual void initAttributes() = 0;

  /// Computes or recomputes the results
  virtual void execute() = 0;

  /// returns the current results of the feature
  MODELAPI_EXPORT const std::list<boost::shared_ptr<ModelAPI_Result> >& results();
  /// returns the first result in the list or NULL reference
  MODELAPI_EXPORT boost::shared_ptr<ModelAPI_Result> firstResult();
  /// sets the alone result
  MODELAPI_EXPORT void setResult(const boost::shared_ptr<ModelAPI_Result>& theResult);
  /// sets the result by index (zero based), results before this must be set before
  MODELAPI_EXPORT void setResult(const boost::shared_ptr<ModelAPI_Result>& theResult,
                                 const int theIndex);
  /// removes the result from the feature
  MODELAPI_EXPORT void removeResult(const boost::shared_ptr<ModelAPI_Result>& theResult);
  /// removes all results from the feature
  MODELAPI_EXPORT void eraseResults();
  /// removes all fields from this feature: results, data, etc
  MODELAPI_EXPORT virtual void erase();

  /// Returns true if result is persistent (stored in document) and on undo-redo, save-open
  /// it is not needed to recompute it.
  virtual bool isPersistentResult() {return true;}

  /// Returns true if this feature must not be created: this is just an action
  /// that is not stored in the features history and data model (like "delete part").
  virtual bool isAction()
  {
    return false;
  }

  /// Must return document where the new feature must be added to
  /// By default it is current document
  MODELAPI_EXPORT virtual boost::shared_ptr<ModelAPI_Document> documentToAdd();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Feature();

  MODELAPI_EXPORT static boost::shared_ptr<ModelAPI_Feature> feature(ObjectPtr theObject);

 //
 // Helper methods, aliases for data()->method()
 // -----------------------------------------------------------------------------------------------
  inline std::string name()
  {
    return data()->name();
  }

  inline boost::shared_ptr<ModelAPI_AttributeBoolean> boolean(const std::string& theID)
  {
    return data()->boolean(theID);
  }

  inline boost::shared_ptr<ModelAPI_AttributeDocRef> document(const std::string& theID)
  {
    return data()->document(theID);
  }

  inline boost::shared_ptr<ModelAPI_AttributeDouble> real(const std::string& theID)
  {
    return data()->real(theID);
  }

  inline boost::shared_ptr<ModelAPI_AttributeInteger> integer(const std::string& theID)
  {
    return data()->integer(theID);
  }

  inline boost::shared_ptr<ModelAPI_AttributeRefAttr> refattr(const std::string& theID)
  {
    return data()->refattr(theID);
  }

  inline boost::shared_ptr<ModelAPI_AttributeReference> reference(const std::string& theID)
  {
    return data()->reference(theID);
  }

  inline boost::shared_ptr<ModelAPI_AttributeRefList> reflist(const std::string& theID)
  {
    return data()->reflist(theID);
  }

  inline boost::shared_ptr<ModelAPI_AttributeSelection> selection(const std::string& theID)
  {
    return data()->selection(theID);
  }

  inline boost::shared_ptr<ModelAPI_AttributeSelectionList> selectionList(const std::string& theID)
  {
    return data()->selectionList(theID);
  }

  inline boost::shared_ptr<ModelAPI_AttributeString> string(const std::string& theID)
  {
    return data()->string(theID);
  }

  inline boost::shared_ptr<ModelAPI_Attribute> attribute(const std::string& theID)
  {
    return data()->attribute(theID);
  }
 // -----------------------------------------------------------------------------------------------
};

//! Pointer on feature object
typedef boost::shared_ptr<ModelAPI_Feature> FeaturePtr;

#endif

