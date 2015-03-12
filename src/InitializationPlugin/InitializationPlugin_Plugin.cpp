
// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <InitializationPlugin_Plugin.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>


#include <Events_Message.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>

#include <memory>

#ifdef _DEBUG
#include <iostream>
#endif

// the only created instance of this plugin
static InitializationPlugin_Plugin* MY_INITIALIZATIONPLUGIN_INSTANCE = new InitializationPlugin_Plugin();

InitializationPlugin_Plugin::InitializationPlugin_Plugin()
{
  // register this plugin
  SessionPtr aSession = ModelAPI_Session::get();
  aSession->registerPlugin(this);

  Events_Loop* aLoop = Events_Loop::loop();
  const Events_ID kDocCreatedEvent = ModelAPI_DocumentCreatedMessage::eventId();
  aLoop->registerListener(this, kDocCreatedEvent, NULL, true);
}

FeaturePtr InitializationPlugin_Plugin::createFeature(std::string theFeatureID)
{
  return FeaturePtr();
}

void InitializationPlugin_Plugin::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  const Events_ID kDocCreatedEvent = ModelAPI_DocumentCreatedMessage::eventId();
  if (theMessage->eventID() == kDocCreatedEvent) {
    std::shared_ptr<ModelAPI_DocumentCreatedMessage> aMessage =
        std::dynamic_pointer_cast<ModelAPI_DocumentCreatedMessage>(theMessage);
    DocumentPtr aDoc = aMessage->document();
    createPoint(aDoc);
    createPlane(aDoc, 1., 0., 0.);
    createPlane(aDoc, 0., 1., 0.);
    createPlane(aDoc, 0., 0., 1.);
  } else if (theMessage.get()) {
    #ifdef _DEBUG
    std::cout << "InitializationPlugin_Plugin::processEvent: unhandled message caught: " << std::endl
              << theMessage->eventID().eventText() << std::endl;
    #endif
  }
}

void InitializationPlugin_Plugin::createPlane(DocumentPtr theDoc, double theA, double theB, double theC)
{
  std::shared_ptr<ModelAPI_Feature> aPlane = theDoc->addFeature("Plane");
  aPlane->string("CreationMethod")->setValue("PlaneByGeneralEquation");
  aPlane->real("A")->setValue(theA);
  aPlane->real("B")->setValue(theB);
  aPlane->real("C")->setValue(theC);
  aPlane->real("D")->setValue(0.);

  if (theA) {
    aPlane->data()->setName("Y0Z");
  } else if (theB) {
    aPlane->data()->setName("X0Z");
  } else if (theC) {
    aPlane->data()->setName("X0Y");
  }
}

void InitializationPlugin_Plugin::createPoint(DocumentPtr theDoc)
{
  std::shared_ptr<ModelAPI_Feature> aPoint = theDoc->addFeature("Point");
  aPoint->real("x")->setValue(0.);
  aPoint->real("y")->setValue(0.);
  aPoint->real("z")->setValue(0.);
  aPoint->data()->setName("Origin");
}

