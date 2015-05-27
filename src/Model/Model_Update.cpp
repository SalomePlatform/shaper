// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Update.cxx
// Created:     25 Jun 2014
// Author:      Mikhail PONIKAROV

#include <Model_Update.h>
#include <Model_Document.h>
#include <Model_Data.h>
#include <Model_Objects.h>
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
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Point2D.h>
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
    bool isOnlyResults = true; // check that only results were changed: only redisplay is needed
    for(; anObjIter != anObjs.cend(); anObjIter++) {
      if (!std::dynamic_pointer_cast<ModelAPI_Result>(*anObjIter).get()) {
        isOnlyResults = false;
      }
      // created objects are always must be up to date (python box feature)
      // and updated not in internal uptation chain
      if (theMessage->eventID() == kCreatedEvent) {
        myJustCreated.insert(*anObjIter);
      } else if (myJustCreated.find(*anObjIter) == myJustCreated.end()) { // moved and updated
        myJustUpdated.insert(*anObjIter);
      }
    }
     // this event is for solver update, not here, do not react immideately
    if (!isOnlyResults && !(theMessage->eventID() == kMovedEvent))
      processOperation(false);
  } else if (theMessage->eventID() == kOpStartEvent) {
    // we don't need the update only on operation start (caused problems in PartSet_Listener::processEvent)
    isOperationChanged = true;
  } else if (theMessage->eventID() == kOpFinishEvent || theMessage->eventID() == kOpAbortEvent) {
    processOperation(true, theMessage->eventID() == kOpFinishEvent);
    isOperationChanged = true;
  }
  if (isOperationChanged) {
    // remove all macros before clearing all created and execute all not-previewed
    std::set<ObjectPtr>::iterator aCreatedIter = myJustCreated.begin();
    std::set<ObjectPtr>::iterator anUpdatedIter = myJustUpdated.begin();
    for(; aCreatedIter != myJustCreated.end() || anUpdatedIter != myJustUpdated.end();
      aCreatedIter == myJustCreated.end() ? anUpdatedIter++ : aCreatedIter++) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*
        (aCreatedIter == myJustCreated.end() ? anUpdatedIter : aCreatedIter));
      if (aFeature.get()) {
        // execute not-previewed feature on "apply"
        if (!aFeature->isPreviewNeeded() && (myJustCreated.find(aFeature) != myJustCreated.end() ||
          myJustUpdated.find(aFeature) != myJustUpdated.end())) {
          static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
          if (aFactory->validate(aFeature)) {
            executeFeature(aFeature);
          }
        }
        // remove macro on apply
        if (aFeature->isMacro()) {
          aFeature->document()->removeFeature(aFeature);
        }
      }
    }
    myJustCreated.clear();
    myJustUpdated.clear();
  }
}

