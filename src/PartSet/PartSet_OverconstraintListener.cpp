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

#include <ModelAPI_Tools.h>
#include <ModelAPI_AttributeString.h>

#include "PartSet_OverconstraintListener.h"
#include <PartSet_Module.h>
#include <PartSet_SketcherMgr.h>
#include <PartSet_SketcherReentrantMgr.h>

#include "XGUI_Displayer.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_Tools.h"
#include "XGUI_Workshop.h"

#include "SketcherPrs_SymbolPrs.h"
#include "SketchPlugin_SketchEntity.h"
#include "SketchPlugin_MacroArcReentrantMessage.h"
#include "SketchPlugin_Sketch.h"

#include "Events_Loop.h"

#include <GeomAPI_IPresentable.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_EventReentrantMessage.h>
#include <ModuleBase_Tools.h>

#include <QString>

//#define DEBUG_FEATURE_OVERCONSTRAINT_LISTENER

PartSet_OverconstraintListener::PartSet_OverconstraintListener(ModuleBase_IWorkshop* theWorkshop)
: myWorkshop(theWorkshop), myIsActive(false), myIsFullyConstrained(false)
{
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SOLVER_FAILED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SOLVER_REPAIRED));

  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SKETCH_UNDER_CONSTRAINED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SKETCH_FULLY_CONSTRAINED));

  aLoop->registerListener(this, ModelAPI_EventReentrantMessage::eventId());
  aLoop->registerListener(this, SketchPlugin_MacroArcReentrantMessage::eventId());
}

void PartSet_OverconstraintListener::setActive(const bool& theActive)
{
  if (myIsActive == theActive)
    return;

  myIsActive = theActive;
  myIsFullyConstrained = false; /// returned to default state, no custom color for it

  if (myIsActive) {
    PartSet_Module* aModule = module();
    CompositeFeaturePtr aSketch = aModule->sketchMgr()->activeSketch();
    if (aSketch.get()) {
      QString aDOFMessage(aSketch->string(SketchPlugin_Sketch::SOLVER_DOF())->value().c_str());
      if (aDOFMessage.contains('=')) {
        // to support old data
        aDOFMessage =
          aDOFMessage.right(aDOFMessage.length() - aDOFMessage.lastIndexOf('=')).trimmed();
      }
      myIsFullyConstrained = (aDOFMessage == "0");
    }
  }
}

void PartSet_OverconstraintListener::getCustomColor(const ObjectPtr& theObject,
                                                    std::vector<int>& theColor)
{
  if (!myIsActive)
    return;

  FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
  std::string aFeatureName = aFeature->data()->name();

  if (myConflictingObjects.find(theObject) != myConflictingObjects.end()) {
    theColor = Config_PropManager::color("Visualization", "sketch_overconstraint_color");
  }
  if (myIsFullyConstrained) {
    // only entity features has custom color when sketch is fully constrained
    if (aFeature.get() && PartSet_SketcherMgr::isEntity(aFeature->getKind()) &&
        !PartSet_SketcherMgr::isExternalFeature(aFeature)) {
      PartSet_Module* aModule = module();
      CompositeFeaturePtr aSketch = aModule->sketchMgr()->activeSketch();
      // the given object is sub feature of the current sketch(created or edited)
      if (ModelAPI_Tools::compositeOwner(aFeature) == aSketch)
        theColor = Config_PropManager::color("Visualization", "sketch_fully_constrained_color");
    }
  }
}

