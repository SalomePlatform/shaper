// File:        PartSet_Listener.h
// Created:     28 Apr 2014
// Author:      Natalia ERMOLAEVA

#include <PartSet_Listener.h>

#include <PartSet_Module.h>
#include <PartSet_OperationSketchBase.h>
#include <PartSet_OperationSketch.h>
#include <PartSet_OperationFeatureCreate.h>

#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>
#include <XGUI_OperationMgr.h>

#include <ModuleBase_Operation.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_Feature.h>

#ifdef _DEBUG
#include <QDebug>
#endif

using namespace std;

PartSet_Listener::PartSet_Listener(PartSet_Module* theModule)
    : myModule(theModule)
{
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_DELETED));
}

PartSet_Listener::~PartSet_Listener()
{
}

//******************************************************
void PartSet_Listener::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  ModuleBase_Operation* anOperation = myModule->xWorkshop()->operationMgr()->currentOperation();
  PartSet_OperationSketchBase* aSketchOp = dynamic_cast<PartSet_OperationSketchBase*>(anOperation);
  if (!aSketchOp)
    return;

  XGUI_Displayer* aDisplayer = myModule->xWorkshop()->displayer();
  QString aType = QString(theMessage->eventID().eventText());
  if (aType == EVENT_OBJECT_CREATED) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    std::set<ObjectPtr> aFeatures = aUpdMsg->objects();

    PartSet_OperationSketch* aSketchOp = 
      dynamic_cast<PartSet_OperationSketch*>(myModule->xWorkshop()->operationMgr()->currentOperation());

    std::set<ObjectPtr>::const_iterator anIt = aFeatures.begin(), aLast = aFeatures.end();
    for (; anIt != aLast; anIt++) {
      ObjectPtr aObj = (*anIt);
      // If current operation is Sketch then there is no active sketching operation
      // and possible the object was created by Redo operation
      if (aSketchOp) {
          XGUI_Displayer* aDisplayer = myModule->xWorkshop()->displayer();
          // Very possible it is not displayed
          aDisplayer->display(aObj, false);
      }
    }
  } else if (aType == EVENT_OBJECT_TO_REDISPLAY) {
    PartSet_OperationFeatureCreate* aCreationOp = 
      dynamic_cast<PartSet_OperationFeatureCreate*>
      (myModule->xWorkshop()->operationMgr()->currentOperation());
    if (aCreationOp) {
      // Deactivate currently creating objects for selection
      XGUI_Displayer* aDisplayer = myModule->xWorkshop()->displayer();
      FeaturePtr aFeature = aCreationOp->feature();
      const std::list<ResultPtr>& aResults = aFeature->results();
      std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aUpdMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);

      std::set<ObjectPtr> aFeatures = aUpdMsg->objects();
      std::set<ObjectPtr>::const_iterator aObjIt, aNoObj = aFeatures.cend();
      std::list<ResultPtr>::const_iterator anIt = aResults.begin(), aLast = aResults.end();
      for (; anIt != aLast; anIt++) {
        aObjIt = aFeatures.find(*anIt);
        if (aObjIt != aNoObj) {
          aDisplayer->deactivate(*aObjIt);
        }
      }
    }
  } else if (aType == EVENT_OBJECT_DELETED) {
    std::shared_ptr<ModelAPI_ObjectDeletedMessage> aDelMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectDeletedMessage>(theMessage);
    std::shared_ptr<ModelAPI_Document> aDoc = aDelMsg->document();

    std::set<std::string> aGroups = aDelMsg->groups();
    std::set<std::string>::const_iterator anIt = aGroups.begin(), aLast = aGroups.end();
    for (; anIt != aLast; anIt++) {
      std::string aGroup = *anIt;
      if (aGroup.compare(SketchPlugin_Sketch::ID()) == 0) {  // Update only Sketch group
        myModule->xWorkshop()->displayer()->eraseDeletedResults();
        myModule->updateCurrentPreview(aGroup);
      }
    }
  }
}
