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
  static const Events_ID kCreatedEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_CREATED);
  aLoop->registerListener(this, kCreatedEvent);
  static const Events_ID kUpdatedEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED);
  aLoop->registerListener(this, kUpdatedEvent);
  static const Events_ID kOpFinishEvent = aLoop->eventByName("FinishOperation");
  aLoop->registerListener(this, kOpFinishEvent);
  static const Events_ID kOpAbortEvent = aLoop->eventByName("AbortOperation");
  aLoop->registerListener(this, kOpAbortEvent);
  static const Events_ID kOpStartEvent = aLoop->eventByName("StartOperation");
  aLoop->registerListener(this, kOpStartEvent);
  static const Events_ID kStabilityEvent = aLoop->eventByName(EVENT_STABILITY_CHANGED);
  aLoop->registerListener(this, kStabilityEvent);
  static const Events_ID kPreviewBlockedEvent = aLoop->eventByName(EVENT_PREVIEW_BLOCKED);
  aLoop->registerListener(this, kPreviewBlockedEvent);
  static const Events_ID kPreviewRequestedEvent = aLoop->eventByName(EVENT_PREVIEW_REQUESTED);
  aLoop->registerListener(this, kPreviewRequestedEvent);

  //  Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
  myIsParamUpdated = false;
  myIsFinish = false;
  myIsProcessed = false;
  myIsPreviewBlocked = false;
}

bool Model_Update::addModified(FeaturePtr theFeature, FeaturePtr theReason) {

  if (!theFeature->data()->isValid())
    return false; // delete an extrusion created on the sketch

  if (theFeature->isPersistentResult()) {
    if (!std::dynamic_pointer_cast<Model_Document>((theFeature)->document())->executeFeatures())
      return false;
  }

  // update arguments for "apply button" state change
  if ((!theFeature->isPreviewNeeded() && !myIsFinish) || myIsPreviewBlocked) {
    myProcessOnFinish.insert(theFeature);
#ifdef DEB_UPDATE
      std::cout<<"*** Add process on finish "<<theFeature->name()<<std::endl;
#endif
    static std::set<FeaturePtr> aCurrentlyUpdated;
    if (aCurrentlyUpdated.find(theFeature) == aCurrentlyUpdated.end()) {
      aCurrentlyUpdated.insert(theFeature);
      updateArguments(theFeature);
      aCurrentlyUpdated.erase(theFeature);
    }
    // make it without conditions otherwise the apply button may have a bad state
    theFeature->data()->execState(ModelAPI_StateDone);
    static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
    aFactory->validate(theFeature); // need to be validated to update the "Apply" state if not previewed

    if (!myIsPreviewBlocked)
      return true;
  }
  if (myModified.find(theFeature) != myModified.end()) {
    if (theReason.get()) {
#ifdef DEB_UPDATE
      std::cout<<"*** Add already modified "<<theFeature->name()<<std::endl;
#endif
      myModified[theFeature].insert(theReason);
    }
    return true;
  }
  // do not add the disabled, but possibly the sub-elements are not disabled
  bool aIsDisabled = theFeature->isDisabled();
  if (!aIsDisabled) {
    std::set<std::shared_ptr<ModelAPI_Feature> > aNewSet;
    if (theFeature->data()->execState() == ModelAPI_StateMustBeUpdated) {
      // do not forget that in this case all were the reasons
      aNewSet.insert(theFeature);
    } else {
      if (theReason.get())
        aNewSet.insert(theReason);
    }
    myModified[theFeature] = aNewSet;
#ifdef DEB_UPDATE
    std::cout<<"*** Add modified "<<theFeature->name()<<std::endl;
#endif
  } else { // will be updated during the finish of the operation, or when it becomes enabled
    if (theFeature->data()->execState() == ModelAPI_StateDone)
      theFeature->data()->execState(ModelAPI_StateMustBeUpdated);
    else 
      return true; // do not need iteration deeply if it is already marked as modified or so
#ifdef DEB_UPDATE
    std::cout<<"*** Set modified state "<<theFeature->name()<<std::endl;
#endif
  }
  // clear processed and fill modified recursively
  const std::set<std::shared_ptr<ModelAPI_Attribute> >& aRefs = theFeature->data()->refsToMe();
  std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aRefIter = aRefs.cbegin();
  for(; aRefIter != aRefs.cend(); aRefIter++) {
    if ((*aRefIter)->isArgument()) {
      FeaturePtr aReferenced = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIter)->owner());
      if (aReferenced.get()) {
        addModified(aReferenced, theFeature);
      }
    }
  }
  // proccess also results
  std::list<ResultPtr> allResults; // list of this feature and results
  ModelAPI_Tools::allResults(theFeature, allResults);
  std::list<ResultPtr>::iterator aRes = allResults.begin();
  for(; aRes != allResults.end(); aRes++) {
    const std::set<std::shared_ptr<ModelAPI_Attribute> >& aRefs = (*aRes)->data()->refsToMe();
    std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aRefIter = aRefs.cbegin();
    for(; aRefIter != aRefs.cend(); aRefIter++) {
      if ((*aRefIter)->isArgument()) {
        FeaturePtr aReferenced = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIter)->owner());
        if (aReferenced.get()) {
          addModified(aReferenced, theFeature);
        }
      }
    }
  }

  // also add part feature that contains this feature to the modified
  if (theFeature->document()->kind() != "PartSet") {
    FeaturePtr aPart = ModelAPI_Tools::findPartFeature(
      ModelAPI_Session::get()->moduleDocument(), theFeature->document());
    if (aPart.get())
      addModified(aPart, theFeature);
  }
  return true;
}

