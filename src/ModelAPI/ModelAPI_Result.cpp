// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_Result.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include "ModelAPI_Result.h"
#include <ModelAPI_Events.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Attribute.h>
#include <Events_Loop.h>

ModelAPI_Result::~ModelAPI_Result()
{
}

bool ModelAPI_Result::setDisabled(std::shared_ptr<ModelAPI_Result> theThis, const bool theFlag)
{
  if (myIsDisabled != theFlag) {
    myIsDisabled = theFlag;
    // this must be before "updated" message send to have history updated for OB update
    document()->updateHistory(groupName()); // to update the history cash data in the document
    // generate related events
    static Events_Loop* aLoop = Events_Loop::loop();
    static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
    if (myIsDisabled) { // disabled result looks like removed
      aECreator->sendDeleted(document(), groupName());
    } else { // un-disabled equals to created
      static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
      aECreator->sendUpdated(theThis, anEvent, false); // do not group: creation must be immediate
    }
    static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    aECreator->sendUpdated(theThis, EVENT_DISP, false);
    return true;
  }
  return false;
}

bool ModelAPI_Result::isDisabled() const
{
  return myIsDisabled;
}

void ModelAPI_Result::setIsConcealed(const bool theValue)
{
  if (myIsConcealed != theValue) {
    myIsConcealed = theValue;
    if (document().get()) // can be on creation of result
      document()->updateHistory(groupName()); // to update the history cash data in the document
  }
}


std::shared_ptr<GeomAPI_Shape> ModelAPI_Result::shape()
{
  return std::shared_ptr<GeomAPI_Shape>();
}

void ModelAPI_Result::attributeChanged(const std::string& theID)
{
  static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();
  aECreator->sendUpdated(data()->attribute(theID)->owner(), EVENT_DISP);
}
