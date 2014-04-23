// File:        Model_Events.cxx
// Created:     10 Apr 2014
// Author:      Mikhail PONIKAROV

#include <Model_Events.h>
#include <Event_Loop.h>

ModelAPI_FeatureUpdatedMessage::ModelAPI_FeatureUpdatedMessage(
  const boost::shared_ptr<ModelAPI_Document>& theDoc,
  const boost::shared_ptr<ModelAPI_Feature>& theFeature, const Event_ID& theEvent)
  : Event_Message(theEvent, 0), myFeature(theFeature), myDoc(theDoc)
{}

ModelAPI_FeatureDeletedMessage::ModelAPI_FeatureDeletedMessage(
  const boost::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup)
  : Event_Message(messageId(), 0), myDoc(theDoc), myGroup(theGroup)

{
}

const Event_ID ModelAPI_FeatureDeletedMessage::messageId()
{
  static Event_ID MY_ID = Event_Loop::eventByName(EVENT_FEATURE_DELETED);
  return MY_ID;
}
