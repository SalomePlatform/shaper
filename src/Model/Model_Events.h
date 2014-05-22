// File:        Model_Events.h
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Events_HeaderFile
#define Model_Events_HeaderFile

#include <Model.h>
#include <Events_MessageGroup.h>
#include <Events_Loop.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <set>

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

/// Message that feature was changed (used for Object Browser update): moved, updated and deleted
class Model_FeatureUpdatedMessage : public Events_MessageGroup {
  std::set<boost::shared_ptr<ModelAPI_Feature> > myFeatures; ///< which feature is changed
public:
  /// sender is not important, all information is located in the feature
  Model_FeatureUpdatedMessage(
    const boost::shared_ptr<ModelAPI_Feature>& theFeature,
    const Events_ID& theEvent) : Events_MessageGroup(theEvent, 0)
  {if (theFeature) myFeatures.insert(theFeature);}

  /// Returns the feature that has been updated
  std::set<boost::shared_ptr<ModelAPI_Feature> > features() const {return myFeatures;}

  //! Creates a new empty group (to store it in the loop before flush)
  virtual Events_MessageGroup* newEmpty();
  //! Allows to join the given message with the current one
  virtual void Join(Events_MessageGroup& theJoined);
};

/// Message that feature was deleted (used for Object Browser update)
class Model_FeatureDeletedMessage : public Events_MessageGroup {
  boost::shared_ptr<ModelAPI_Document> myDoc; ///< document owner of the feature
  std::set<std::string> myGroups; ///< group identifiers that contained the deleted feature
public:
  /// creates a message by initialization of fields
  Model_FeatureDeletedMessage(const boost::shared_ptr<ModelAPI_Document>& theDoc,
    const std::string& theGroup);

  /// Returns the ID of this message (EVENT_FEATURE_DELETED)
  static const Events_ID messageId();

  /// Returns the feature that has been updated
  boost::shared_ptr<ModelAPI_Document> document() const {return myDoc;}

  /// Returns the group where the feature was deleted
  const std::set<std::string >& groups() const {return myGroups;}

  //! Creates a new empty group (to store it in the loop before flush)
  virtual Events_MessageGroup* newEmpty();

  //! Allows to join the given message with the current one
  virtual void Join(Events_MessageGroup& theJoined);
};

#endif
