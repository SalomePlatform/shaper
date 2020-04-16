// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Model_Events.h>
#include <Events_Loop.h>

/// Alone instance of the creator per application
Model_EventCreator MY_CREATOR;

/////////////////////// CREATOR /////////////////////////////
void Model_EventCreator::sendUpdated(const ObjectPtr& theObject, const Events_ID& theEvent,
                                     const bool isGroupped) const
{
  std::shared_ptr<Model_ObjectUpdatedMessage> aMsg(
    new Model_ObjectUpdatedMessage(theObject, theEvent));
  Events_Loop::loop()->send(aMsg, isGroupped);
}

void Model_EventCreator::sendUpdated(const std::list<ObjectPtr>& theObjects,
  const Events_ID& theEvent, const bool isGroupped) const
{
  if (theObjects.empty())
    return;
  std::list<ObjectPtr>::const_iterator anObj = theObjects.cbegin();
  std::shared_ptr<Model_ObjectUpdatedMessage> aMsg(
    new Model_ObjectUpdatedMessage(*anObj, theEvent));
  for(anObj++; anObj != theObjects.cend(); anObj++) {
    std::shared_ptr<Model_ObjectUpdatedMessage> aJoined(
      new Model_ObjectUpdatedMessage(*anObj, theEvent));
    aMsg->Join(aJoined);
  }
  Events_Loop::loop()->send(aMsg, isGroupped);
}

void Model_EventCreator::sendDeleted(const std::shared_ptr<ModelAPI_Document>& theDoc,
                                     const std::string& theGroup) const
{
  std::shared_ptr<Model_ObjectDeletedMessage> aMsg(
    new Model_ObjectDeletedMessage(theDoc, theGroup));
  Events_Loop::loop()->send(aMsg, true);
}

void Model_EventCreator::sendReordered(const std::shared_ptr<ModelAPI_Feature>& theReordered) const
{
  std::shared_ptr<Model_OrderUpdatedMessage> aMsg(
    new Model_OrderUpdatedMessage(theReordered));
  Events_Loop::loop()->send(aMsg, true);
}

Model_EventCreator::Model_EventCreator()
{
  ModelAPI_EventCreator::set(this);
}

/////////////////////// UPDATED MESSAGE /////////////////////////////
Model_ObjectUpdatedMessage::Model_ObjectUpdatedMessage(const ObjectPtr& theObject,
                                                       const Events_ID& theEvent)
    : ModelAPI_ObjectUpdatedMessage(theEvent, 0)
{
  if (theObject) {
    myObjects.insert(theObject);
  }
}

const std::set<ObjectPtr>& Model_ObjectUpdatedMessage::objects() const
{
  return myObjects;
}

std::shared_ptr<Events_MessageGroup> Model_ObjectUpdatedMessage::newEmpty()
{
  ObjectPtr anEmptyObject;
  return std::shared_ptr<Model_ObjectUpdatedMessage>(
    new Model_ObjectUpdatedMessage(anEmptyObject, eventID()));
}

void Model_ObjectUpdatedMessage::Join(const std::shared_ptr<Events_MessageGroup>& theJoined)
{
  std::shared_ptr<Model_ObjectUpdatedMessage> aJoined =
    std::dynamic_pointer_cast<Model_ObjectUpdatedMessage>(theJoined);
  std::set<ObjectPtr>::iterator aFIter = aJoined->myObjects.begin();
  for (; aFIter != aJoined->myObjects.end(); aFIter++) {
    myObjects.insert(*aFIter);
  }
}

/////////////////////// DELETED MESSAGE /////////////////////////////
Model_ObjectDeletedMessage::Model_ObjectDeletedMessage(
    const std::shared_ptr<ModelAPI_Document>& theDoc, const std::string& theGroup)
    : ModelAPI_ObjectDeletedMessage(messageId(), 0)
{
  if (!theGroup.empty()) {
    myGroups.push_back(
      std::pair<std::shared_ptr<ModelAPI_Document>, std::string>(theDoc, theGroup));
  }
}

std::shared_ptr<Events_MessageGroup> Model_ObjectDeletedMessage::newEmpty()
{
  static const std::shared_ptr<ModelAPI_Document> anEmpty;
  return std::shared_ptr<Model_ObjectDeletedMessage>(new Model_ObjectDeletedMessage(anEmpty, ""));
}

const Events_ID Model_ObjectDeletedMessage::messageId()
{
  static Events_ID MY_ID = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
  return MY_ID;
}

void Model_ObjectDeletedMessage::Join(const std::shared_ptr<Events_MessageGroup>& theJoined)
{
  std::shared_ptr<Model_ObjectDeletedMessage> aJoined =
    std::dynamic_pointer_cast<Model_ObjectDeletedMessage>(theJoined);

  const std::list<std::pair<std::shared_ptr<ModelAPI_Document>, std::string>>& aJGroups =
    aJoined->groups();

  std::list<std::pair<std::shared_ptr<ModelAPI_Document>, std::string>>::iterator aGIter;
  std::list<std::pair<std::shared_ptr<ModelAPI_Document>, std::string>>::const_iterator aJIter;
  for (aJIter = aJGroups.cbegin(); aJIter != aJGroups.cend(); aJIter++) {
    for (aGIter = myGroups.begin(); aGIter != myGroups.end(); aGIter++) {
      if (aGIter->first == aJIter->first && aGIter->second == aJIter->second)
        break; // exists, so no need to insert
    }
    if (aGIter == myGroups.end())
      myGroups.push_back(*aJIter);
  }
}

/////////////////////// REORDERED MESSAGE /////////////////////////////
Model_OrderUpdatedMessage::Model_OrderUpdatedMessage(
    FeaturePtr theReordered, const void* theSender)
    : ModelAPI_OrderUpdatedMessage(messageId(), theSender),
    myReordered(theReordered)
{
}

const Events_ID Model_OrderUpdatedMessage::messageId()
{
  static Events_ID MY_ID = Events_Loop::eventByName(EVENT_ORDER_UPDATED);
  return MY_ID;
}
