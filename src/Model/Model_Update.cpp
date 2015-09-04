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
//#define DEB_UPDATE

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
  myIsAutomatic = true;
  //  Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
  myIsParamUpdated = false;
  myIsFinish = false;
  myModification = 0;
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
#ifdef DEB_UPDATE
  std::cout<<"****** Event "<<theMessage->eventID().eventText()<<std::endl;
#endif
  if (theMessage->eventID() == kChangedEvent) { // automatic and manual rebuild flag is changed
    bool aPropVal =
      Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
    if (aPropVal != myIsAutomatic) { // something is changed
      // myIsAutomatic = aPropVal;
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
      if ((*anObjIter)->groupName() == ModelAPI_ResultParameter::group()) {
        myIsParamUpdated = true;
      }
      // on undo/redo, abort do not update persisten features
      FeaturePtr anUpdated = std::dynamic_pointer_cast<ModelAPI_Feature>(*anObjIter);
      if (std::dynamic_pointer_cast<Model_Document>((*anObjIter)->document())->executeFeatures() ||
          (anUpdated.get() && !anUpdated->isPersistentResult())) {
        // created objects are always must be up to date (python box feature)
        // and updated not in internal uptation chain
        myUpdated[*anObjIter] = myModification;

        // something is updated during the execution: re-execute it (sketch update by parameters or
        // Box macro that updates the upper features during the execution)
        if (myIsExecuted) { 
          FeaturePtr anUpdated = std::dynamic_pointer_cast<ModelAPI_Feature>(*anObjIter);
          if (anUpdated.get() &&  anUpdated->data()->isValid())
            iterateUpdateBreak(anUpdated);
        }
#ifdef DEB_UPDATE
        if (myIsExecuted) std::cout<<"During execution ";
        if ((*anObjIter)->data() && (*anObjIter)->data()->isValid()) {
          std::cout<<"add updated "<<(*anObjIter)->groupName()<<" "
            <<(*anObjIter)->data()->name()<<std::endl;
        }
#endif
      }

    }
    // this event is for solver update, not here, do not react immediately
    if (!isOnlyResults && !(theMessage->eventID() == kMovedEvent))
      processOperation(false);
  } else if (theMessage->eventID() == kOpFinishEvent || theMessage->eventID() == kOpAbortEvent ||
      theMessage->eventID() == kOpStartEvent) {

    if (!(theMessage->eventID() == kOpStartEvent)) {
      myIsFinish = true;
      processOperation(true, theMessage->eventID() == kOpFinishEvent);
      myIsFinish = false;
    }
    // remove all macros before clearing all created
    std::set<ObjectPtr>::iterator anUpdatedIter = myWaitForFinish.begin();
    while(anUpdatedIter != myWaitForFinish.end()) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(*anUpdatedIter);
      if (aFeature.get()) {
        // remove macro on finish
        if (aFeature->isMacro()) {
          aFeature->document()->removeFeature(aFeature);
          myWaitForFinish.erase(aFeature);
        }
        // to avoid the map update problems on "remove"
        if (myWaitForFinish.find(aFeature) == myWaitForFinish.end()) {
          anUpdatedIter = myWaitForFinish.begin();
        } else {
          anUpdatedIter++;
        }
      } else {
        anUpdatedIter++;
      }
    }
    // in the end of transaction everything is updated, so clear the old objects (the only one
    // place where results are cleared)
    myIsParamUpdated = false;
    myUpdated.clear();
    myModification = 0;
    myWaitForFinish.clear();
  }
}

bool Model_Update::iterateUpdate(std::shared_ptr<ModelAPI_CompositeFeature> theFeature)
{
  myProcessIterator.push_back(IterationItem(theFeature));
  IterationItem& aCurrent = *myProcessIterator.rbegin();
  // two cycles: parameters must be processed first
  for(aCurrent.startIteration(true); aCurrent.more(); aCurrent.next()) {
    if (aCurrent.current()->getKind() == "Parameter")
      updateFeature(aCurrent.current());
  }
  // number of subs can be changed in execution: like fillet
  for(aCurrent.startIteration(false); aCurrent.more(); aCurrent.next()) {
    FeaturePtr aSub = aCurrent.current();
    if (aSub->getKind() != "Parameter")
      updateFeature(aSub);
  }
  // processing is finished, so, remove the iterated
  bool aResult = !aCurrent.isBreaked(); // iteration is finished correctly, not breaked
  myProcessIterator.pop_back();
  return aResult;
}

