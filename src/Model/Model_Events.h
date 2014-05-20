// File:        Model_Events.h
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Events_HeaderFile
#define Model_Events_HeaderFile

#include <Model.h>
#include <Events_Message.h>
#include <Events_Loop.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <list>

class ModelAPI_Feature;
class ModelAPI_Document;

/// Event ID that feature is created (comes with Model_FeatureUpdatedMessage)
static const char * EVENT_FEATURE_CREATED = "FeatureCreated";
/// Event ID that data of feature is updated (comes with Model_FeatureUpdatedMessage)
static const char * EVENT_FEATURE_UPDATED = "FeatureUpdated";
/// Event ID that data of feature is deleted (comes with Model_FeatureDeletedMessage)
static const char * EVENT_FEATURE_DELETED = "FeatureDeleted";
/// Event ID that data of feature is updated (comes with Model_FeaturesMovedMessage)
static const char * EVENT_FEATURES_MOVED = "FeaturesMoved";

/// Message that feature was changed (used for Object Browser update)
class Model_FeatureUpdatedMessage : public Events_Message {
  boost::shared_ptr<ModelAPI_Feature> myFeature; ///< which feature is changed
public:
  /// sender is not important, all information is located in the feature
  Model_FeatureUpdatedMessage(
    const boost::shared_ptr<ModelAPI_Feature>& theFeature,
    const Events_ID& theEvent) : Events_Message(theEvent, 0), myFeature(theFeature)
  {}

  /// Returns the feature that has been updated
  boost::shared_ptr<ModelAPI_Feature> feature() const {return myFeature;}
};

/// Message that feature was deleted (used for Object Browser update)
class Model_FeatureDeletedMessage : public Events_Message {
  boost::shared_ptr<ModelAPI_Document> myDoc; ///< document owner of the feature
  std::string myGroup; ///< group identifier that contained the deleted feature
public:
  /// creates a message by initialization of fields
  Model_FeatureDeletedMessage(const boost::shared_ptr<ModelAPI_Document>& theDoc,
    const std::string& theGroup);

  /// Returns the ID of this message (EVENT_FEATURE_DELETED)
  static const Events_ID messageId();

  /// Returns the feature that has been updated
  boost::shared_ptr<ModelAPI_Document> document() const {return myDoc;}

  /// Returns the group where the feature was deleted
  const std::string& group() const {return myGroup;}
};

/// Message that features were moved (used for the feature preview update)
class Model_FeaturesMovedMessage : public Events_Message {
  std::list<boost::shared_ptr<ModelAPI_Feature> > myFeatures; ///< which features are moved
public:
  /// creates a message by initialization of fields
  MODEL_EXPORT Model_FeaturesMovedMessage();

  /// Returns the ID of this message (EVENT_FEATURES_MOVED)
  static const Events_ID messageId();

  /// Sets a list of features
  MODEL_EXPORT void setFeatures(const std::list<boost::shared_ptr<ModelAPI_Feature> >& theFeatures);

  /// Returns a list of features
  MODEL_EXPORT const std::list<boost::shared_ptr<ModelAPI_Feature> >& features() const;
};

#endif
