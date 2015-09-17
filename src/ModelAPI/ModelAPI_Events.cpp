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

ModelAPI_OrderUpdatedMessage::ModelAPI_OrderUpdatedMessage(const Events_ID theID,
                                                             const void* theSender)
    : Events_Message(theID, theSender)
{

}

ModelAPI_OrderUpdatedMessage::~ModelAPI_OrderUpdatedMessage()
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


ModelAPI_DocumentCreatedMessage::ModelAPI_DocumentCreatedMessage(const Events_ID theID,
                                                                 const void* theSender)
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

ModelAPI_AttributeEvalMessage::ModelAPI_AttributeEvalMessage(const Events_ID theID,
                                                                         const void* theSender)
: Events_Message(theID, theSender)
{

}

ModelAPI_AttributeEvalMessage::~ModelAPI_AttributeEvalMessage()
{

}

AttributePtr ModelAPI_AttributeEvalMessage::attribute() const
{
  return myAttribute;
}

void ModelAPI_AttributeEvalMessage::setAttribute(AttributePtr theDocument)
{
  myAttribute = theDocument;
}

ModelAPI_ObjectRenamedMessage::ModelAPI_ObjectRenamedMessage(const Events_ID theID,
                                                             const void* theSender)
: Events_Message(theID, theSender)
{

}

ModelAPI_ObjectRenamedMessage::~ModelAPI_ObjectRenamedMessage()
{

}

void ModelAPI_ObjectRenamedMessage::send(ObjectPtr theObject,
                                         const std::string& theOldName,
                                         const std::string& theNewName,
                                         const void* theSender)
{
  std::shared_ptr<ModelAPI_ObjectRenamedMessage> aMessage(
    new ModelAPI_ObjectRenamedMessage(eventId(), theSender));
  aMessage->setObject(theObject);
  aMessage->setOldName(theOldName);
  aMessage->setNewName(theNewName);
  Events_Loop::loop()->send(aMessage);
}

ObjectPtr ModelAPI_ObjectRenamedMessage::object() const
{
  return myObject;
}

void ModelAPI_ObjectRenamedMessage::setObject(ObjectPtr theObject)
{
  myObject = theObject;
}

std::string ModelAPI_ObjectRenamedMessage::oldName() const
{
  return myOldName;
}

void ModelAPI_ObjectRenamedMessage::setOldName(const std::string& theOldName)
{
  myOldName = theOldName;
}

std::string ModelAPI_ObjectRenamedMessage::newName() const
{
  return myNewName;
}

void ModelAPI_ObjectRenamedMessage::setNewName(const std::string& theNewName)
{
  myNewName = theNewName;
}

ModelAPI_ReplaceParameterMessage::ModelAPI_ReplaceParameterMessage(const Events_ID theID,
                                                                   const void* theSender)
: Events_Message(theID, theSender)
{

}

ModelAPI_ReplaceParameterMessage::~ModelAPI_ReplaceParameterMessage()
{

}

void ModelAPI_ReplaceParameterMessage::send(ObjectPtr theObject,
                                            const void* theSender)
{
  std::shared_ptr<ModelAPI_ReplaceParameterMessage> aMessage(
      new ModelAPI_ReplaceParameterMessage(eventId(), theSender));
  aMessage->setObject(theObject);
  Events_Loop::loop()->send(aMessage);
}

ObjectPtr ModelAPI_ReplaceParameterMessage::object() const
{
  return myObject;
}

void ModelAPI_ReplaceParameterMessage::setObject(ObjectPtr theObject)
{
  myObject = theObject;
}