void Model_Update::iterateUpdateBreak(std::shared_ptr<ModelAPI_Feature> theFeature)
{
  // checking that this feature is before the current iterated one: otherwise break is not needed
  std::list<IterationItem>::reverse_iterator aProcessed = myProcessIterator.rbegin();
  for(; aProcessed != myProcessIterator.rend(); aProcessed++) {
    if (aProcessed->isIterated(theFeature)) {
      if (aProcessed->isEarlierThanCurrent(theFeature)) {
        // break all lower level iterators
        std::list<IterationItem>::reverse_iterator aBreaked = myProcessIterator.rbegin();
        for(; aBreaked != aProcessed; aBreaked++) {
          aBreaked->setBreaked();
        }
        // for the current breaked, set iteration to this feature precisely
        aBreaked->setCurrentBefore(theFeature);
        myModification++;
      }
      // the iterator that contains breaked is found, so, nothing else is needed
      return;
    }
  }
  // if this feature is not found in the list of the currently iterated, try to break the parent
  FeaturePtr aParent = ModelAPI_Tools::compositeOwner(theFeature);
  if (aParent.get())
    iterateUpdateBreak(aParent);
}

void Model_Update::processOperation(const bool theTotalUpdate, const bool theFinish)
{
  if (theFinish) {
    // the hardcode (DBC asked): hide the sketch referenced by extrusion on apply
    std::set<std::shared_ptr<ModelAPI_Object> >::iterator aFIter;
    for(aFIter = myWaitForFinish.begin(); aFIter != myWaitForFinish.end(); aFIter++)
    {
      FeaturePtr aF = std::dynamic_pointer_cast<ModelAPI_Feature>(*aFIter);
      if (aF && aF->data()->isValid() && 
           (aF->getKind() == "Extrusion" || aF->getKind() == "Revolution")) {
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
    #ifdef DEB_UPDATE
      std::cout<<"****** Start processing"<<std::endl;
    #endif
    myIsExecuted = true;

    bool isAutomaticChanged = false;

    if (theTotalUpdate && !myIsAutomatic) { // Apply button now works as "Rebuild"
      isAutomaticChanged = true;
      myIsAutomatic = true;
    }
    // init iteration from the root document
    iterateUpdate(CompositeFeaturePtr());

    if (isAutomaticChanged) myIsAutomatic = false;
    myIsExecuted = false;
    myModification++;

    // flush to update display
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    aLoop->flush(EVENT_DISP);
    #ifdef DEB_UPDATE
      std::cout<<"****** End processing"<<std::endl;
    #endif
  }
}

void Model_Update::updateFeature(FeaturePtr theFeature)
{
  // check all features this feature depended on (recursive call of updateFeature)
  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();

  if (theFeature->isDisabled())
    return;

  #ifdef DEB_UPDATE
    //std::cout<<"Update Feature "<<theFeature->name()<<std::endl;
  #endif
  CompositeFeaturePtr aCompos = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  // If automatice update is not needed and feature attributes were not updated right now,
  // do not execute it and do not update arguments.
  if (!myIsAutomatic && 
       (myUpdated.find(theFeature) == myUpdated.end() || myUpdated[theFeature] != myModification)
       && !aCompos.get()) {
    // execute will be performed later, but some features may have not-result 
    // presentations, so call update for them (like coincidence in the sketcher)
    static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    ModelAPI_EventCreator::get()->sendUpdated(theFeature, EVENT_DISP);
    return;
  }

  // Update selection and parameters attributes first, before sub-features analysis (sketch plane).
  updateArguments(theFeature);

  // composite feature must be executed after sub-features execution
  if (aCompos) {
    if (!iterateUpdate(aCompos))
      return; // iteration was interrupted, so, interrupt the update of this feature (it will be done later)
    // reupdate arguments of composite feature: it may be changed during subs execution
    if (theFeature->data()->execState() != ModelAPI_StateMustBeUpdated)
      updateArguments(theFeature);
  }
  // this checking must be after the composite feature sub-elements processing:
  // composite feature status may depend on it's subelements
  if (theFeature->data()->execState() == ModelAPI_StateInvalidArgument) {
    theFeature->eraseResults();
    redisplayWithResults(theFeature, ModelAPI_StateInvalidArgument); // result also must be updated
    return;
  }

  // only the currently updated features are executed
  bool aJustUpdated = myUpdated.find(theFeature) != myUpdated.end();
  if (aJustUpdated) {
    // if preview is not needed, the created feature was not updated before, so, myModification is not actual for this
    if (theFeature->isPreviewNeeded()) {
      aJustUpdated = myUpdated[theFeature] == myModification;
    }
  }

  if (myIsAutomatic && theFeature->data()->execState() == ModelAPI_StateMustBeUpdated)
    aJustUpdated = true;

  // On abort, undo or redo execute is not needed: results in document are updated automatically
  // But redisplay is needed: results are updated, must be also updated in the viewer.
  if (aJustUpdated && 
      !std::dynamic_pointer_cast<Model_Document>(theFeature->document())->executeFeatures()) {
    if (!theFeature->isPersistentResult()) { // not persistent must be re-executed on abort, etc.
      ModelAPI_ExecState aState = theFeature->data()->execState();
      if (aFactory->validate(theFeature)) {
        executeFeature(theFeature);
      } else {
        theFeature->eraseResults();
        redisplayWithResults(theFeature, ModelAPI_StateInvalidArgument); // result also must be updated
      }
    } else {
      redisplayWithResults(theFeature, ModelAPI_StateNothing);
      if (theFeature->data()->execState() == ModelAPI_StateMustBeUpdated) { // it is done (in the tree)
        theFeature->data()->execState(ModelAPI_StateDone);
      }
      // it will be not updated with new modifications: only the currently updated features are updated
      //if (myUpdated.find(theFeature) != myUpdated.end()) {
      //  myUpdated.erase(theFeature); // do not update this persistent feature even in the future
      //}
    }
    return;
  }

  // execute feature if it must be updated
  if (theFeature->isPreviewNeeded() || myIsFinish) {
    if (aJustUpdated) {
      ModelAPI_ExecState aState = theFeature->data()->execState();
      if (aFactory->validate(theFeature)) {
        #ifdef DEB_UPDATE
          std::cout<<"Execute Feature "<<theFeature->name()<<std::endl;
        #endif
        executeFeature(theFeature);
      } else {
        theFeature->eraseResults();
        redisplayWithResults(theFeature, ModelAPI_StateInvalidArgument); // result also must be updated
      }
    }
  } else { // preview is not needed => make state Done
    if (theFeature->data()->execState() == ModelAPI_StateMustBeUpdated) {
      theFeature->data()->execState(ModelAPI_StateDone);
      if (aJustUpdated) // store that it must be updated on finish
        myUpdated[theFeature] = myModification;
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
    if (!aRes->isDisabled()) {// update state only for enabled results (Placement Result Part may make the original Part Result as invalid)
      aRes->data()->execState(theState);
      if (theState == ModelAPI_StateDone) // feature become "done", so execution changed results
        myUpdated[aRes] = myModification;
    }
    if (theFeature->data()->updateID() > aRes->data()->updateID()) {
      aRes->data()->setUpdateID(theFeature->data()->updateID());
    }
    ModelAPI_EventCreator::get()->sendUpdated(aRes, EVENT_DISP);
    // iterate sub-bodies of compsolid
    ResultCompSolidPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(aRes);
    if (aComp.get()) {
      int aNumSub = aComp->numberOfSubs();
      for(int a = 0; a < aNumSub; a++) {
        ResultPtr aSub = aComp->subResult(a);
        if (!aSub->isDisabled()) {// update state only for enabled results (Placement Result Part may make the original Part Result as invalid)
          aSub->data()->execState(theState);
          if (theState == ModelAPI_StateDone) // feature become "done", so execution changed results
            myUpdated[aSub] = myModification;
        }
        if (theFeature->data()->updateID() > aSub->data()->updateID()) {
          aSub->data()->setUpdateID(theFeature->data()->updateID());
        }
        ModelAPI_EventCreator::get()->sendUpdated(aSub, EVENT_DISP);
      }
    }
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
      if (theCurrent == ModelAPI_StateDone)
        return ModelAPI_StateMustBeUpdated;
    } else if (aRefState != ModelAPI_StateDone) {
      return ModelAPI_StateInvalidArgument;
    }
  }
  return theCurrent;
}

bool Model_Update::isOlder(std::shared_ptr<ModelAPI_Feature> theFeature, 
                           std::shared_ptr<ModelAPI_Object> theArgument)
{
  int aFeatureID = theFeature->data()->updateID();
  int anArgID = theArgument->data()->updateID();
  if (aFeatureID < anArgID)
    return true;
  std::map<std::shared_ptr<ModelAPI_Object>, int >::iterator anAIter = myUpdated.find(theArgument);
  if (anAIter == myUpdated.end())
    return false;
  std::map<std::shared_ptr<ModelAPI_Object>, int >::iterator aFIter = myUpdated.find(theFeature);
  if (aFIter == myUpdated.end())
    return true; // argument is updated, but feature is not updated at all
  return aFIter->second < anAIter->second;
}

void Model_Update::updateArguments(FeaturePtr theFeature) {
  // perform this method also for disabled features: to make "not done" state for
  // features referenced to the active and modified features

  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();

  ModelAPI_ExecState aState = theFeature->data()->execState();
  if (aState == ModelAPI_StateExecFailed) { // try again failed feature: issue 577
    aState = ModelAPI_StateMustBeUpdated;
  }
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
      if (myIsParamUpdated) {
        ModelAPI_AttributeEvalMessage::send(aDouble, this);
      }
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
        if (myIsParamUpdated) {
          ModelAPI_AttributeEvalMessage::send(aPointAttribute, this);
        }
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
        if (myIsParamUpdated) {
          ModelAPI_AttributeEvalMessage::send(aPoint2DAttribute, this);
        }
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
    // update argument only if the referenced object is changed
    if (aContext.get() && !aContext->isDisabled()) {
      bool isObligatory = !aFactory->isNotObligatory(
        theFeature->getKind(), theFeature->data()->id(aSel)) &&
        aFactory->isCase(theFeature, theFeature->data()->id(aSel));
      if (isOlder(theFeature, aContext)) {
        if (aState == ModelAPI_StateDone)
          aState = ModelAPI_StateMustBeUpdated;
        if (!aSel->update()) { // this must be done on execution since it may be long operation
          if (isObligatory)
            aState = ModelAPI_StateInvalidArgument;
        }
      } else if (aSel->isInvalid()) { // not needed to update, but invalid (stated previously)
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
        if (aContext.get() && !aContext->isDisabled()) {
          bool isObligatory = !aFactory->isNotObligatory(
            theFeature->getKind(), theFeature->data()->id(aSel)) &&
            aFactory->isCase(theFeature, theFeature->data()->id(aSel));
          if (isOlder(theFeature, aContext)) {
            if (aState == ModelAPI_StateDone)
                aState = ModelAPI_StateMustBeUpdated;
            if (!aSelAttr->update()) {
              if (isObligatory)
                aState = ModelAPI_StateInvalidArgument;
            }
          } else if (aSelAttr->isInvalid()) {
            aState = ModelAPI_StateInvalidArgument;
          }
        }
      }
    }
  }
  // check all references: if referenced objects are updated, this object also must be updated
  // also check state of referenced objects: if they are not ready, inherit corresponding state
  std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefsObj;
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theFeature->data());
  aData->referencesToObjects(aRefsObj);
  std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRef = aRefsObj.begin();
  for(; aRef != aRefsObj.end(); aRef++) {
    std::list<ObjectPtr>::iterator aRefObj = aRef->second.begin();
    for(; aRefObj != aRef->second.end(); aRefObj++) {
      // if reference is null, it may mean that this reference is to other document
      // the does not supported by RefList: parameters may be recomputed
      if (!aRefObj->get() && theFeature->firstResult().get() && 
               theFeature->firstResult()->groupName() == ModelAPI_ResultParameter::group()) {
          if (aState == ModelAPI_StateDone)
            aState = ModelAPI_StateMustBeUpdated;
      } else if (aRefObj->get() && isOlder(theFeature, *aRefObj)) {
        if (aState == ModelAPI_StateDone)
          aState = ModelAPI_StateMustBeUpdated;
      }
      aState = stateByReference(*aRefObj, aState);
    }
  }
  // composites sub-elements
  CompositeFeaturePtr aCompos = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  // composite feature must be executed after sub-features execution
  if (aCompos) {
    // number of subs can be changed in execution: like fillet
    int aNumSubs = aCompos->numberOfSubs();
    for(int a = 0; a < aNumSubs; a++) {
      FeaturePtr aSub = aCompos->subFeature(a);
      if (aSub.get() && aState == ModelAPI_StateDone) {
        if (isOlder(theFeature, aSub)) {
          aState = ModelAPI_StateMustBeUpdated;
        }
        // also check that all results of subs were updated: composite also depends on the results
        const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
        std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter = aResults.begin();
        for(; aResIter != aResults.end(); aResIter++) {
          if (aResIter->get() && (*aResIter)->data()->isValid() && !(*aResIter)->isDisabled() &&
              isOlder(theFeature, *aResIter)) {
            aState = ModelAPI_StateMustBeUpdated;
          }
        }
      }
      if (a == aNumSubs - 1) // in case number of subs is changed, just recheck before end
        aNumSubs = aCompos->numberOfSubs();
    }
  }


  if (aState != ModelAPI_StateDone)
    theFeature->data()->execState(aState);
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
      myWaitForFinish.insert(theFeature);
    }
  } catch(...) {
    aState = ModelAPI_StateExecFailed;
    Events_Error::send(
      "Feature " + theFeature->getKind() + " has failed during the execution");
  }
  if (aState != ModelAPI_StateDone) {
    theFeature->eraseResults();
  }
  theFeature->data()->setUpdateID(ModelAPI_Session::get()->transactionID());
  redisplayWithResults(theFeature, aState);
}

