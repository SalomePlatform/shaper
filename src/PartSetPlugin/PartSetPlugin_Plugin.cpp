// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "PartSetPlugin_Plugin.h"
#include "PartSetPlugin_Part.h"
#include "PartSetPlugin_Duplicate.h"
#include "PartSetPlugin_Remove.h"

#include <Events_Loop.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>

#ifdef _DEBUG
#include <iostream>
#endif

using namespace std;

// the only created instance of this plugin
static PartSetPlugin_Plugin* MY_PARTSET_INSTANCE = new PartSetPlugin_Plugin();

PartSetPlugin_Plugin::PartSetPlugin_Plugin()
{
  // register this plugin
  ModelAPI_Session::get()->registerPlugin(this);
}

FeaturePtr PartSetPlugin_Plugin::createFeature(string theFeatureID)
{
  if (theFeatureID == PartSetPlugin_Part::ID()) {
    return FeaturePtr(new PartSetPlugin_Part);
  }
  if (theFeatureID == PartSetPlugin_Duplicate::ID()) {
    return FeaturePtr(new PartSetPlugin_Duplicate);
  }
  if (theFeatureID == PartSetPlugin_Remove::ID()) {
    return FeaturePtr(new PartSetPlugin_Remove);
  }
  // feature of such kind is not found
  return FeaturePtr();
}

void PartSetPlugin_Plugin::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  const Events_ID kRequestEvent =
      Events_Loop::loop()->eventByName(EVENT_FEATURE_STATE_REQUEST);
  if (theMessage->eventID() == kRequestEvent) {
    Events_Loop::loop()->send(getFeaturesState(), false);
  } else if (theMessage.get()) {
    #ifdef _DEBUG
    std::cout << "PartSetPlugin_Plugin::processEvent: unhandled message caught: " << std::endl
              << theMessage->eventID().eventText() << std::endl;
    #endif
  }
}

std::shared_ptr<ModelAPI_FeatureStateMessage> PartSetPlugin_Plugin::getFeaturesState()
{
  const Events_ID kResponseEvent =
        Events_Loop::loop()->eventByName(EVENT_FEATURE_STATE_RESPONSE);
  std::shared_ptr<ModelAPI_FeatureStateMessage> aMsg(
      new ModelAPI_FeatureStateMessage(kResponseEvent, this));
  std::string aStdDocKind = ModelAPI_Session::get()->activeDocument()->kind();
  bool aDocIsPart = (aStdDocKind == PartSetPlugin_Part::ID());
  aMsg->setState(PartSetPlugin_Part::ID(), true);
  aMsg->setState(PartSetPlugin_Duplicate::ID(), aDocIsPart);
  aMsg->setState(PartSetPlugin_Remove::ID(), aDocIsPart);
  return aMsg;
}
