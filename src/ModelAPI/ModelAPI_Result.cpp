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