///////////////// Updated items iterator ////////////////////////
Model_Update::IterationItem::IterationItem(std::shared_ptr<ModelAPI_CompositeFeature> theFeature)
{
  myBreaked = false;
  myIsVirtual = false;
  myMain = theFeature;
  myObjects = NULL;
  if (!myMain.get() && ModelAPI_Session::get()->hasModuleDocument()) { // no document => nothing to do
    DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
    myObjects = std::dynamic_pointer_cast<Model_Document>(aRootDoc)->objects();
  }
  mySkipNext = false;
}

void Model_Update::IterationItem::next()
{
  if (mySkipNext) { // ignore one next
    mySkipNext = false;
    return;
  }
  if (!myBreaked) {
    if (myMain.get()) {
      myIndex++;
      int aNumSubs = myMain->numberOfSubs();
      if (myIndex == aNumSubs)
        return;
      // skip sub-objects, that are subs not only for this: sketch elements relatively to Part
      for(FeaturePtr aSub = myMain->subFeature(myIndex); aSub.get();
          aSub = myMain->subFeature(myIndex)) {
        aSub = myMain->subFeature(myIndex);
        CompositeFeaturePtr anOwner = ModelAPI_Tools::compositeOwner(aSub);
        if (!anOwner.get() || anOwner == myMain) {
          break;
        }
        myIndex++;
        if (myIndex == aNumSubs)
          break;
      }
    } else if (mySub.get()) {
      mySub = myObjects->nextFeature(mySub);
    }
  }
}

