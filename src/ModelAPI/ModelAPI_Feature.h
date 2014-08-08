// File:        ModelAPI_Feature.hxx
// Created:     21 Mar 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Feature_H_
#define ModelAPI_Feature_H_

#include "ModelAPI_Object.h"
#include "ModelAPI_PluginManager.h"

#include <string>
#include <list>
#include <boost/shared_ptr.hpp>

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
    {static std::string MY_GROUP = "Features"; return MY_GROUP;}

  /// Returns the group identifier of this result
  virtual std::string groupName() { return group(); }

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
  MODELAPI_EXPORT void setResult(
    const boost::shared_ptr<ModelAPI_Result>& theResult, const int theIndex);

  /// Returns true if this feature must not be created: this is just an action
  /// that is not stored in the features history and data model (like "delete part").
  virtual bool isAction() {return false;}

  /// Must return document where the new feature must be added to
  /// By default it is current document
  MODELAPI_EXPORT virtual boost::shared_ptr<ModelAPI_Document> documentToAdd();

  /// To virtually destroy the fields of successors
  MODELAPI_EXPORT virtual ~ModelAPI_Feature();

  MODELAPI_EXPORT static boost::shared_ptr<ModelAPI_Feature> feature(ObjectPtr theObject);

};

//! Pointer on feature object
typedef boost::shared_ptr<ModelAPI_Feature> FeaturePtr;

#endif
