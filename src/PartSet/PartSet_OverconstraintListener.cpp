// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        SketcherPrs_Angle.cpp
// Created:     20 August 2015
// Author:      Vitaly SMETANNIKOV

#include "PartSet_OverconstraintListener.h"

#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"
#include "XGUI_Displayer.h"

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
}

bool PartSet_OverconstraintListener::isConflictingObject(const ObjectPtr& theObject)
{
  return myConflictingObjects.find(theObject) != myConflictingObjects.end();
}

void PartSet_OverconstraintListener::getConflictingColor(std::vector<int>& theColor)
{
  Quantity_Color aColor = ModuleBase_Tools::color("Visualization", "sketch_overconstraint_color",
                                                  SKETCH_OVERCONSTRAINT_COLOR);

  theColor.push_back(aColor.Red()*255.);
  theColor.push_back(aColor.Green()*255.);
  theColor.push_back(aColor.Blue()*255.);
}

void PartSet_OverconstraintListener::processEvent(
                                                 const std::shared_ptr<Events_Message>& theMessage)
{

#ifdef DEBUG_FEATURE_OVERCONSTRAINT_LISTENER
  bool isRepaired = theMessage->eventID() == Events_Loop::eventByName(EVENT_SOLVER_REPAIRED);
  qDebug(QString("PartSet_OverconstraintListener::processEvent:\n %1").arg(isRepaired ? "REPAIRED" : "FAILED").toStdString().c_str());
#endif

  if (theMessage->eventID() == Events_Loop::eventByName(EVENT_SOLVER_FAILED) ||
             theMessage->eventID() == Events_Loop::eventByName(EVENT_SOLVER_REPAIRED)) {
    std::shared_ptr<ModelAPI_SolverFailedMessage> anErrorMsg =
                   std::dynamic_pointer_cast<ModelAPI_SolverFailedMessage>(theMessage);
    bool anUpdated = false;
    if (anErrorMsg.get()) {
      const std::set<ObjectPtr>& aConflictingObjects = anErrorMsg->objects();
      anUpdated = updateConflictingObjects(aConflictingObjects);
    }
    else
      anUpdated = updateConflictingObjects(std::set<ObjectPtr>());
  }
}

bool PartSet_OverconstraintListener::updateConflictingObjects(
                                                  const std::set<ObjectPtr>& theConflictingObjects)
{
  std::set<ObjectPtr>::const_iterator anIt, aLast;

#ifdef DEBUG_FEATURE_OVERCONSTRAINT_LISTENER
  anIt = theConflictingObjects.begin();
  aLast = theConflictingObjects.end();

  QStringList anInfo;
  for (; anIt != aLast; ++anIt) {
    anInfo.append(ModuleBase_Tools::objectInfo((*anIt)));
  }
  QString anInfoStr = anInfo.join(";\n");
  qDebug(QString("PartSet_OverconstraintListener::updateConflictingObjects: %1: \n%2").arg(theConflictingObjects.size())
                                                                                    .arg(anInfoStr).toStdString().c_str());
#endif

  bool isUpdated = false;
  std::set<ObjectPtr> aModifiedObjects;
  // erase error state of absent current objects in the parameter list
  for (anIt = myConflictingObjects.begin(), aLast = myConflictingObjects.end() ; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (theConflictingObjects.find(anObject) == theConflictingObjects.end()) { // it is not found
      setConflictingObject(anObject, false);
      aModifiedObjects.insert(anObject);
    }
  }

  // erase absent objects from the internal container
  for (anIt = aModifiedObjects.begin(), aLast = aModifiedObjects.end(); anIt != aLast; anIt++) {
    myConflictingObjects.erase(*anIt);
  }

  // set error state for new objects and append them in the internal map of objects
  for (anIt = theConflictingObjects.begin(), aLast = theConflictingObjects.end() ; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    if (myConflictingObjects.find(anObject) == myConflictingObjects.end()) { // it is not found
      setConflictingObject(anObject, true);
      aModifiedObjects.insert(anObject);
      myConflictingObjects.insert(anObject);
    }
  }
  isUpdated = !aModifiedObjects.empty();
  if (isUpdated)
    redisplayObjects(aModifiedObjects);

  return isUpdated;
}

void PartSet_OverconstraintListener::redisplayObjects(
                                              const std::set<ObjectPtr>& theObjects)
{
/*static Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_UPDATED);

  static const ModelAPI_EventCreator* aECreator = ModelAPI_EventCreator::get();

  std::set<ObjectPtr>::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
  for (; anIt != aLast; anIt++) {
    aECreator->sendUpdated(*anIt, EVENT_DISP);

    //#ifdef DEBUG_FEATURE_OVERCONSTRAINT_LISTENER
    //  QString anInfoStr = ModuleBase_Tools::objectInfo(*anIt);
    //  qDebug(QString("PartSet_OverconstraintListener::SEND UPDATED: %1").arg(anInfoStr).toStdString().c_str());
    //#endif
  }
  aLoop->flush(EVENT_DISP);*/

  XGUI_Displayer* aDisplayer = workshop()->displayer();
  bool aHidden;
  std::set<ObjectPtr>::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
  for (; anIt != aLast; anIt++) {
    ObjectPtr anObject = *anIt;
    aHidden = !anObject->data() || !anObject->data()->isValid() || 
               anObject->isDisabled() || (!anObject->isDisplayed());
    if (!aHidden)
      aDisplayer->redisplay(anObject, false);
  }
  aDisplayer->updateViewer();
}

void PartSet_OverconstraintListener::setConflictingObject(const ObjectPtr& theObject,
                                                          const bool theConflicting)
{
  AISObjectPtr anAISObject;
  GeomPresentablePtr aPrs = std::dynamic_pointer_cast<GeomAPI_IPresentable>(theObject);

  if (aPrs.get() != NULL) {
    XGUI_Workshop* aWorkshop = workshop();
    XGUI_Displayer* aDisplayer = aWorkshop->displayer();

    anAISObject = aPrs->getAISObject(aDisplayer->getAISObject(theObject));
    if (anAISObject.get()) {
      Handle(AIS_InteractiveObject) anAISIO = anAISObject->impl<Handle(AIS_InteractiveObject)>();
      if (!anAISIO.IsNull()) {
        if (!Handle(SketcherPrs_SymbolPrs)::DownCast(anAISIO).IsNull()) {
          Handle(SketcherPrs_SymbolPrs) aPrs = Handle(SketcherPrs_SymbolPrs)::DownCast(anAISIO);
          if (!aPrs.IsNull())
            aPrs->SetConflictingConstraint(theConflicting);
        }
      }
    }
  }
}

XGUI_Workshop* PartSet_OverconstraintListener::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}

