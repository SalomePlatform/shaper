// File:        Model_Events.cxx
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#include <Model_Events.h>
#include <Events_Loop.h>

// UPDATED methods
Events_MessageGroup* Model_FeatureUpdatedMessage::newEmpty() {
  boost::shared_ptr<ModelAPI_Feature> anEmptyFeature;
  return new Model_FeatureUpdatedMessage(anEmptyFeature, eventID());
}

void Model_FeatureUpdatedMessage::Join(Events_MessageGroup& theJoined)
{
  Model_FeatureUpdatedMessage* aJoined = dynamic_cast<Model_FeatureUpdatedMessage*>(&theJoined);
  std::set<boost::shared_ptr<ModelAPI_Feature> >::iterator aFIter = aJoined->myFeatures.begin();
  for(; aFIter != aJoined->myFeatures.end(); aFIter++) {
    myFeatures.insert(*aFIter);
  }
}

// DELETED methods
Events_MessageGroup* Model_FeatureDeletedMessage::newEmpty() {
  return new Model_FeatureDeletedMessage(myDoc, "");
}

Model_FeatureDeletedMessage::Model_FeatureDeletedMessage(
  const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup)
  : Events_MessageGroup(messageId(), 0), myDoc(theDoc)

{
  if (!theGroup.empty())
    myGroups.insert(theGroup);
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