void Model_Update::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  static Events_Loop* aLoop = Events_Loop::loop();
  static const Events_ID kCreatedEvent = aLoop->eventByName(EVENT_OBJECT_CREATED);
  static const Events_ID kUpdatedEvent = aLoop->eventByName(EVENT_OBJECT_UPDATED);
  static const Events_ID kOpFinishEvent = aLoop->eventByName("FinishOperation");
  static const Events_ID kOpAbortEvent = aLoop->eventByName("AbortOperation");
  static const Events_ID kOpStartEvent = aLoop->eventByName("StartOperation");
  static const Events_ID kStabilityEvent = aLoop->eventByName(EVENT_STABILITY_CHANGED);
  static const Events_ID kPreviewBlockedEvent = aLoop->eventByName(EVENT_PREVIEW_BLOCKED);
  static const Events_ID kPreviewRequestedEvent = aLoop->eventByName(EVENT_PREVIEW_REQUESTED);

#ifdef DEB_UPDATE
  std::cout<<"****** Event "<<theMessage->eventID().eventText()<<std::endl;
#endif
  if (theMessage->eventID() == kStabilityEvent) {
    updateStability(theMessage->sender());
    return;
  }
  if (theMessage->eventID() == kPreviewBlockedEvent) {
    myIsPreviewBlocked = true;
    return;
  }
  if (theMessage->eventID() == kPreviewRequestedEvent) {
    if (myIsPreviewBlocked) {
      myIsPreviewBlocked = false;
      processFeatures();
      myIsPreviewBlocked = true;
    }
    return;
  }
  // creation is added to "update" to avoid recomputation twice: on create and immediately after on update
  if (theMessage->eventID() == kCreatedEvent) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    const std::set<ObjectPtr>& anObjs = aMsg->objects();
    std::set<ObjectPtr>::const_iterator anObjIter = anObjs.cbegin();
    for(; anObjIter != anObjs.cend(); anObjIter++) {
      if (std::dynamic_pointer_cast<Model_Document>((*anObjIter)->document())->executeFeatures())
        ModelAPI_EventCreator::get()->sendUpdated(*anObjIter, kUpdatedEvent);
    }
    return;
  }
  if (theMessage->eventID() == kUpdatedEvent) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    const std::set<ObjectPtr>& anObjs = aMsg->objects();
    std::set<ObjectPtr>::const_iterator anObjIter = anObjs.cbegin();
    bool aSomeModified = false; // check that features not changed: only redisplay is needed
    for(; anObjIter != anObjs.cend(); anObjIter++) {
      if (!(*anObjIter)->data()->isValid())
        continue;
#ifdef DEB_UPDATE
      std::cout<<">>> in event updated "<<(*anObjIter)->data()->name()<<std::endl;
#endif
      if ((*anObjIter)->groupName() == ModelAPI_ResultParameter::group()) {
        myIsParamUpdated = true;
      }
      // on undo/redo, abort do not update persisten features
      FeaturePtr anUpdated = std::dynamic_pointer_cast<ModelAPI_Feature>(*anObjIter);
      if (anUpdated.get()) {
        if (addModified(anUpdated, FeaturePtr()))
          aSomeModified = true;
      } else { // process the updated result as update of features that refers to this result
        const std::set<std::shared_ptr<ModelAPI_Attribute> >& aRefs = (*anObjIter)->data()->refsToMe();
        std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aRefIter = aRefs.cbegin();
        for(; aRefIter != aRefs.cend(); aRefIter++) {
          if (!(*aRefIter)->owner()->data()->isValid())
            continue;
          FeaturePtr anUpdated = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIter)->owner());
          if (anUpdated.get()) {
            if (addModified(anUpdated, FeaturePtr()))
              aSomeModified = true;
          }
        }
      }
    }
    // this event is for solver update, not here, do not react immediately
    if (aSomeModified) {
        processFeatures();
    }
  } else if (theMessage->eventID() == kOpFinishEvent || theMessage->eventID() == kOpAbortEvent ||
      theMessage->eventID() == kOpStartEvent) {
    myIsPreviewBlocked = false;

    if (theMessage->eventID() == kOpFinishEvent) {
      myIsFinish = true;
      // add features that wait for finish as modified
      std::set<std::shared_ptr<ModelAPI_Feature> >::iterator aFeature = myProcessOnFinish.begin();
      for(; aFeature != myProcessOnFinish.end(); aFeature++)
        if ((*aFeature)->data()->isValid()) // there may be already removed wait for features
          addModified(*aFeature, FeaturePtr());
      myIsFinish = false;
    }
    myProcessOnFinish.clear(); // processed features must be only on finish, so clear anyway (to avoid reimport on load)

    if (!(theMessage->eventID() == kOpStartEvent)) {
      processFeatures(false);
    }
    // remove all macros before clearing all created
    std::set<FeaturePtr>::iterator anUpdatedIter = myWaitForFinish.begin();
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
    // the redisplay signal should be flushed in order to erase the feature presentation in the viewer
    // if should be done after removeFeature() of document,
    // by this reason, upper processFeatures() do not perform this flush
    Events_Loop::loop()->flush(Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY));

    // in the end of transaction everything is updated, so clear the old objects
    myIsParamUpdated = false;
    myWaitForFinish.clear();
  }
}

