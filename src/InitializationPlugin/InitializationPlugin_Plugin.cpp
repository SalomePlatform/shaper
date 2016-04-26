// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <InitializationPlugin_Plugin.h>

#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Result.h>

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

    /// Issue 431: for the current moment create planes only in the module document,
    /// Later if it is needed we may create special initial planes in Parts (may be different)
    if (aDoc != ModelAPI_Session::get()->moduleDocument())
      return;

    std::list<FeaturePtr> aFeatures;

    // the viewer update should be blocked in order to avoid the features blinking before they are
    // hidden
    std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
        new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)));
    Events_Loop::loop()->send(aMsg);

    FeaturePtr aOrigin = createPoint(aDoc, "Origin", 0., 0., 0.);
    aFeatures.push_back(aOrigin);
    aFeatures.push_back(createAxis(aDoc, aOrigin, 100., 0., 0.));
    aFeatures.push_back(createAxis(aDoc, aOrigin, 0., 100., 0.));
    aFeatures.push_back(createAxis(aDoc, aOrigin, 0., 0., 100.));
    aFeatures.push_back(createPlane(aDoc, 1., 0., 0.));
    aFeatures.push_back(createPlane(aDoc, 0., -1., 0.));
    aFeatures.push_back(createPlane(aDoc, 0., 0., 1.));
    // for PartSet it is done outside of the transaction, so explicitly flush this creation
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));

    // hides the created features, the precondition is that the feature's results have been
    // already built, so the createPlane/Points method calls the execute function for the planes
    std::list<FeaturePtr >::const_iterator aFIter = aFeatures.begin();
    for (; aFIter != aFeatures.cend(); aFIter++) {
      FeaturePtr aPlane = *aFIter;
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aPlane->results();
      std::list<ResultPtr >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        (*aRIter)->setDisplayed(false);
      }
    }
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));

    // the viewer update should be unblocked in order to avoid the features blinking before they are
    // hidden
    aMsg = std::shared_ptr<Events_Message>(
                  new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));

    Events_Loop::loop()->send(aMsg);

  } else if (theMessage.get()) {
    Events_Error::send(
        std::string("InitializationPlugin_Plugin::processEvent: unhandled message caught: ")
            + theMessage->eventID().eventText());
  }
}

FeaturePtr InitializationPlugin_Plugin::createPlane(DocumentPtr theDoc, double theX, double theY,
                                                    double theZ)
{
  FeaturePtr aPlane = theDoc->addFeature("Plane");
  aPlane->string("CreationMethod")->setValue("PlaneByGeneralEquation");
  aPlane->real("A")->setValue(theX);
  aPlane->real("B")->setValue(theY);
  aPlane->real("C")->setValue(theZ);
  aPlane->real("D")->setValue(0.);

  if (theX) {
    aPlane->data()->setName("YOZ");
  } else if (theY) {
    aPlane->data()->setName("XOZ");
  } else if (theZ) {
    aPlane->data()->setName("XOY");
  }
  aPlane->setInHistory(aPlane, false);  // don't show automatically created feature in the features history

  // the plane should be executed in order to build the feature result immediatelly
  // the results are to be hidden in the plugin
  aPlane->execute();
  // this flag is needed here to avoid setting it inside of the next transaction
  // (may cause crash on redo of the first transaction in OCAF)
  aPlane->data()->execState(ModelAPI_StateDone);
  aPlane->firstResult()->data()->execState(ModelAPI_StateDone);

  return aPlane;
}

FeaturePtr InitializationPlugin_Plugin::createPoint(DocumentPtr theDoc, const std::string& theName,
                                                    double theX, double theY, double theZ)
{
  std::shared_ptr<ModelAPI_Feature> aPoint = theDoc->addFeature("Point");
  aPoint->real("x")->setValue(theX);
  aPoint->real("y")->setValue(theY);
  aPoint->real("z")->setValue(theZ);
  aPoint->data()->setName(theName);
  aPoint->setInHistory(aPoint, false);  // don't show automatically created feature in the features history

  // the point should be executed in order to build the feature result immediatelly
  // the results are to be hidden in the plugin
  aPoint->execute();
  aPoint->data()->execState(ModelAPI_StateDone);
  aPoint->firstResult()->data()->execState(ModelAPI_StateDone);

  return aPoint;
}

FeaturePtr InitializationPlugin_Plugin::createAxis(DocumentPtr theDoc, FeaturePtr theOrigin,
                                                   double theX, double theY, double theZ)
{
  std::shared_ptr<ModelAPI_Feature> aAxis = theDoc->addFeature("Axis");
  aAxis->string("CreationMethod")->setValue("AxisByPointAndDirection");

  ResultPtr aResult = theOrigin->firstResult();
  aAxis->selection("FirstPoint")->setValue(aResult, aResult->shape());

  aAxis->real("X_Direction")->setValue(theX);
  aAxis->real("Y_Direction")->setValue(theY);
  aAxis->real("Z_Direction")->setValue(theZ);

  if (theX != 0) {
    aAxis->data()->setName("OX");
  } else if (theY != 0) {
    aAxis->data()->setName("OY");
  } else if (theZ != 0) {
    aAxis->data()->setName("OZ");
  }
  aAxis->setInHistory(aAxis, false);  // don't show automatically created feature in the features history
  aAxis->execute();
  aAxis->data()->execState(ModelAPI_StateDone);
  aAxis->firstResult()->data()->execState(ModelAPI_StateDone);

  return aAxis;
}
