// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <InitializationPlugin_Plugin.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_Events.h>

#include <Events_Message.h>
#include <Events_Error.h>

#include <memory>

// the only created instance of this plugin
static InitializationPlugin_Plugin* MY_INITIALIZATIONPLUGIN_INSTANCE =
    new InitializationPlugin_Plugin();

InitializationPlugin_Plugin::InitializationPlugin_Plugin()
{
  Events_Loop* aLoop = Events_Loop::loop();
  const Events_ID kDocCreatedEvent = ModelAPI_DocumentCreatedMessage::eventId();
  aLoop->registerListener(this, kDocCreatedEvent, NULL, true);
}

void InitializationPlugin_Plugin::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  const Events_ID kDocCreatedEvent = ModelAPI_DocumentCreatedMessage::eventId();
  if (theMessage->eventID() == kDocCreatedEvent) {
    std::shared_ptr<ModelAPI_DocumentCreatedMessage> aMessage = std::dynamic_pointer_cast<
        ModelAPI_DocumentCreatedMessage>(theMessage);
    DocumentPtr aDoc = aMessage->document();
    createPoint(aDoc);
    createPlane(aDoc, 1., 0., 0.);
    createPlane(aDoc, 0., 1., 0.);
    createPlane(aDoc, 0., 0., 1.);
    // for PartSet it is done outside of the transaction, so explicitly flush this creation
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  } else if (theMessage.get()) {
    Events_Error::send(
        std::string("InitializationPlugin_Plugin::processEvent: unhandled message caught: ")
            + theMessage->eventID().eventText());
  }
}

void InitializationPlugin_Plugin::createPlane(DocumentPtr theDoc, double theX, double theY,
                                              double theZ)
{
  std::shared_ptr<ModelAPI_Feature> aPlane = theDoc->addFeature("Plane");
  aPlane->string("CreationMethod")->setValue("PlaneByGeneralEquation");
  aPlane->real("A")->setValue(theX);
  aPlane->real("B")->setValue(theY);
  aPlane->real("C")->setValue(theZ);
  aPlane->real("D")->setValue(0.);

  if (theX) {
    aPlane->data()->setName("Y0Z");
  } else if (theY) {
    aPlane->data()->setName("X0Z");
  } else if (theZ) {
    aPlane->data()->setName("X0Y");
  }
  aPlane->setInHistory(aPlane, false);  // don't show automatically created feature in the features history
}

void InitializationPlugin_Plugin::createPoint(DocumentPtr theDoc)
{
  std::shared_ptr<ModelAPI_Feature> aPoint = theDoc->addFeature("Point");
  aPoint->real("x")->setValue(0.);
  aPoint->real("y")->setValue(0.);
  aPoint->real("z")->setValue(0.);
  aPoint->data()->setName("Origin");
  aPoint->setInHistory(aPoint, false);  // don't show automatically created feature in the features history
}