void Model_Update::processFeatures(const bool theFlushRedisplay)
{
   // perform update of everything if it is not performed right now or any preview is blocked
  if (!myIsProcessed && !myIsPreviewBlocked) {
    myIsProcessed = true;
    #ifdef DEB_UPDATE
      std::cout<<"****** Start processing"<<std::endl;
    #endif

    while(!myModified.empty()) {
      processFeature(myModified.begin()->first);
    }
    myIsProcessed = false;

    // flush updates just before "myModification" increment: to distinguish
    // updates by "execute" produced by this updater and other updates, coming outside,
    // which are really important for "processEvent" of this updater
    static Events_Loop* aLoop = Events_Loop::loop();
    static const Events_ID kUpdatedEvent = aLoop->eventByName(EVENT_OBJECT_UPDATED);
    aLoop->flush(kUpdatedEvent);

    // flush to update display
    if (theFlushRedisplay) {
      static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
      aLoop->flush(EVENT_DISP);
    }
    #ifdef DEB_UPDATE
      std::cout<<"****** End processing"<<std::endl;
    #endif
    myProcessed.clear();
  }
}

bool Model_Update::processFeature(FeaturePtr theFeature)
{
  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();

  if (!theFeature->data()->isValid()) { // deleted feature, just remove from all containers
    if (myModified.find(theFeature) != myModified.end())
      myModified.erase(theFeature);
    return false;
  }

  if (theFeature->isPersistentResult()) {
    if (!std::dynamic_pointer_cast<Model_Document>((theFeature)->document())->executeFeatures())
      return false;
  }

  if (myProcessed.find(theFeature) == myProcessed.end()) {
    myProcessed[theFeature] = 0;
  } else {
    int aCount = myProcessed[theFeature];
    if (aCount > 100) { // too many repetition of processing (in VS it may crash on 330 with stack overflow)
      Events_Error::send(
        "Feature '" + theFeature->data()->name() + "' is updated in infinitive loop");
      return false;
    }
    myProcessed[theFeature] = aCount + 1;
  }

  // check this feature is not yet checked or processed
  bool aIsModified = myModified.find(theFeature) != myModified.end();
  if (!aIsModified && myIsFinish) { // get info about the modification for features without preview
    if (theFeature->data()->execState() == ModelAPI_StateMustBeUpdated) {
      aIsModified = true;
      std::set<std::shared_ptr<ModelAPI_Feature> > aNewSet;
      aNewSet.insert(theFeature); // contains itself, so, we don't know which was the reason and the reason is any
      myModified[theFeature] = aNewSet;
    }
  }

#ifdef DEB_UPDATE
    std::cout<<"* Process feature "<<theFeature->name()<<std::endl;
#endif

  // update the sketch plane before the sketch sub-elements are recomputed
  // (otherwise sketch will update plane, modify subs, after executed, but with old subs edges)
  if (aIsModified && theFeature->getKind() == "Sketch") {
#ifdef DEB_UPDATE
    std::cout<<"****** Update sketch args "<<theFeature->name()<<std::endl;
#endif
    AttributeSelectionPtr anExtSel = theFeature->selection("External");
    if (anExtSel.get()) {
      ResultPtr aContext = anExtSel->context();
      if (aContext.get() && aContext->document().get()) {
        FeaturePtr anExtBase = aContext->document()->feature(aContext);
        if (anExtBase.get()) {
          processFeature(anExtBase);
        }
      }
    }
    updateArguments(theFeature);
  }

  if (!aIsModified) { // no modification is needed
    return false;
  }

  // evaluate parameter before the sub-elements update: it updates dependencies on evaluation (#1085)
  if (theFeature->getKind() == "Parameter") {
    theFeature->execute();
  }

  // check all features this feature depended on (recursive call of updateFeature)
  std::set<std::shared_ptr<ModelAPI_Feature> >& aReasons = myModified[theFeature];
  if (aReasons.find(theFeature) == aReasons.end()) {
    std::set<std::shared_ptr<ModelAPI_Feature> >::iterator aReasonIter = aReasons.begin();
    for(; aReasonIter != aReasons.end(); aReasonIter++) {
      if (*aReasonIter != theFeature && processFeature(*aReasonIter)) {
        aIsModified = true;
      }
    }
  } else { // check all features this feature depended on because here which one was modified is unknown
    std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > > aDeps;
    theFeature->data()->referencesToObjects(aDeps);
    std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > >::iterator
      anAttrsIter = aDeps.begin();
    for(; anAttrsIter != aDeps.end(); anAttrsIter++) {
      if (theFeature->attribute(anAttrsIter->first)->isArgument()) {
        std::list<std::shared_ptr<ModelAPI_Object> >::iterator aDepIter = anAttrsIter->second.begin();
        for(; aDepIter != anAttrsIter->second.end(); aDepIter++) {
          FeaturePtr aDepFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(*aDepIter);
          if (!aDepFeat.get()) { // so, it depends on the result and process the feature owner of it
            ResultPtr aDepRes = std::dynamic_pointer_cast<ModelAPI_Result>(*aDepIter);
            if (aDepRes.get()) {
              aDepFeat = (*aDepIter)->document()->feature(aDepRes);
            }
          }
          if (aDepFeat.get()) {
            if (processFeature(aDepFeat))
              aIsModified = true;
          }
        }
      }
    }
    if (theFeature->getKind() == "Part") { // part is not depended on its subs directly, but subs must be iterated anyway
      CompositeFeaturePtr aPart = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
      int aNum = aPart->numberOfSubs();
      for(int a = 0; a < aNum; a++) {
        FeaturePtr aSub = aPart->subFeature(a);
        if (aSub.get()) {
          if (processFeature(aSub))
            aIsModified = true;
        }
      }
    }
  }

  // do not execute the composite that contains the current
  bool isPostponedMain = false;
  CompositeFeaturePtr aCompos = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (theFeature->getKind() == "ExtrusionSketch" && aCompos.get()) {
    CompositeFeaturePtr aCurrentOwner = 
      ModelAPI_Tools::compositeOwner(theFeature->document()->currentFeature(false));
    isPostponedMain = aCurrentOwner.get() && aCompos->isSub(aCurrentOwner);
  }

  #ifdef DEB_UPDATE
    std::cout<<"Update args "<<theFeature->name()<<std::endl;
  #endif
  // Update selection and parameters attributes first, before sub-features analysis (sketch plane).
  updateArguments(theFeature);

  // add this feature to the processed right now to be able remove it from this list on
  // update signal during this feature execution
  myModified.erase(theFeature);
  if (theFeature->data()->execState() == ModelAPI_StateMustBeUpdated)
    theFeature->data()->execState(ModelAPI_StateDone);

  // this checking must be after the composite feature sub-elements processing:
  // composite feature status may depend on it's subelements
  if (theFeature->data()->execState() == ModelAPI_StateInvalidArgument) {
  #ifdef DEB_UPDATE
    std::cout<<"Invalid args "<<theFeature->name()<<std::endl;
  #endif
    theFeature->eraseResults();
    redisplayWithResults(theFeature, ModelAPI_StateInvalidArgument); // result also must be updated
    return true; // so, feature is modified (results are erased)
  }

  // execute feature if it must be updated
  ModelAPI_ExecState aState = theFeature->data()->execState();
  if (aFactory->validate(theFeature)) {
    if (!isPostponedMain) {
      executeFeature(theFeature);
    }
  } else {
    #ifdef DEB_UPDATE
      std::cout<<"Feature is not valid, erase results "<<theFeature->name()<<std::endl;
    #endif
    theFeature->eraseResults();
    redisplayWithResults(theFeature, ModelAPI_StateInvalidArgument); // result also must be updated
  }
  return true;
}

