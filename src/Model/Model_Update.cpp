// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Update.cxx
// Created:     25 Jun 2014
// Author:      Mikhail PONIKAROV

#include <Model_Update.h>
#include <Model_Document.h>
#include <Model_Data.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_AttributeSelection.h>
#include <ModelAPI_AttributeSelectionList.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Tools.h>
#include <Events_Loop.h>
#include <Events_LongOp.h>
#include <Events_Error.h>
#include <Config_PropManager.h>

using namespace std;

Model_Update MY_UPDATER_INSTANCE;  /// the only one instance initialized on load of the library

Model_Update::Model_Update()
{
  Events_Loop* aLoop = Events_Loop::loop();
  static const Events_ID kChangedEvent = aLoop->eventByName("PreferenceChanged");
  aLoop->registerListener(this, kChangedEvent);
  static const Events_ID kRebuildEvent = aLoop->eventByName("Rebuild");
  aLoop->registerListener(this, kRebuildEvent);
  static const Events_ID kCreatedEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED);
  aLoop->registerListener(this, kCreatedEvent);
  static const Events_ID kUpdatedEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED);
  aLoop->registerListener(this, kUpdatedEvent);
  static const Events_ID kMovedEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_MOVED);
  aLoop->registerListener(this, kMovedEvent);
  static const Events_ID kOpFinishEvent = aLoop->eventByName("FinishOperation");
  aLoop->registerListener(this, kOpFinishEvent);
  static const Events_ID kOpAbortEvent = aLoop->eventByName("AbortOperation");
  aLoop->registerListener(this, kOpAbortEvent);
  static const Events_ID kOpStartEvent = aLoop->eventByName("StartOperation");
  aLoop->registerListener(this, kOpStartEvent);

  Config_PropManager::registerProp("Model update", "automatic_rebuild", "Rebuild immediately",
                                   Config_Prop::Boolean, "false");
  myIsAutomatic =
    Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
}

void Model_Update::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  static Events_Loop* aLoop = Events_Loop::loop();
  static const Events_ID kChangedEvent = aLoop->eventByName("PreferenceChanged");
  static const Events_ID kRebuildEvent = aLoop->eventByName("Rebuild");
  static const Events_ID kCreatedEvent = aLoop->eventByName(EVENT_OBJECT_CREATED);
  static const Events_ID kUpdatedEvent = aLoop->eventByName(EVENT_OBJECT_UPDATED);
  static const Events_ID kMovedEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_MOVED);
  static const Events_ID kOpFinishEvent = aLoop->eventByName("FinishOperation");
  static const Events_ID kOpAbortEvent = aLoop->eventByName("AbortOperation");
  static const Events_ID kOpStartEvent = aLoop->eventByName("StartOperation");
  bool isOperationChanged = false;
  if (theMessage->eventID() == kChangedEvent) { // automatic and manual rebuild flag is changed
    bool aPropVal =
      Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
    if (aPropVal != myIsAutomatic) { // something is changed
      myIsAutomatic = aPropVal;
      if (myIsAutomatic) // higher level of automatization => to rebuild
        processOperation(false);
    }
  } else if (theMessage->eventID() == kRebuildEvent) { // the rebuild command
    processOperation(true);
  } else if (theMessage->eventID() == kCreatedEvent || theMessage->eventID() == kUpdatedEvent ||
             theMessage->eventID() == kMovedEvent) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    const std::set<ObjectPtr>& anObjs = aMsg->objects();
    std::set<ObjectPtr>::const_iterator anObjIter = anObjs.cbegin();
    for(; anObjIter != anObjs.cend(); anObjIter++) {
      // created objects are always must be up to date (python box feature)
      // and updated not in internal uptation chain
      if (theMessage->eventID() == kCreatedEvent) {
        myJustCreated.insert(*anObjIter);
      } else if (myJustCreated.find(*anObjIter) == myJustCreated.end()) { // moved and updated
        myJustUpdated.insert(*anObjIter);
      }
    }
     // this event is for solver update, not here, do not react immideately
    if (!(theMessage->eventID() == kMovedEvent))
      processOperation(false);
  } else if (theMessage->eventID() == kOpStartEvent) {
    // we don't need the update only on operation start (caused problems in PartSet_Listener::processEvent)
    isOperationChanged = true;
  } else if (theMessage->eventID() == kOpFinishEvent || theMessage->eventID() == kOpAbortEvent) {
    processOperation(true);
    isOperationChanged = true;
  }
  if (isOperationChanged) {
    // remove all macros before clearing all created
    std::set<ObjectPtr>::iterator aCreatedIter = myJustCreated.begin();
    for(; aCreatedIter != myJustCreated.end(); aCreatedIter++) {
      FeaturePtr aFeature = 
        std::dynamic_pointer_cast<ModelAPI_Feature>(*aCreatedIter);
      if (aFeature.get() && aFeature->isMacro()) {
        aFeature->document()->removeFeature(aFeature);
      }
    }
    myJustCreated.clear();
    myJustUpdated.clear();
  }
}

