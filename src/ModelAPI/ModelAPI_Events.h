// File:        ModelAPI_Events.h
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef ModelAPI_Events_HeaderFile
#define ModelAPI_Events_HeaderFile

#include <ModelAPI.h>
#include <Events_MessageGroup.h>
#include <boost/shared_ptr.hpp>
#include <string>
#include <set>

#include "ModelAPI_Object.h"

class ModelAPI_Document;

/// Event ID that feature is created (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_CREATED = "ObjectCreated";
/// Event ID that data of feature is updated (comes with Model_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_UPDATED = "ObjectUpdated";
/// Event ID that data of feature is deleted (comes with Model_ObjectDeletedMessage)
static const char * EVENT_OBJECT_DELETED = "ObjectDeleted";
/// Event ID that data of feature is updated (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_MOVED = "ObjectsMoved";
/// Event ID that visualization must be redisplayed (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_TO_REDISPLAY = "ObjectsToRedisplay";
/// Event ID that visualization must be redisplayed (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OPERATION_LAUNCHED = "OperationLaunched";

/// Message that feature was changed (used for Object Browser update): moved, updated and deleted
class ModelAPI_ObjectUpdatedMessage : public Events_MessageGroup {
protected:
  ModelAPI_ObjectUpdatedMessage(const Events_ID theID, const void* theSender = 0)
    : Events_MessageGroup(theID, theSender) {}

public:
  /// Returns the feature that has been updated
  virtual std::set<ObjectPtr> objects() const = 0;

  //! Creates a new empty group (to store it in the loop before flush)
  virtual Events_MessageGroup* newEmpty() = 0;

  //! Allows to join the given message with the current one
  virtual void Join(Events_MessageGroup& theJoined) = 0;
};

/// Message that feature was deleted (used for Object Browser update)
class ModelAPI_ObjectDeletedMessage : public Events_MessageGroup {
protected:
  ModelAPI_ObjectDeletedMessage(const Events_ID theID, const void* theSender = 0)
    : Events_MessageGroup(theID, theSender) {}

public:
  /// Returns the feature that has been updated
  virtual boost::shared_ptr<ModelAPI_Document> document() const = 0;

  /// Returns the group where the feature was deleted
  virtual const std::set<std::string >& groups() const = 0;

  virtual Events_MessageGroup* newEmpty() = 0;

  virtual const Events_ID messageId() = 0;

  virtual void Join(Events_MessageGroup& theJoined) = 0;
};

/// Allows to create ModelAPI messages
class MODELAPI_EXPORT ModelAPI_EventCreator {
public:
  /// creates created, updated or moved messages and sends to the loop
  virtual void sendUpdated(const ObjectPtr& theObject, const Events_ID& theEvent,
                           const bool isGroupped = true) const = 0;
  /// creates deleted message and sends to the loop
  virtual void sendDeleted(
    const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup) const = 0;

  /// returns the creator instance
  static const ModelAPI_EventCreator* get();
  
  /// sets the creator instance
  static void set(const ModelAPI_EventCreator* theCreator);
};

#endif