void PartSet_OverconstraintListener::processEvent(
                                                 const std::shared_ptr<Events_Message>& theMessage)
{
  // #2271 open document: if sketch has confilcting elements, solver sends message with the
  // elements by opening the document. Sketch is not active, but an internal container should
  // be updated. So, we should not check whether the listener is active here
  //if (!myIsActive)
  //  return;

#ifdef DEBUG_FEATURE_OVERCONSTRAINT_LISTENER
  bool isRepaired = theMessage->eventID() == Events_Loop::eventByName(EVENT_SOLVER_REPAIRED);
  int aCount = 0;

  std::shared_ptr<ModelAPI_SolverFailedMessage> anErrorMsg =
                  std::dynamic_pointer_cast<ModelAPI_SolverFailedMessage>(theMessage);
  QString anInfoStr;
  if (anErrorMsg.get()) {
    const std::set<ObjectPtr>& aConflictingObjects = anErrorMsg->objects();
    aCount = aConflictingObjects.size();
    anInfoStr = getObjectsInfo(aConflictingObjects);
  }

  QString aCurrentInfoStr = getObjectsInfo(myConflictingObjects);

  QString aMsg("PartSet_OverconstraintListener::processEvent: %1,\nobjects "
               "count = %2:%3\ncurrent objects count = %4:%5");
  qDebug(aMsg.arg(isRepaired ? "REPAIRED" : "FAILED")
             .arg(aCount).arg(anInfoStr).arg(myConflictingObjects.size())
             .arg(aCurrentInfoStr).toStdString().c_str());
#endif

  Events_ID anEventID = theMessage->eventID();
  if (anEventID == Events_Loop::eventByName(EVENT_SOLVER_FAILED) ||
      anEventID == Events_Loop::eventByName(EVENT_SOLVER_REPAIRED)) {
    std::shared_ptr<ModelAPI_SolverFailedMessage> anErrorMsg =
                   std::dynamic_pointer_cast<ModelAPI_SolverFailedMessage>(theMessage);
    bool anUpdated = false;
    if (anErrorMsg.get()) {
      const std::set<ObjectPtr>& aConflictingObjects = anErrorMsg->objects();
      if (anEventID == Events_Loop::eventByName(EVENT_SOLVER_FAILED))
        anUpdated = appendConflictingObjects(aConflictingObjects);
      else
        anUpdated = repairConflictingObjects(aConflictingObjects);
    }
  }
  else if (anEventID == Events_Loop::eventByName(EVENT_SKETCH_UNDER_CONSTRAINED) ||
           anEventID == Events_Loop::eventByName(EVENT_SKETCH_FULLY_CONSTRAINED)) {
    bool aPrevFullyConstrained = myIsFullyConstrained;
    myIsFullyConstrained = anEventID == Events_Loop::eventByName(EVENT_SKETCH_FULLY_CONSTRAINED);

    if (aPrevFullyConstrained != myIsFullyConstrained) {
      std::set<ObjectPtr> aModifiedObjects;
      PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
      CompositeFeaturePtr aSketch = aModule->sketchMgr()->activeSketch();

      // check the sketch in the message and the active sketch are the same
      std::shared_ptr<ModelAPI_SolverFailedMessage> anErrorMsg =
          std::dynamic_pointer_cast<ModelAPI_SolverFailedMessage>(theMessage);
      if (aSketch && anErrorMsg && !anErrorMsg->objects().empty()) {
        ObjectPtr anObject = *anErrorMsg->objects().begin();
        CompositeFeaturePtr aSketchFromMsg =
            std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(anObject);
        if (!aSketchFromMsg || aSketchFromMsg != aSketch)
          aSketch = CompositeFeaturePtr();
      }

      if (aSketch.get()) {
        int aNumberOfSubs = aSketch->numberOfSubs();
        for (int i = 0; i < aNumberOfSubs; i++) {
          FeaturePtr aFeature = aSketch->subFeature(i);
          aModifiedObjects.insert(aFeature); // is necessary to redisplay presentations
          std::list<ResultPtr> aResults = aFeature->results();
          for (std::list<ResultPtr>::const_iterator aIt = aResults.begin();
               aIt != aResults.end(); ++aIt) {
            aModifiedObjects.insert(*aIt);
          }
        }
        redisplayObjects(aModifiedObjects);
      }
    }
  }
  else if (anEventID == ModelAPI_EventReentrantMessage::eventId() ||
           anEventID == SketchPlugin_MacroArcReentrantMessage::eventId()) {
    // the message is sent to sketcher reentrant manager only if the name of feature
    // sender is equal to feature name of the current operation. E.g. Horizontal create operation
    // is active. Sketch Line feature is changed, so execute is called, it will send message
    // This Line's message should not be processed, as the reentrant operation is not for Line
    // It is not enoght of kind, the name should be used, e.g. restarted Lines on auxiliary
    // cirlce sometimes causes previous line change, kind the same, but feature line is different
    std::string aCurrentFeatureName;
    ModuleBase_Operation* anOperation =
                XGUI_Tools::workshop(myWorkshop)->operationMgr()->currentOperation();
    if (anOperation) {
      ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                           (anOperation);
      if (aFOperation) {
        FeaturePtr aFeature = aFOperation->feature();
        // data valid is necessary if the feature has been already deleted
        // (e.g. Esc of Lenght if lenght value is modified)
        if (aFeature.get() && aFeature->data()->isValid())
          aCurrentFeatureName = aFeature->data()->name();
      }
    }
    if (theMessage->sender()) {
      ModelAPI_Object* aSender = static_cast<ModelAPI_Object*>(theMessage->sender());
      if (aSender) {
        FeaturePtr aFeatureSender =
          std::dynamic_pointer_cast<ModelAPI_Feature>(aSender->data()->owner());
        if (aFeatureSender.get() && aFeatureSender->data()->name() == aCurrentFeatureName) {
          PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
          PartSet_SketcherReentrantMgr* aReentrantMgr = aModule->sketchReentranceMgr();
          aReentrantMgr->setReentrantMessage(theMessage);
        }
      }
    }
  }

#ifdef DEBUG_FEATURE_OVERCONSTRAINT_LISTENER
  aCurrentInfoStr = getObjectsInfo(myConflictingObjects);
  qDebug(QString("RESULT: current objects count = %1:%2\n")
                .arg(myConflictingObjects.size()).arg(aCurrentInfoStr).toStdString().c_str());
#endif
}