void Model_Update::redisplayWithResults(FeaturePtr theFeature, const ModelAPI_ExecState theState) 
{
  // make updated and redisplay all results
  static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);

  std::list<ResultPtr> allResults;
  ModelAPI_Tools::allResults(theFeature, allResults);
  std::list<ResultPtr>::iterator aRIter = allResults.begin();
  for (; aRIter != allResults.cend(); aRIter++) {
    std::shared_ptr<ModelAPI_Result> aRes = *aRIter;
    if (!aRes->isDisabled()) {// update state only for enabled results (Placement Result Part may make the original Part Result as invalid)
      aRes->data()->execState(theState);
    }
    if (theFeature->data()->updateID() > aRes->data()->updateID()) {
      aRes->data()->setUpdateID(theFeature->data()->updateID());
    }
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
      if (theCurrent == ModelAPI_StateDone)
        return ModelAPI_StateMustBeUpdated;
    } else if (aRefState != ModelAPI_StateDone) {
      return ModelAPI_StateInvalidArgument;
    }
  }
  return theCurrent;
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
  // Integer
  {
    std::list<AttributePtr> anAttrinbutes =
      theFeature->data()->attributes(ModelAPI_AttributeInteger::typeId());
    std::list<AttributePtr>::iterator anIter = anAttrinbutes.begin();
    for(; anIter != anAttrinbutes.end(); anIter++) {
      AttributeIntegerPtr anAttribute =
        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(*anIter);
      if (anAttribute.get() && !anAttribute->text().empty()) {
        if (myIsParamUpdated) {
          ModelAPI_AttributeEvalMessage::send(anAttribute, this);
        }
        if (anAttribute->expressionInvalid()) {
          aState = ModelAPI_StateInvalidArgument;
        }
      }
    }
  }
  // Double
  {
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
  }
  // Point
  {
    std::list<AttributePtr> anAttributes =
      theFeature->data()->attributes(GeomDataAPI_Point::typeId());
    std::list<AttributePtr>::iterator anIter = anAttributes.begin();
    for(; anIter != anAttributes.end(); anIter++) {
      AttributePointPtr aPointAttribute =
        std::dynamic_pointer_cast<GeomDataAPI_Point>(*anIter);
      if (aPointAttribute.get() && (!aPointAttribute->textX().empty() || 
          !aPointAttribute->textY().empty() || !aPointAttribute->textZ().empty())) {
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
        if (myIsParamUpdated && (!aPoint2DAttribute->textX().empty() || 
            !aPoint2DAttribute->textY().empty())) {
          ModelAPI_AttributeEvalMessage::send(aPoint2DAttribute, this);
        }
        if ((!aPoint2DAttribute->textX().empty() && aPoint2DAttribute->expressionInvalid(0)) ||
          (!aPoint2DAttribute->textY().empty() && aPoint2DAttribute->expressionInvalid(1)))
          aState = ModelAPI_StateInvalidArgument;
      }
    }
  }
  // update the selection attributes if any
  list<AttributePtr> aRefs = 
    theFeature->data()->attributes(ModelAPI_AttributeSelection::typeId());
  list<AttributePtr>::iterator aRefsIter = aRefs.begin();
  for (; aRefsIter != aRefs.end(); aRefsIter++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSel =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*aRefsIter);
    ObjectPtr aContext = aSel->context();
    // update argument only if the referenced object is ready to use
    if (aContext.get() && !aContext->isDisabled() && isReason(theFeature, aContext)) {
      if (!aSel->update()) { // this must be done on execution since it may be long operation
        bool isObligatory = !aFactory->isNotObligatory(
          theFeature->getKind(), theFeature->data()->id(aSel)) &&
          aFactory->isCase(theFeature, theFeature->data()->id(aSel));
        if (isObligatory)
          aState = ModelAPI_StateInvalidArgument;
      }
    }
  }
  // update the selection list attributes if any
  aRefs = theFeature->data()->attributes(ModelAPI_AttributeSelectionList::typeId());
  for (aRefsIter = aRefs.begin(); aRefsIter != aRefs.end(); aRefsIter++) {
    std::shared_ptr<ModelAPI_AttributeSelectionList> aSel =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(*aRefsIter);
    for(int a = aSel->size() - 1; a >= 0; a--) {
      std::shared_ptr<ModelAPI_AttributeSelection> aSelAttr =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(aSel->value(a));
      if (aSelAttr) {
        ObjectPtr aContext = aSelAttr->context();
        // update argument only if the referenced object is ready to use
        if (aContext.get() && !aContext->isDisabled() && isReason(theFeature, aContext)) {
          if (!aSelAttr->update()) {
            bool isObligatory = !aFactory->isNotObligatory(
              theFeature->getKind(), theFeature->data()->id(aSel)) &&
              aFactory->isCase(theFeature, theFeature->data()->id(aSel));
            if (isObligatory)
              aState = ModelAPI_StateInvalidArgument;
          }
        }
      }
    }
  }

  if (aState != ModelAPI_StateDone)
    theFeature->data()->execState(aState);
}

