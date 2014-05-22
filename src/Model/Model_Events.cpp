// File:        Model_Events.cxx
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#include <Model_Events.h>
#include <Events_Loop.h>

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