bool Model_Update::IterationItem::more()
{
  if (myBreaked)
    return false;
  if (myMain.get())
    return myIndex < myMain->numberOfSubs();
  return mySub.get() != NULL;
}

FeaturePtr Model_Update::IterationItem::current()
{
  if (myMain.get())
    return myMain->subFeature(myIndex);
  return mySub;
}

void Model_Update::IterationItem::setBreaked()
{
  if (!myIsVirtual)
    myBreaked = true;
}

void Model_Update::IterationItem::startIteration(const bool theVirtual)
{
  myIsVirtual = theVirtual;
  if (myMain.get()) {
    myIndex = 0;
  } else if (myObjects) {
    mySub = myObjects->firstFeature();
  }
}

bool Model_Update::IterationItem::isIterated(FeaturePtr theFeature)
{
  if (myMain.get()) {
    if (myMain->isSub(theFeature)) {
      CompositeFeaturePtr anOwner = ModelAPI_Tools::compositeOwner(theFeature);
      if (!anOwner.get() || anOwner == myMain)
        return true;
    }
    return false;
  }
  // for the root document just check that this feature in this document and it is not sub
  return myObjects->owner() == theFeature->document() && 
         !ModelAPI_Tools::compositeOwner(theFeature).get();
}

bool Model_Update::IterationItem::isEarlierThanCurrent(FeaturePtr theFeature)
{
  if (myMain.get()) {
    for(int a = 0; a < myIndex; a++) {
      if (myMain->subFeature(a) == theFeature)
        return true;
    }
  } else {
    return !mySub.get() && !myObjects->isLater(theFeature, mySub);
  }
  return false;
}

void Model_Update::IterationItem::setCurrentBefore(FeaturePtr theFeature)
{
  if (myMain.get()) {
    for(int a = 0; a < myIndex; a++) {
      if (myMain->subFeature(a) == theFeature) {
        myIndex = a;
        break;
      }
    }
  } else {
    mySub = theFeature;
  }
  mySkipNext = true;
}