void Model_Update::processOperation(const bool theTotalUpdate)
{
  // the hardcode (DBC asked): hide the sketch referenced by extrusion on apply
  std::set<std::shared_ptr<ModelAPI_Object> >::iterator aFIter;
  for(aFIter = myJustCreated.begin(); aFIter != myJustCreated.end(); aFIter++)
  {
    FeaturePtr aF = std::dynamic_pointer_cast<ModelAPI_Feature>(*aFIter);
    if (aF && aF->data().get() && aF->getKind() == "Extrusion") {
      AttributeSelectionListPtr aBase = aF->selectionList("base");
      if (aBase.get()) {
        for(int a = aBase->size() - 1; a >= 0; a--) {
          ResultPtr aSketchRes = aBase->value(a)->context();
          if (aSketchRes) {
            static Events_ID HIDE_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TOHIDE);
            ModelAPI_EventCreator::get()->sendUpdated(aSketchRes, HIDE_DISP);
          }
        }
      }
    }
  }
  // perform update of everything if needed
  if (!myIsExecuted) {
    myIsExecuted = true;

    bool isAutomaticChanged = false;

    if (theTotalUpdate && !myIsAutomatic) { // Apply button now works as "Rebuild"
      isAutomaticChanged = true;
      myIsAutomatic = true;
    }

    updateInDoc(ModelAPI_Session::get()->moduleDocument());

    if (isAutomaticChanged) myIsAutomatic = false;
    myIsExecuted = false;

    // flush to update display
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    aLoop->flush(EVENT_DISP);
  }
}

void Model_Update::updateInDoc(std::shared_ptr<ModelAPI_Document> theDoc)
{
  std::set<FeaturePtr> alreadyProcessed; // features that are processed before others
  // all features one by one
  for (int aFIndex = 0; aFIndex < theDoc->size(ModelAPI_Feature::group(), true); aFIndex++) {
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(
      theDoc->object(ModelAPI_Feature::group(), aFIndex, true));
    if (aFeature && alreadyProcessed.find(aFeature) == alreadyProcessed.end()) {
      // update selection and parameters attributes first, before sub-features analysis (sketch plane)
      updateArguments(aFeature);
      // composite feature must be executed after sub-features execution
      CompositeFeaturePtr aComposite = 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
      if (aComposite) {
        // number of subs can be changed in execution: like fillet
        for(int a = 0; a < aComposite->numberOfSubs(); a++) {
          FeaturePtr aSub = aComposite->subFeature(a);
          updateFeature(aSub);
          alreadyProcessed.insert(aSub);
        }
      }

      updateFeature(aFeature);
      // update the document results recursively
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aRIter);
        if (aPart.get()) {
          if (aPart->isActivated()) {
            updateInDoc(aPart->partDoc());
          }
        }
      }
    }
  }
}

