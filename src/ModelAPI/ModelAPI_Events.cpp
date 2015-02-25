// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

/*
 * ModelAPI_Events.cpp
 *
 *  Created on: Dec 8, 2014
 *      Author: sbh
 */

#include <ModelAPI.h>
#include <ModelAPI_Events.h>

ModelAPI_ObjectUpdatedMessage::ModelAPI_ObjectUpdatedMessage(const Events_ID theID,
                                                             const void* theSender)
    : Events_MessageGroup(theID, theSender)
{

}

ModelAPI_ObjectUpdatedMessage::~ModelAPI_ObjectUpdatedMessage()
{

}

ModelAPI_ObjectDeletedMessage::ModelAPI_ObjectDeletedMessage(const Events_ID theID,
                                                             const void* theSender)
    : Events_MessageGroup(theID, theSender)
{

}

ModelAPI_ObjectDeletedMessage::~ModelAPI_ObjectDeletedMessage()
{

}

ModelAPI_FeatureStateMessage::ModelAPI_FeatureStateMessage(const Events_ID theID,
                                                           const void* theSender)
 : Events_Message(theID, theSender)
{
  myCurrentFeature = std::shared_ptr<ModelAPI_Feature>();
}

ModelAPI_FeatureStateMessage::~ModelAPI_FeatureStateMessage()
{

}

std::shared_ptr<ModelAPI_Feature> ModelAPI_FeatureStateMessage::feature() const
{
  return myCurrentFeature;
}

void ModelAPI_FeatureStateMessage::setFeature(std::shared_ptr<ModelAPI_Feature>& theFeature)
{
  myCurrentFeature = theFeature;
}

bool ModelAPI_FeatureStateMessage::hasState(const std::string& theKey) const
{
  return myFeatureState.find(theKey) != myFeatureState.end();
}

bool ModelAPI_FeatureStateMessage::state(const  std::string& theFeatureId, bool theDefault) const
{
  if(hasState(theFeatureId)) {
    return myFeatureState.at(theFeatureId);
  }
  return theDefault;
}

void ModelAPI_FeatureStateMessage::setState(const std::string& theFeatureId, bool theValue)
{
  myFeatureState[theFeatureId] = theValue;
}

std::list<std::string> ModelAPI_FeatureStateMessage::features() const
{
  std::list<std::string> result;
  std::map<std::string, bool>::const_iterator it = myFeatureState.begin();
  for( ; it != myFeatureState.end(); ++it) {
    result.push_back(it->first);
  }
  return result;
}


ModelAPI_DocumentCreatedMessage::ModelAPI_DocumentCreatedMessage(const Events_ID theID, const void* theSender)
: Events_Message(theID, theSender)
{

}

ModelAPI_DocumentCreatedMessage::~ModelAPI_DocumentCreatedMessage()
{

}

DocumentPtr ModelAPI_DocumentCreatedMessage::document() const
{
  return myDocument;
}

void ModelAPI_DocumentCreatedMessage::setDocument(DocumentPtr theDocument)
{
  myDocument = theDocument;
}
