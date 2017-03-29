// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Angle.cpp
// Created:     20 August 2015
// Author:      Vitaly SMETANNIKOV

#include <ModelAPI_Tools.h>

#include "PartSet_OverconstraintListener.h"
#include <PartSet_Module.h>
#include <PartSet_SketcherMgr.h>

#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"
#include "XGUI_Displayer.h"
#include "XGUI_CustomPrs.h"

#include "SketcherPrs_SymbolPrs.h"
#include "SketchPlugin_SketchEntity.h"

#include "Events_Loop.h"

#include <GeomAPI_IPresentable.h>
#include <ModelAPI_Events.h>
#include <ModuleBase_Tools.h>

#include <QString>

//#define DEBUG_FEATURE_OVERCONSTRAINT_LISTENER

PartSet_OverconstraintListener::PartSet_OverconstraintListener(ModuleBase_IWorkshop* theWorkshop)
: myWorkshop(theWorkshop)
{
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SOLVER_FAILED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SOLVER_REPAIRED));

  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SKETCH_UNDER_CONSTRAINED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_SKETCH_FULLY_CONSTRAINED));
}

bool PartSet_OverconstraintListener::hasCustomColor(const ObjectPtr& theObject,
                                                    std::vector<int>& theColor)
{
  if (myConflictingObjects.find(theObject) != myConflictingObjects.end()) {
    Quantity_Color aColor = ModuleBase_Tools::color("Visualization",
                                                    "sketch_overconstraint_color");
    theColor.push_back(aColor.Red()*255.);
    theColor.push_back(aColor.Green()*255.);
    theColor.push_back(aColor.Blue()*255.);
    return true;
  }
  if (myIsFullyConstrained) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature.get()) {
      PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
      CompositeFeaturePtr aCompositeFeature = aModule->sketchMgr()->activeSketch();
      // the given object is sub feature of the current sketch(created or edited)
      if (ModelAPI_Tools::compositeOwner(aFeature) == aCompositeFeature) {
        Quantity_Color aColor = ModuleBase_Tools::color("Visualization",
                                                        "sketch_fully_constrained_color");
        theColor.push_back(aColor.Red()*255.);
        theColor.push_back(aColor.Green()*255.);
        theColor.push_back(aColor.Blue()*255.);
        return true;
      }
    }
  }
  return false;
}

void PartSet_OverconstraintListener::processEvent(
                                                 const std::shared_ptr<Events_Message>& theMessage)
{
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
    myIsFullyConstrained = anEventID == Events_Loop::eventByName(EVENT_SKETCH_FULLY_CONSTRAINED);
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
  //std::vector<int> aColor;
  //getConflictingColor(aColor);

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

  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();

  std::set<ObjectPtr>::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
  for (; anIt != aLast; anIt++)
    aECreator->sendUpdated(*anIt, EVENT_DISP);

  aLoop->flush(EVENT_DISP);
}

XGUI_Workshop* PartSet_OverconstraintListener::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
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