void Model_Update::redisplayWithResults(FeaturePtr theFeature, const ModelAPI_ExecState theState) 
{
  // make updated and redisplay all results
  static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
  std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
  for (; aRIter != aResults.cend(); aRIter++) {
    std::shared_ptr<ModelAPI_Result> aRes = *aRIter;
    aRes->data()->execState(theState);
    myJustUpdated.insert(aRes);
    ModelAPI_EventCreator::get()->sendUpdated(aRes, EVENT_DISP);
  }
  // to redisplay "presentable" feature (for ex. distance constraint)
  ModelAPI_EventCreator::get()->sendUpdated(theFeature, EVENT_DISP);
  theFeature->data()->execState(theState);
}

/// Updates the state by the referenced object: if something bad with it, set state for this one
ModelAPI_ExecState stateByReference(ObjectPtr theTarget, const ModelAPI_ExecState theCurrent)
{
  if (theTarget) {
    ModelAPI_ExecState aRefState = theTarget->data()->execState();
    if (aRefState == ModelAPI_StateMustBeUpdated) {
      return ModelAPI_StateMustBeUpdated;
    } else if (aRefState != ModelAPI_StateDone) {
      return ModelAPI_StateInvalidArgument;
    }
  }
  return theCurrent;
}

void Model_Update::updateArguments(FeaturePtr theFeature) {
  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();

  bool aJustUpdated = false;
  ModelAPI_ExecState aState = ModelAPI_StateDone;
  // check the parameters: values can be changed
  std::list<AttributePtr> aDoubles = 
    theFeature->data()->attributes(ModelAPI_AttributeDouble::typeId()); 
  std::list<AttributePtr>::iterator aDoubleIter = aDoubles.begin();
  for(; aDoubleIter != aDoubles.end(); aDoubleIter++) {
    AttributeDoublePtr aDouble = 
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(*aDoubleIter);
    if (aDouble.get() && !aDouble->text().empty()) {
      double aNewVal;
      if (ModelAPI_Tools::findVariable(aDouble->text(), aNewVal)) {
        if (aNewVal != aDouble->value()) {
          aDouble->setValue(aNewVal);
          aJustUpdated = true;
        }
      } else {
        aState = ModelAPI_StateInvalidArgument;
      }
    }
  }

  if (aState == ModelAPI_StateDone) {// all referenced objects are ready to be used
    //std::cout<<"Execute feature "<<theFeature->getKind()<<std::endl;
    // before execution update the selection attributes if any
    list<AttributePtr> aRefs = 
      theFeature->data()->attributes(ModelAPI_AttributeSelection::typeId());
    list<AttributePtr>::iterator aRefsIter = aRefs.begin();
    for (; aRefsIter != aRefs.end(); aRefsIter++) {
      std::shared_ptr<ModelAPI_AttributeSelection> aSel =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*aRefsIter);
      if (!aSel->update()) { // this must be done on execution since it may be long operation
        if (!aFactory->isNotObligatory(theFeature->getKind(), theFeature->data()->id(aSel)) &&
            aFactory->isCase(theFeature, theFeature->data()->id(aSel)))
          aState = ModelAPI_StateInvalidArgument;
      }
    }
    aRefs = theFeature->data()->attributes(ModelAPI_AttributeSelectionList::typeId());
    for (aRefsIter = aRefs.begin(); aRefsIter != aRefs.end(); aRefsIter++) {
      std::shared_ptr<ModelAPI_AttributeSelectionList> aSel =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(*aRefsIter);
      for(int a = aSel->size() - 1; a >= 0; a--) {
        std::shared_ptr<ModelAPI_AttributeSelection> aSelAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(aSel->value(a));
        if (aSelAttr) {
          if (!aSelAttr->update()) {
            if (!aFactory->isNotObligatory(
                  theFeature->getKind(), theFeature->data()->id(aSel)) &&
                aFactory->isCase(theFeature, theFeature->data()->id(aSel)))
              aState = ModelAPI_StateInvalidArgument;
          }
        }
      }
    }
  }
  if (aJustUpdated && myJustCreated.find(theFeature) == myJustCreated.end())
  myJustUpdated.insert(theFeature);
  if (aState != ModelAPI_StateDone)
    theFeature->data()->execState(aState);
}