bool PartSet_OverconstraintListener::appendConflictingObjects(
                                               const std::set<ObjectPtr>& theConflictingObjects)
{
  std::set<ObjectPtr> aModifiedObjects;

  // set error state for new objects and append them in the internal map of objects
  std::set<ObjectPtr>::const_iterator
    anIt = theConflictingObjects.begin(), aLast = theConflictingObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (myConflictingObjects.find(anObject) == myConflictingObjects.end()) { // it is not found
      aModifiedObjects.insert(anObject);
      myConflictingObjects.insert(anObject);
    }
  }
  bool isUpdated = !aModifiedObjects.empty();
  if (isUpdated)
    redisplayObjects(aModifiedObjects);

  return isUpdated;
}

bool PartSet_OverconstraintListener::repairConflictingObjects(
                                              const std::set<ObjectPtr>& theConflictingObjects)
{
  std::set<ObjectPtr> aModifiedObjects;
  // erase error state of absent current objects in the parameter list
  std::set<ObjectPtr>::const_iterator anIt, aLast;
  for (anIt = theConflictingObjects.begin(), aLast = theConflictingObjects.end();
       anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (theConflictingObjects.find(anObject) != theConflictingObjects.end()) { // it is found
      myConflictingObjects.erase(anObject);

      aModifiedObjects.insert(anObject);
    }
  }
  bool isUpdated = !aModifiedObjects.empty();
  if (isUpdated)
    redisplayObjects(aModifiedObjects);

  return isUpdated;
}

void PartSet_OverconstraintListener::redisplayObjects(
                                              const std::set<ObjectPtr>& theObjects)
{
  static Events_Loop* aLoop = Events_Loop::loop();

  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_VISUAL_ATTRIBUTES);
  static Events_ID EVENT_REDISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();

  std::set<ObjectPtr>::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr aObj = *anIt;
    aECreator->sendUpdated(aObj, EVENT_DISP);
    aECreator->sendUpdated(aObj, EVENT_REDISP);
  }
  aLoop->flush(EVENT_DISP);
  aLoop->flush(EVENT_REDISP);
}

PartSet_Module* PartSet_OverconstraintListener::module() const
{
  return dynamic_cast<PartSet_Module*>(myWorkshop->module());
}

#ifdef _DEBUG
QString PartSet_OverconstraintListener::getObjectsInfo(const std::set<ObjectPtr>& theObjects)
{
  std::set<ObjectPtr>::const_iterator anIt = theObjects.begin(),
                                      aLast = theObjects.end();
  QStringList anInfo;
  for (; anIt != aLast; ++anIt)
    anInfo.append(ModuleBase_Tools::objectInfo((*anIt)));

  return anInfo.join(";\n");
}
#endif
