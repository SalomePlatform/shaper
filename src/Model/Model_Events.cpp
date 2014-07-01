// File:        Model_Events.cxx
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#include <Model_Events.h>
#include <Events_Loop.h>

/// Alone instance of the creator per application
Model_EventCreator MY_CREATOR;

/////////////////////// CREATOR /////////////////////////////
void Model_EventCreator::sendUpdated(
  const FeaturePtr& theFeature, const Events_ID& theEvent, const bool isGroupped) const
{
  Model_FeatureUpdatedMessage aMsg(theFeature, theEvent);
  Events_Loop::loop()->send(aMsg, isGroupped);
}

void Model_EventCreator::sendDeleted(
  const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup) const
{
  Model_FeatureDeletedMessage aMsg(theDoc, theGroup);
  Events_Loop::loop()->send(aMsg, true);
}

Model_EventCreator::Model_EventCreator()
{
  ModelAPI_EventCreator::set(this);
}

/////////////////////// UPDATED MESSAGE /////////////////////////////
Model_FeatureUpdatedMessage::Model_FeatureUpdatedMessage(
  const FeaturePtr& theFeature,
  const Events_ID& theEvent) : ModelAPI_FeatureUpdatedMessage(theEvent, 0)
{
  if (theFeature) myFeatures.insert(theFeature);
}

std::set<FeaturePtr> Model_FeatureUpdatedMessage::features() const 
{
  return myFeatures;
}

Events_MessageGroup* Model_FeatureUpdatedMessage::newEmpty() 
{
  FeaturePtr anEmptyFeature;
  return new Model_FeatureUpdatedMessage(anEmptyFeature, eventID());
}

void Model_FeatureUpdatedMessage::Join(Events_MessageGroup& theJoined) 
{
  Model_FeatureUpdatedMessage* aJoined = dynamic_cast<Model_FeatureUpdatedMessage*>(&theJoined);
  std::set<FeaturePtr >::iterator aFIter = aJoined->myFeatures.begin();
  for(; aFIter != aJoined->myFeatures.end(); aFIter++) {
    myFeatures.insert(*aFIter);
  }
}

/////////////////////// DELETED MESSAGE /////////////////////////////
Model_FeatureDeletedMessage::Model_FeatureDeletedMessage(
  const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup)
  : ModelAPI_FeatureDeletedMessage(messageId(), 0), myDoc(theDoc)
{
  if (!theGroup.empty())
    myGroups.insert(theGroup);
}

Events_MessageGroup* Model_FeatureDeletedMessage::newEmpty() 
{
  return new Model_FeatureDeletedMessage(myDoc, "");
}

const Events_ID Model_FeatureDeletedMessage::messageId()
{
  static Events_ID MY_ID = Events_Loop::eventByName(EVENT_FEATURE_DELETED);
  return MY_ID;
}

void Model_FeatureDeletedMessage::Join(Events_MessageGroup& theJoined)
{
  Model_FeatureDeletedMessage* aJoined = dynamic_cast<Model_FeatureDeletedMessage*>(&theJoined);
  std::set<std::string>::iterator aGIter = aJoined->myGroups.begin();
  for(; aGIter != aJoined->myGroups.end(); aGIter++) {
    myGroups.insert(*aGIter);
  }
}