void Model_Update::processOperation(const bool theTotalUpdate, const bool theFinish)
{
  if (theFinish) {
    // the hardcode (DBC asked): hide the sketch referenced by extrusion on apply
    std::set<std::shared_ptr<ModelAPI_Object> >::iterator aFIter;
    for(aFIter = myJustCreated.begin(); aFIter != myJustCreated.end(); aFIter++)
    {
      FeaturePtr aF = std::dynamic_pointer_cast<ModelAPI_Feature>(*aFIter);
      if (aF && aF->data()->isValid() && aF->getKind() == "Extrusion") {
        AttributeSelectionListPtr aBase = aF->selectionList("base");
        if (aBase.get()) {
          for(int a = aBase->size() - 1; a >= 0; a--) {
            ResultPtr aSketchRes = aBase->value(a)->context();
            if (aSketchRes) {
              aSketchRes->setDisplayed(false);
            }
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
  Model_Objects* anObjs = std::dynamic_pointer_cast<Model_Document>(theDoc)->objects();
  if (!anObjs) return;
  FeaturePtr aFeatureIter = anObjs->firstFeature();
  for (; aFeatureIter.get(); aFeatureIter = anObjs->nextFeature(aFeatureIter)) {
    if (!aFeatureIter->data()->isValid()) // this may be on close of the document
      continue;
    if (aFeatureIter && alreadyProcessed.find(aFeatureIter) == alreadyProcessed.end()) {
      // update selection and parameters attributes first, before sub-features analysis (sketch plane)
      updateArguments(aFeatureIter);
      // composite feature must be executed after sub-features execution
      CompositeFeaturePtr aComposite = 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeatureIter);
      if (aComposite) {
        // number of subs can be changed in execution: like fillet
        for(int a = 0; a < aComposite->numberOfSubs(); a++) {
          FeaturePtr aSub = aComposite->subFeature(a);
          updateArguments(aSub);
          updateFeature(aSub);
          alreadyProcessed.insert(aSub);
        }
      }

      updateFeature(aFeatureIter);
      // update the document results recursively
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeatureIter->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aRIter);
        if (aPart.get()) {
          if (!aPart->isDisabled() && aPart->isActivated()) {
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
  // perform this method also for disabled features: to make "not done" state for
  // featuers referenced to the active and modified features

  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();

  bool aJustUpdated = false;
  ModelAPI_ExecState aState = theFeature->data()->execState();
  if (aState == ModelAPI_StateInvalidArgument) // a chance to be corrected
    aState = ModelAPI_StateMustBeUpdated;
  // check the parameters state
  // Double
  std::list<AttributePtr> aDoubles =
    theFeature->data()->attributes(ModelAPI_AttributeDouble::typeId());
  std::list<AttributePtr>::iterator aDoubleIter = aDoubles.begin();
  for(; aDoubleIter != aDoubles.end(); aDoubleIter++) {
    AttributeDoublePtr aDouble =
      std::dynamic_pointer_cast<ModelAPI_AttributeDouble>(*aDoubleIter);
    if (aDouble.get() && !aDouble->text().empty()) {
      if (aDouble->expressionInvalid()) {
        aState = ModelAPI_StateInvalidArgument;
      }
    }
  }
  // Point
  {
    std::list<AttributePtr> anAttributes =
      theFeature->data()->attributes(GeomDataAPI_Point::typeId());
    std::list<AttributePtr>::iterator anIter = anAttributes.begin();
    for(; anIter != anAttributes.end(); anIter++) {
      AttributePointPtr aPointAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(*anIter);
      if (aPointAttribute.get()) {
        if ((!aPointAttribute->textX().empty() && aPointAttribute->expressionInvalid(0)) ||
            (!aPointAttribute->textY().empty() && aPointAttribute->expressionInvalid(1)) ||
            (!aPointAttribute->textZ().empty() && aPointAttribute->expressionInvalid(2)))
          aState = ModelAPI_StateInvalidArgument;
      }
    }
  }
  // Point2D
  {
    std::list<AttributePtr> anAttributes =
      theFeature->data()->attributes(GeomDataAPI_Point2D::typeId());
    std::list<AttributePtr>::iterator anIter = anAttributes.begin();
    for(; anIter != anAttributes.end(); anIter++) {
      AttributePoint2DPtr aPoint2DAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point2D>(*anIter);
      if (aPoint2DAttribute.get()) {
        if ((!aPoint2DAttribute->textX().empty() && aPoint2DAttribute->expressionInvalid(0)) ||
            (!aPoint2DAttribute->textY().empty() && aPoint2DAttribute->expressionInvalid(1)))
          aState = ModelAPI_StateInvalidArgument;
      }
    }
  }

  //if (aState == ModelAPI_StateDone) {// all referenced objects are ready to be used
    //std::cout<<"Execute feature "<<theFeature->getKind()<<std::endl;
    // before execution update the selection attributes if any
    list<AttributePtr> aRefs = 
      theFeature->data()->attributes(ModelAPI_AttributeSelection::typeId());
    list<AttributePtr>::iterator aRefsIter = aRefs.begin();
    for (; aRefsIter != aRefs.end(); aRefsIter++) {
      std::shared_ptr<ModelAPI_AttributeSelection> aSel =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*aRefsIter);
      ObjectPtr aContext = aSel->context();
      // update argument onlt if the referenced object is changed
      if (aContext.get() && isUpdated(aContext) && !aContext->isDisabled()) {
        if (aState == ModelAPI_StateDone)
          aState = ModelAPI_StateMustBeUpdated;
        if (!aSel->update()) { // this must be done on execution since it may be long operation
          if (!aFactory->isNotObligatory(theFeature->getKind(), theFeature->data()->id(aSel)) &&
              aFactory->isCase(theFeature, theFeature->data()->id(aSel)))
            aState = ModelAPI_StateInvalidArgument;
        }
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
          ObjectPtr aContext = aSelAttr->context();
          // update argument onlt if the referenced object is changed
          if (aContext.get() && isUpdated(aContext) && !aContext->isDisabled()) {
            if (aState == ModelAPI_StateDone)
              aState = ModelAPI_StateMustBeUpdated;
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
  //}
  if (aJustUpdated && myJustCreated.find(theFeature) == myJustCreated.end())
  myJustUpdated.insert(theFeature);
  if (aState != ModelAPI_StateDone)
    theFeature->data()->execState(aState);
}

void Model_Update::updateFeature(FeaturePtr theFeature)
{
  // check all features this feature depended on (recursive call of updateFeature)
  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();

  if (theFeature->isDisabled() ||  // nothing to do with disabled feature
      theFeature->data()->execState() == ModelAPI_StateInvalidArgument)
    return;
  bool aJustUpdated = false;

  if (theFeature) {
    if (myIsAutomatic && theFeature->data()->execState() == ModelAPI_StateMustBeUpdated)
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
      if (theFeature->isPreviewNeeded()) {
        if (std::dynamic_pointer_cast<Model_Document>(theFeature->document())->executeFeatures() ||
            !theFeature->isPersistentResult()) {
          if (aFactory->validate(theFeature)) {
            FeaturePtr aCurrent = theFeature->document()->currentFeature(false);
            if (myIsAutomatic || !theFeature->isPersistentResult() /* execute quick, not persistent results */
                || (isUpdated(theFeature) && 
                     (theFeature == aCurrent || 
                      (aCurrent.get() && aCurrent->isMacro())))) // currently edited
            {
              if (aState == ModelAPI_StateDone || aState == ModelAPI_StateMustBeUpdated) {
                executeFeature(theFeature);
              }
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
      } else { // preview is not needed => make state Done
        if (theFeature->data()->execState() == ModelAPI_StateMustBeUpdated)
          theFeature->data()->execState(ModelAPI_StateDone);
      }
    }
  }
}

void Model_Update::executeFeature(FeaturePtr theFeature)
{
  // execute in try-catch to avoid internal problems of the feature
  ModelAPI_ExecState aState = ModelAPI_StateDone;
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
  if (aState != ModelAPI_StateDone) {
    theFeature->eraseResults();
  }
  redisplayWithResults(theFeature, aState);
}

bool Model_Update::isUpdated(const ObjectPtr& theObj)
{
  return myJustCreated.find(theObj) != myJustCreated.end() ||
         myJustUpdated.find(theObj) != myJustUpdated.end();
}