bool Model_Update::isReason(std::shared_ptr<ModelAPI_Feature>& theFeature, 
     std::shared_ptr<ModelAPI_Object> theReason) 
{
  std::map<std::shared_ptr<ModelAPI_Feature>, std::set<std::shared_ptr<ModelAPI_Feature> > >
    ::iterator aReasonsIt = myModified.find(theFeature);
  if (aReasonsIt == myModified.end())
    return false; // this case only for not-previewed items update state, nothing is changed in args for it
  if (aReasonsIt->second.find(theFeature) != aReasonsIt->second.end())
    return true; // any is reason if it contains itself
  FeaturePtr aReasFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(theReason);
  if (!aReasFeat.get()) { // try to get feature of this result
    ResultPtr aReasRes = std::dynamic_pointer_cast<ModelAPI_Result>(theReason);
    if (aReasRes.get())
      aReasFeat = theReason->document()->feature(aReasRes);
  }
  return aReasonsIt->second.find(aReasFeat) != aReasonsIt->second.end();

}

void Model_Update::executeFeature(FeaturePtr theFeature)
{
#ifdef DEB_UPDATE
  std::cout<<"Execute Feature "<<theFeature->name()<<std::endl;
#endif
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
  // The macro feature has to be deleted in any case even its execution is failed 
  myWaitForFinish.insert(theFeature);
  if (aState != ModelAPI_StateDone) {
    theFeature->eraseResults();
  }
  theFeature->data()->setUpdateID(ModelAPI_Session::get()->transactionID());
  redisplayWithResults(theFeature, aState);
}

