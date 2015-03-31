// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Events.h
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef MODELAPI_EVENTS_H_
#define MODELAPI_EVENTS_H_

#include <ModelAPI.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Feature.h>
#include <Events_MessageGroup.h>
#include <Events_Loop.h>

#include <memory>
#include <string>
#include <set>
#include <map>


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
/// Event ID that plugin is loaded (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_PLUGIN_LOADED = "PliginLoaded";
/// Event ID that data of feature has to be shown (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_TOSHOW = "ObjectShow";
/// Event ID that data of feature has to be shown (comes with ModelAPI_ObjectUpdatedMessage)
static const char * EVENT_OBJECT_TOHIDE = "ObjectHide";
//
static const char * EVENT_DOCUMENT_CHANGED = "CurrentDocumentChanged";

static const char * EVENT_FEATURE_STATE_REQUEST = "FeatureStateRequest";
static const char * EVENT_FEATURE_STATE_RESPONSE = "FeatureStateResponse";


static const char * EVENT_UPDATE_VIEWER_BLOCKED = "UpdateViewerBlocked";
static const char * EVENT_UPDATE_VIEWER_UNBLOCKED = "UpdateViewerUnblocked";

/// Message that feature was changed (used for Object Browser update): moved, updated and deleted
class MODELAPI_EXPORT ModelAPI_ObjectUpdatedMessage : public Events_MessageGroup
{
 protected:
  /// Creates an empty message
  ModelAPI_ObjectUpdatedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  virtual ~ModelAPI_ObjectUpdatedMessage();

 public:
  /// Returns the feature that has been updated
  virtual const std::set<ObjectPtr>& objects() const = 0;

  //! Creates a new empty group (to store it in the loop before flush)
  virtual std::shared_ptr<Events_MessageGroup> newEmpty() = 0;

  //! Allows to join the given message with the current one
  virtual void Join(const std::shared_ptr<Events_MessageGroup>& theJoined) = 0;
};

/// Message that feature was deleted (used for Object Browser update)
class MODELAPI_EXPORT ModelAPI_ObjectDeletedMessage : public Events_MessageGroup
{
protected:
  /// Creates an empty message
  ModelAPI_ObjectDeletedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  virtual ~ModelAPI_ObjectDeletedMessage();

public:
  /// Returns the feature that has been updated
  virtual std::shared_ptr<ModelAPI_Document> document() const = 0;

  /// Returns the group where the feature was deleted
  virtual const std::set<std::string>& groups() const = 0;

  /// Creates the new empty message of this kind
  virtual std::shared_ptr<Events_MessageGroup> newEmpty() = 0;

  /// Returns the identifier of the kind of a message
  virtual const Events_ID messageId() = 0;

  /// Appenad to this message the given one.
  virtual void Join(const std::shared_ptr<Events_MessageGroup>& theJoined) = 0;
};

/// Allows to create ModelAPI messages
class MODELAPI_EXPORT ModelAPI_EventCreator
{
public:
  /// creates created, updated or moved messages and sends to the loop
  virtual void sendUpdated(const ObjectPtr& theObject, const Events_ID& theEvent,
                           const bool isGroupped = true) const = 0;
  /// creates deleted message and sends to the loop
  virtual void sendDeleted(const std::shared_ptr<ModelAPI_Document>& theDoc,
                           const std::string& theGroup) const = 0;

  /// returns the creator instance
  static const ModelAPI_EventCreator* get();

  /// sets the creator instance
  static void set(const ModelAPI_EventCreator* theCreator);
};

// TODO(sbh): Move this message into a separate package, like "GuiAPI"
/// Contains the state information about the feature: is it enabled or disabled.
class ModelAPI_FeatureStateMessage : public Events_Message
{
public:
  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_FeatureStateMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_FeatureStateMessage();

  /// Returns the feature this message is related to
  MODELAPI_EXPORT std::shared_ptr<ModelAPI_Feature> feature() const;
  /// Stores the feature this message is related to
  MODELAPI_EXPORT void setFeature(std::shared_ptr<ModelAPI_Feature>& theFeature);
  // For response
  /// Returns true if feature has specific state
  MODELAPI_EXPORT bool hasState(const std::string& theFeatureId) const;
  /// Returns true if feature is enabled
  MODELAPI_EXPORT bool state(const  std::string& theFeatureId, bool theDefault = false) const;
  /// Stores the feature state
  MODELAPI_EXPORT void setState(const std::string& theFeatureId, bool theValue);
  /// Returns all feature IDs with states
  MODELAPI_EXPORT std::list<std::string> features() const;

 private:
  /// For Request
  std::shared_ptr<ModelAPI_Feature> myCurrentFeature;
  /// For response
  std::map<std::string, bool> myFeatureState;
};

/// Message that document (Part, PartSet) was created
class ModelAPI_DocumentCreatedMessage : public Events_Message
{
  DocumentPtr myDocument;

 public:
  /// Creates an empty message
  MODELAPI_EXPORT ModelAPI_DocumentCreatedMessage(const Events_ID theID, const void* theSender = 0);
  /// The virtual destructor
  MODELAPI_EXPORT virtual ~ModelAPI_DocumentCreatedMessage();
  /// Static. Returns EventID of the message.
  MODELAPI_EXPORT static Events_ID eventId()
  {
    static const char * MY_DOCUMENT_CREATED_EVENT_ID("DocumentCreated");
    return Events_Loop::eventByName(MY_DOCUMENT_CREATED_EVENT_ID);
  }

  /// Returns a document stored in the message
  MODELAPI_EXPORT DocumentPtr document() const;
  /// Sets a document to the message
  MODELAPI_EXPORT void setDocument(DocumentPtr theDocument);
};

#endif
