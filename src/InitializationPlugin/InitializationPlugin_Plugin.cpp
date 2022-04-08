// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#include <InitializationPlugin_Plugin.h>

#include <InitializationPlugin_EvalListener.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeString.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Result.h>

#include <GeomDataAPI_Point.h>

#include <Events_Message.h>
#include <Events_InfoMessage.h>
#include <Events_MessageBool.h>

#include <memory>

// the only created instance of this plugin
static InitializationPlugin_Plugin* MY_INITIALIZATIONPLUGIN_INSTANCE =
    new InitializationPlugin_Plugin();

InitializationPlugin_Plugin::InitializationPlugin_Plugin() :
  myCreatePartOnStart(false) // by default in TUI mode part is not created on start of PartSet
{
  char* isUnitTest = getenv("SHAPER_UNIT_TEST_IN_PROGRESS");
  myInitDataModel = (!isUnitTest || isUnitTest[0] != '1');

  Events_Loop* aLoop = Events_Loop::loop();
  static const Events_ID kDocCreatedEvent = ModelAPI_DocumentCreatedMessage::eventId();
  aLoop->registerListener(this, kDocCreatedEvent, NULL, true);
  static const Events_ID kCreatePartEvent = Events_Loop::eventByName(EVENT_CREATE_PART_ON_START);
  aLoop->registerListener(this, kCreatePartEvent, NULL, true);

  myEvalListener =
    std::shared_ptr<InitializationPlugin_EvalListener>(new InitializationPlugin_EvalListener());
}

void InitializationPlugin_Plugin::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  static const Events_ID kDocCreatedEvent = ModelAPI_DocumentCreatedMessage::eventId();
  static const Events_ID kCreatePartEvent = Events_Loop::eventByName(EVENT_CREATE_PART_ON_START);
  if (theMessage->eventID() == kDocCreatedEvent) {
    std::shared_ptr<ModelAPI_DocumentCreatedMessage> aMessage = std::dynamic_pointer_cast<
        ModelAPI_DocumentCreatedMessage>(theMessage);
    DocumentPtr aDoc = aMessage->document();

    /// Issue 431: for the current moment create planes only in the module document,
    /// Later if it is needed we may create special initial planes in Parts (may be different)
    SessionPtr aMgr = ModelAPI_Session::get();
    if (aDoc != aMgr->moduleDocument() || aMgr->isLoading())
      return;

    if (myInitDataModel)
      myEvalListener->initDataModel();

    std::list<FeaturePtr> aFeatures;
    aMgr->startOperation("Initialization");

    // the viewer update should be blocked in order to avoid the features blinking before they are
    // hidden
    std::shared_ptr<Events_Message> aMsg = std::shared_ptr<Events_Message>(
        new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_BLOCKED)));
    Events_Loop::loop()->send(aMsg);

    FeaturePtr aOrigin = createPoint(aDoc, L"Origin", 0., 0., 0.);
    aFeatures.push_back(aOrigin);
    aFeatures.push_back(createAxis(aDoc, aOrigin, 100., 0., 0.));
    aFeatures.push_back(createAxis(aDoc, aOrigin, 0., 100., 0.));
    aFeatures.push_back(createAxis(aDoc, aOrigin, 0., 0., 100.));
    aFeatures.push_back(createPlane(aDoc, 1., 0., 0.));
    aFeatures.push_back(createPlane(aDoc, 0., -1., 0.));
    aFeatures.push_back(createPlane(aDoc, 0., 0., 1.));
    if (myCreatePartOnStart)
      createPart(aDoc);
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

    aMgr->finishOperation(); // before last message flush to update the title, make it not-modified
    aMgr->clearUndoRedo(); // to forbid undo of auxiliary elements and initial part

    // the viewer update should be unblocked to avoid the features blinking before they are hidden
    aMsg = std::shared_ptr<Events_Message>(
                  new Events_Message(Events_Loop::eventByName(EVENT_UPDATE_VIEWER_UNBLOCKED)));

    Events_Loop::loop()->send(aMsg);
  }
  else if (theMessage->eventID() == kCreatePartEvent)
  {
    std::shared_ptr<Events_MessageBool> aMsg = std::dynamic_pointer_cast<Events_MessageBool>(theMessage);
    myCreatePartOnStart = aMsg->value();
  }
}

FeaturePtr InitializationPlugin_Plugin::createPlane(DocumentPtr theDoc, double theX, double theY,
                                                    double theZ)
{
  FeaturePtr aPlane = theDoc->addFeature("Plane");
  aPlane->string("creation_method")->setValue("by_general_equation");
  aPlane->real("A")->setValue(theX);
  aPlane->real("B")->setValue(theY);
  aPlane->real("C")->setValue(theZ);
  aPlane->real("D")->setValue(0.);

  if (theX) {
    aPlane->data()->setName(L"YOZ");
  } else if (theY) {
    aPlane->data()->setName(L"XOZ");
  } else if (theZ) {
    aPlane->data()->setName(L"XOY");
  }
    // don't show automatically created feature in the features history
  aPlane->setInHistory(aPlane, false);

  // the plane should be executed in order to build the feature result immediatelly
  // the results are to be hidden in the plugin
  aPlane->execute();
  // this flag is needed here to avoid setting it inside of the next transaction
  // (may cause crash on redo of the first transaction in OCAF)
  aPlane->data()->execState(ModelAPI_StateDone);
  aPlane->firstResult()->data()->execState(ModelAPI_StateDone);

  return aPlane;
}

FeaturePtr InitializationPlugin_Plugin::createPoint(DocumentPtr theDoc, const std::wstring& theName,
                                                    double theX, double theY, double theZ)
{
  std::shared_ptr<ModelAPI_Feature> aPoint = theDoc->addFeature("Point");
  AttributePointPtr aPointAttr = std::dynamic_pointer_cast<GeomDataAPI_Point>
    (aPoint->data()->attribute("point3d"));
  aPointAttr->setValue(theX, theY, theZ);
  aPoint->string("creation_method")->setValue("by_xyz");
  aPoint->data()->setName(theName);
  // don't show automatically created feature in the features history
  aPoint->setInHistory(aPoint, false);

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
    aAxis->data()->setName(L"OX");
  } else if (theY != 0) {
    aAxis->data()->setName(L"OY");
  } else if (theZ != 0) {
    aAxis->data()->setName(L"OZ");
  }
   // don't show automatically created feature in the features history
  aAxis->setInHistory(aAxis, false);
  aAxis->execute();
  aAxis->data()->execState(ModelAPI_StateDone);
  aAxis->firstResult()->data()->execState(ModelAPI_StateDone);

  return aAxis;
}

void InitializationPlugin_Plugin::createPart(DocumentPtr thePartSet)
{
  std::shared_ptr<ModelAPI_Feature> aPart = thePartSet->addFeature("Part");
  if (aPart.get())
    aPart->execute(); // to initialize and activate this part document
}