void Model_Update::updateFeature(FeaturePtr theFeature)
{
  // check all features this feature depended on (recursive call of updateFeature)
  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
  bool aJustUpdated = false;

  if (theFeature) {
    if (theFeature->data()->execState() != ModelAPI_StateDone)
      aJustUpdated = true;

    ModelAPI_ExecState aState = ModelAPI_StateDone;

    // check all references: if referenced objects are updated, this object also must be updated
    // also check state of referenced objects: if they are not ready, inherit corresponding state
    std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
    std::shared_ptr<Model_Data> aData = 
      std::dynamic_pointer_cast<Model_Data>(theFeature->data());
    aData->referencesToObjects(aRefs);
    std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRef = aRefs.begin();
    for(; aRef != aRefs.end(); aRef++) {
      std::list<ObjectPtr>::iterator aRefObj = aRef->second.begin();
      for(; aRefObj != aRef->second.end(); aRefObj++) {
        if (myJustCreated.find(*aRefObj) != myJustCreated.end() ||
            myJustUpdated.find(*aRefObj) != myJustUpdated.end()) {
          aJustUpdated = true;
        }
        aState = stateByReference(*aRefObj, aState);
      }
    }

    // some arguments were changed, so, this feature must be updated
    if (myJustCreated.find(theFeature) != myJustCreated.end()) {
      aJustUpdated = true;
    } else {
      if (aJustUpdated) {
        if (myJustUpdated.find(theFeature) == myJustUpdated.end())
          myJustUpdated.insert(theFeature);
      } else {
        aJustUpdated = myJustUpdated.find(theFeature) != myJustUpdated.end();
      }
    }
    //std::cout<<"Update feature "<<theFeature->getKind()<<" must be updated = "<<aMustbeUpdated<<std::endl;
    // execute feature if it must be updated
    if (aJustUpdated) {
      if (std::dynamic_pointer_cast<Model_Document>(theFeature->document())->executeFeatures() ||
          !theFeature->isPersistentResult()) {
        if (aFactory->validate(theFeature)) {
          if (myIsAutomatic || 
              (myJustCreated.find(theFeature) != myJustCreated.end() ||
              !theFeature->isPersistentResult() /* execute quick, not persistent results */))
          {
            // execute in try-catch to avoid internal problems of the feature
            if (aState == ModelAPI_StateDone || aState == ModelAPI_StateMustBeUpdated) {
              theFeature->data()->execState(ModelAPI_StateDone);
              try {
                theFeature->execute();
                if (theFeature->data()->execState() != ModelAPI_StateDone) {
                  aState = ModelAPI_StateExecFailed;
                } else {
                  aState = ModelAPI_StateDone;
                }
              } catch(...) {
                aState = ModelAPI_StateExecFailed;
                Events_Error::send(
                  "Feature " + theFeature->getKind() + " has failed during the execution");
              }
            }
            if (aState != ModelAPI_StateDone) {
              theFeature->eraseResults();
            }
            redisplayWithResults(theFeature, aState);
          } else { // must be updatet, but not updated yet
            theFeature->data()->execState(ModelAPI_StateMustBeUpdated);
            const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
            std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
            for (; aRIter != aResults.cend(); aRIter++) {
              std::shared_ptr<ModelAPI_Result> aRes = *aRIter;
              aRes->data()->execState(ModelAPI_StateMustBeUpdated);
            }
          }
        } else {
          theFeature->eraseResults();
          redisplayWithResults(theFeature, ModelAPI_StateInvalidArgument); // result also must be updated
        }
      } else { // for automatically updated features (on abort, etc) it is necessary to redisplay anyway
        redisplayWithResults(theFeature, ModelAPI_StateNothing);
      }
    }
  }
}
