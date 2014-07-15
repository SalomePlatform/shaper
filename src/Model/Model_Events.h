// File:        Model_Events.h
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#ifndef Model_Events_HeaderFile
#define Model_Events_HeaderFile

#include <Model.h>
#include <ModelAPI_Events.h>

/// Allovs to create ModelAPI messages
class Model_EventCreator : public ModelAPI_EventCreator {
public:
  /// creates created, updated or moved messages and sends to the loop
  virtual void sendUpdated(
    const ObjectPtr& theObject, const Events_ID& theEvent, const bool isGroupped = true) const;
  /// creates deleted message and sends to the loop
  virtual void sendDeleted(
    const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup) const;

  /// must be one per application, the constructor for internal usage only
  Model_EventCreator();
};

/// Message that feature was changed (used for Object Browser update): moved, updated and deleted
class Model_ObjectUpdatedMessage : public ModelAPI_ObjectUpdatedMessage {
  std::set<ObjectPtr> myObjects; ///< which feature is changed

  /// Sender is not important, all information is located in the feature.
  /// Use ModelAPI for creation of this event. Used for creation, movement and edition events.
  Model_ObjectUpdatedMessage(
    const ObjectPtr& theObject,
    const Events_ID& theEvent);

  friend class Model_EventCreator;
public:

  /// Returns the feature that has been updated
  virtual std::set<ObjectPtr> features() const;

  //! Creates a new empty group (to store it in the loop before flush)
  virtual Events_MessageGroup* newEmpty();

  //! Allows to join the given message with the current one
  virtual void Join(Events_MessageGroup& theJoined);
};

/// Message that feature was deleted (used for Object Browser update)
class Model_ObjectDeletedMessage : public ModelAPI_ObjectDeletedMessage {
  boost::shared_ptr<ModelAPI_Document> myDoc; ///< document owner of the feature
  std::set<std::string> myGroups; ///< group identifiers that contained the deleted feature

  /// Use ModelAPI for creation of this event.
  Model_ObjectDeletedMessage(
    const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup);

  friend class Model_EventCreator;
public:
  /// Returns the feature that has been updated
  virtual boost::shared_ptr<ModelAPI_Document> document() const {return myDoc;}

  /// Returns the group where the feature was deleted
  virtual const std::set<std::string >& groups() const {return myGroups;}

  virtual Events_MessageGroup* newEmpty();


  virtual const Events_ID messageId();

  virtual void Join(Events_MessageGroup& theJoined);
};

#endif
