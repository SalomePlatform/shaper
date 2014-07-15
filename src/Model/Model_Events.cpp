// File:        Model_Events.cxx
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#include <Model_Events.h>
#include <Events_Loop.h>

/// Alone instance of the creator per application
Model_EventCreator MY_CREATOR;

/////////////////////// CREATOR /////////////////////////////
void Model_EventCreator::sendUpdated(
  const ObjectPtr& theObject, const Events_ID& theEvent, const bool isGroupped) const
{
  Model_ObjectUpdatedMessage aMsg(theObject, theEvent);
  Events_Loop::loop()->send(aMsg, isGroupped);
}

void Model_EventCreator::sendDeleted(
  const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup) const
{
  Model_ObjectDeletedMessage aMsg(theDoc, theGroup);
  Events_Loop::loop()->send(aMsg, true);
}

Model_EventCreator::Model_EventCreator()
{
  ModelAPI_EventCreator::set(this);
}

/////////////////////// UPDATED MESSAGE /////////////////////////////
Model_ObjectUpdatedMessage::Model_ObjectUpdatedMessage(
  const ObjectPtr& theObject,
  const Events_ID& theEvent) : ModelAPI_ObjectUpdatedMessage(theEvent, 0)
{
  if (theObject) myObjects.insert(theObject);
}

std::set<ObjectPtr> Model_ObjectUpdatedMessage::features() const 
{
  return myObjects;
}

Events_MessageGroup* Model_ObjectUpdatedMessage::newEmpty() 
{
  ObjectPtr anEmptyObject;
  return new Model_ObjectUpdatedMessage(anEmptyObject, eventID());
}

void Model_ObjectUpdatedMessage::Join(Events_MessageGroup& theJoined) 
{
  Model_ObjectUpdatedMessage* aJoined = dynamic_cast<Model_ObjectUpdatedMessage*>(&theJoined);
  std::set<ObjectPtr >::iterator aFIter = aJoined->myObjects.begin();
  for(; aFIter != aJoined->myObjects.end(); aFIter++) {
    myObjects.insert(*aFIter);
  }
}

/////////////////////// DELETED MESSAGE /////////////////////////////
Model_ObjectDeletedMessage::Model_ObjectDeletedMessage(
  const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup)
  : ModelAPI_ObjectDeletedMessage(messageId(), 0), myDoc(theDoc)
{
  if (!theGroup.empty())
    myGroups.insert(theGroup);
}

Events_MessageGroup* Model_ObjectDeletedMessage::newEmpty() 
{
  return new Model_ObjectDeletedMessage(myDoc, "");
}

const Events_ID Model_ObjectDeletedMessage::messageId()
{
  static Events_ID MY_ID = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
  return MY_ID;
}

void Model_ObjectDeletedMessage::Join(Events_MessageGroup& theJoined)
{
  Model_ObjectDeletedMessage* aJoined = dynamic_cast<Model_ObjectDeletedMessage*>(&theJoined);
  std::set<std::string>::iterator aGIter = aJoined->myGroups.begin();
  for(; aGIter != aJoined->myGroups.end(); aGIter++) {
    myGroups.insert(*aGIter);
  }
}