void Model_Update::updateStability(void* theSender)
{
  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
  if (theSender) {
    bool added = false; // object may be was crated
    ModelAPI_Object* aSender = static_cast<ModelAPI_Object*>(theSender);
    if (aSender && aSender->document()) {
      FeaturePtr aFeatureSender = 
        std::dynamic_pointer_cast<ModelAPI_Feature>(aSender->data()->owner());
      if (aFeatureSender.get()) {
        Model_Objects* aDocObjects = 
          std::dynamic_pointer_cast<Model_Document>(aSender->document())->objects();
        if (aDocObjects) {
          //aDocObjects->synchronizeBackRefs();
          // remove or add all concealment refs from this feature
          std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
          aSender->data()->referencesToObjects(aRefs);
          std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRefIt = aRefs.begin();
          for(; aRefIt != aRefs.end(); aRefIt++) {
            if (!aFactory->isConcealed(aFeatureSender->getKind(), aRefIt->first))
              continue; // take into account only concealed references (do not remove the sketch constraint and the edge on constraint edit)
            std::list<ObjectPtr>& aRefFeaturesList = aRefIt->second;
            std::list<ObjectPtr>::iterator aReferenced = aRefFeaturesList.begin();
            for(; aReferenced != aRefFeaturesList.end(); aReferenced++) {
               // stability is only on results: feature to feature reference mean nested 
              // features, that will remove nesting references
              if (aReferenced->get() && (*aReferenced)->data()->isValid() && 
                (*aReferenced)->groupName() != ModelAPI_Feature::group()) {
                std::shared_ptr<Model_Data> aData = 
                  std::dynamic_pointer_cast<Model_Data>((*aReferenced)->data());
                if (aFeatureSender->isStable()) {
                  aData->addBackReference(aFeatureSender, aRefIt->first);
                } else {
                  aData->removeBackReference(aFeatureSender, aRefIt->first);
                  added = true; // remove of concealment may be caused creation of some result
                }
              }
            }
          }
        }
      }
    }
    if (added) {
      static Events_Loop* aLoop = Events_Loop::loop();
      static Events_ID kEventCreated = aLoop->eventByName(EVENT_OBJECT_CREATED);
      aLoop->flush(kEventCreated);
    }
  }
}
