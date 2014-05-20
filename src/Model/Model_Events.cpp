// File:        Model_Events.cxx
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#include <Model_Events.h>
#include <Events_Loop.h>

Model_FeatureDeletedMessage::Model_FeatureDeletedMessage(
  const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup)
  : Events_Message(messageId(), 0), myDoc(theDoc), myGroup(theGroup)

{
}

const Events_ID Model_FeatureDeletedMessage::messageId()
{
  static Events_ID MY_ID = Events_Loop::eventByName(EVENT_FEATURE_DELETED);
  return MY_ID;
}

Model_FeaturesMovedMessage::Model_FeaturesMovedMessage()
: Events_Message(messageId(), 0)
{
}

const Events_ID Model_FeaturesMovedMessage::messageId()
{
  static Events_ID MY_ID = Events_Loop::eventByName(EVENT_FEATURES_MOVED);
  return MY_ID;
}

void Model_FeaturesMovedMessage::setFeatures(
                                const std::list<boost::shared_ptr<ModelAPI_Feature> >& theFeatures)
{
  myFeatures = theFeatures;  
}

const std::list<boost::shared_ptr<ModelAPI_Feature> >& Model_FeaturesMovedMessage::features() const
{
  return myFeatures;
}

