// File:        ModelAPI_Feature.cpp
// Created:     17 Jul 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_Feature.h"
#include <ModelAPI_Events.h>
#include <ModelAPI_Result.h>
#include <Events_Loop.h>

const std::list<boost::shared_ptr<ModelAPI_Result> >& ModelAPI_Feature::results() 
{
  return myResults;
}

boost::shared_ptr<ModelAPI_Result> ModelAPI_Feature::firstResult() 
{
  return myResults.empty() ? boost::shared_ptr<ModelAPI_Result>() : *(myResults.begin());
}

void ModelAPI_Feature::setResult(const boost::shared_ptr<ModelAPI_Result>& theResult) 
{
  if (firstResult() == theResult) { // just updated
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
    ModelAPI_EventCreator::get()->sendUpdated(theResult, anEvent);
    return;
  }
  // created
  while(!myResults.empty()) { // remove one by one with messages
    boost::shared_ptr<ModelAPI_Result> aRes = *(myResults.begin());
    myResults.erase(myResults.begin());
    ModelAPI_EventCreator::get()->sendDeleted(aRes->document(), aRes->groupName());
  }
  myResults.push_back(theResult);
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
  ModelAPI_EventCreator::get()->sendUpdated(theResult, anEvent);
  // Create event for first Feature 
  Events_Loop::loop()->flush(anEvent);
}

boost::shared_ptr<ModelAPI_Document> ModelAPI_Feature::documentToAdd()
{
  return ModelAPI_PluginManager::get()->currentDocument();
}

ModelAPI_Feature::~ModelAPI_Feature()
{
  while(!myResults.empty()) { // remove one by one with messages
    boost::shared_ptr<ModelAPI_Result> aRes = *(myResults.begin());
    myResults.erase(myResults.begin());
    ModelAPI_EventCreator::get()->sendDeleted(aRes->document(), aRes->groupName());
  }
}
