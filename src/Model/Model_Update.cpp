// Copyright (C) 2014-2025  CEA, EDF
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
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_ResultConstruction.h>
#include <GeomAPI_Shape.h>
#include <GeomDataAPI_Point.h>
#include <GeomDataAPI_Dir.h>
#include <GeomDataAPI_Point2D.h>
#include <Events_Loop.h>
#include <Events_LongOp.h>
#include <Events_InfoMessage.h>
#include <Config_PropManager.h>

Model_Update MY_UPDATER_INSTANCE;  /// the only one instance initialized on load of the library
//#define DEB_UPDATE

#ifdef DEB_UPDATE
#include <Locale_Convert.h>
#endif

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
  static const Events_ID kReorderEvent = aLoop->eventByName(EVENT_ORDER_UPDATED);
  aLoop->registerListener(this, kReorderEvent);
  static const Events_ID kUpdatedSel = aLoop->eventByName(EVENT_UPDATE_SELECTION);
  aLoop->registerListener(this, kUpdatedSel);
  static const Events_ID kAutoRecomp = aLoop->eventByName(EVENT_AUTOMATIC_RECOMPUTATION_ENABLE);
  aLoop->registerListener(this, kAutoRecomp);

  //  Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
  myIsParamUpdated = false;
  myIsFinish = false;
  myIsProcessed = false;
  myIsPreviewBlocked = false;
  myUpdateBlocked = false;
}

bool Model_Update::addModified(FeaturePtr theFeature, FeaturePtr theReason) {

  if (!theFeature->data()->isValid())
    return false; // delete an extrusion created on the sketch


  bool isNotExecuted = theFeature->isPersistentResult() &&
    !std::dynamic_pointer_cast<Model_Document>((theFeature)->document())->executeFeatures();
  if (isNotExecuted) {
    redisplayWithResults(theFeature, ModelAPI_StateNothing, false); // redisplay even not executed
    if (!theReason.get()) // no reason => no construction reason
      return false;
    if (myNotPersistentRefs.find(theFeature) == myNotPersistentRefs.end()) {
      myNotPersistentRefs[theFeature].insert(theReason);
    } else {
      std::set<std::shared_ptr<ModelAPI_Feature> > aNewSet;
      aNewSet.insert(theReason);
      myNotPersistentRefs[theFeature] = aNewSet;
    }
    return false;
  }

  // update arguments for "apply button" state change
  if ((!theFeature->isPreviewNeeded() && !myIsFinish) || myIsPreviewBlocked) {
    if (theReason.get())
      myProcessOnFinish[theFeature].insert(theReason);
    else if (myProcessOnFinish.find(theFeature) == myProcessOnFinish.end())
      myProcessOnFinish[theFeature] = std::set<std::shared_ptr<ModelAPI_Feature> >();
#ifdef DEB_UPDATE
      std::wcout<<L"*** Add process on finish "<<theFeature->name()<<std::endl;
#endif
    // keeps the currently updated features to avoid infinitive cycling here: where feature on
    // "updateArguments" sends "updated" (in selection attribute) and goes here again
    static std::set<FeaturePtr> aCurrentlyUpdated;
    if (aCurrentlyUpdated.find(theFeature) == aCurrentlyUpdated.end()) {
      aCurrentlyUpdated.insert(theFeature);
      updateArguments(theFeature);
      aCurrentlyUpdated.erase(theFeature);
    }
    // make it without conditions otherwise the apply button may have a bad state
    theFeature->data()->execState(ModelAPI_StateDone);
    static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
     // need to be validated to update the "Apply" state if not previewed
    aFactory->validate(theFeature);

    // to redisplay split's arguments presentation, even result is not computed
    if (!theFeature->isPreviewNeeded()) {
      static Events_Loop* aLoop = Events_Loop::loop();
      static const Events_ID kRedisplayEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
      ModelAPI_EventCreator::get()->sendUpdated(theFeature, kRedisplayEvent);
      aLoop->flush(kRedisplayEvent);
    }

    if (!myIsPreviewBlocked)
      return true;
  }
  if (myModified.find(theFeature) != myModified.end()) {
    if (theReason.get()) {
#ifdef DEB_UPDATE
      //std::cout<<"*** Add already modified "
      //   <<theFeature->name()<<" reason "<<theReason->name()<<std::endl;
#endif
      myModified[theFeature].insert(theReason);
    }
    return true;
  }
  // do not add the disabled, but possibly the sub-elements are not disabled
  bool aIsDisabled = theFeature->isDisabled();
  if (!aIsDisabled) {
    std::set<std::shared_ptr<ModelAPI_Feature> > aNewSet;
    if (theFeature->data()->execState() == ModelAPI_StateMustBeUpdated ||
        theFeature->data()->execState() == ModelAPI_StateInvalidArgument || // issue 1519
        theFeature->data()->execState() == ModelAPI_StateExecFailed) {
      // do not forget that in this case all were the reasons
      aNewSet.insert(theFeature);
    } else {
      if (theReason.get())
        aNewSet.insert(theReason);
    }
    myModified[theFeature] = aNewSet;
#ifdef DEB_UPDATE
    if (theReason.get()) {
      //std::cout<<"*** Add modified "<<theFeature->name()
      //  <<" reason "<<theReason->name()<<std::endl;
    } else {
      //std::cout<<"*** Add modified "<<theFeature->name()<<std::endl;
    }
#endif
  } else { // will be updated during the finish of the operation, or when it becomes enabled
    if (theFeature->data()->execState() == ModelAPI_StateDone ||
        theFeature->data()->execState() == ModelAPI_StateExecFailed) // fix issue 1819
      theFeature->data()->execState(ModelAPI_StateMustBeUpdated);
    else
      return true; // do not need iteration deeply if it is already marked as modified or so
#ifdef DEB_UPDATE
    //std::cout<<"*** Set modified state "<<theFeature->name()<<std::endl;
#endif
  }
  // clear processed and fill modified recursively
  std::set<FeaturePtr> aRefSet;
  const std::set<std::shared_ptr<ModelAPI_Attribute> >& aRefs = theFeature->data()->refsToMe();
  std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aRefIter = aRefs.cbegin();
  for(; aRefIter != aRefs.cend(); aRefIter++) {
    if ((*aRefIter)->isArgument()) {
      FeaturePtr aReferenced = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIter)->owner());
      if (aReferenced.get()) {
        aRefSet.insert(aReferenced);
      }
    }
  }
  // process also results
  std::list<ResultPtr> allResults; // list of this feature and results
  ModelAPI_Tools::allResults(theFeature, allResults);
  std::list<ResultPtr>::iterator aRes = allResults.begin();
  for(; aRes != allResults.end(); aRes++) {
    const std::set<std::shared_ptr<ModelAPI_Attribute> >& aResRefs = (*aRes)->data()->refsToMe();
    std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aRIter = aResRefs.cbegin();
    for(; aRIter != aResRefs.cend(); aRIter++) {
      if ((*aRIter)->isArgument()) {
        FeaturePtr aReferenced = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRIter)->owner());
        if (aReferenced.get()) {
          aRefSet.insert(aReferenced);
        }
      }
    }
  }
  // also add part feature that contains this feature to the modified
  if (theFeature->document()->kind() != "PartSet") {
    FeaturePtr aPart = ModelAPI_Tools::findPartFeature(
      ModelAPI_Session::get()->moduleDocument(), theFeature->document());
    if (aPart.get())
      aRefSet.insert(aPart);
  }
  for(std::set<FeaturePtr>::iterator aRef = aRefSet.begin(); aRef != aRefSet.end(); aRef++)
    addModified(*aRef, theFeature);

  return true;
}

void Model_Update::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  static Events_Loop* aLoop = Events_Loop::loop();
  //static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
  static const Events_ID kCreatedEvent = aLoop->eventByName(EVENT_OBJECT_CREATED);
  static const Events_ID kUpdatedEvent = aLoop->eventByName(EVENT_OBJECT_UPDATED);
  static const Events_ID kOpFinishEvent = aLoop->eventByName("FinishOperation");
  static const Events_ID kOpAbortEvent = aLoop->eventByName("AbortOperation");
  static const Events_ID kOpStartEvent = aLoop->eventByName("StartOperation");
  static const Events_ID kStabilityEvent = aLoop->eventByName(EVENT_STABILITY_CHANGED);
  static const Events_ID kPreviewBlockedEvent = aLoop->eventByName(EVENT_PREVIEW_BLOCKED);
  static const Events_ID kPreviewRequestedEvent = aLoop->eventByName(EVENT_PREVIEW_REQUESTED);
  static const Events_ID kReorderEvent = aLoop->eventByName(EVENT_ORDER_UPDATED);
  static const Events_ID kRedisplayEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const Events_ID kUpdatedSel = aLoop->eventByName(EVENT_UPDATE_SELECTION);

#ifdef DEB_UPDATE
  std::cout<<"****** Event "<<theMessage->eventID().eventText()<<std::endl;
#endif
  // check the automatic update flag on any event
  bool aNewAutomaticState = ModelAPI_Session::get()->isAutoUpdateBlocked();
  if (myUpdateBlocked != aNewAutomaticState) {
    myUpdateBlocked = aNewAutomaticState;
    if (!myUpdateBlocked) { // process all modified features, even if preview is blocked
      bool aPreviewBlockedState = myIsPreviewBlocked; // to update the selected arguments
      myIsPreviewBlocked = false;
      // iterate everything and add features in state "MustBeUpdated" into modified
      std::list<std::shared_ptr<ModelAPI_Document> > allDocs =
        ModelAPI_Session::get()->allOpenedDocuments();
      std::list<std::shared_ptr<ModelAPI_Document> >::iterator aDoc = allDocs.begin();
      for(; aDoc != allDocs.end(); aDoc++) {
        std::list<std::shared_ptr<ModelAPI_Feature> > allFeats = (*aDoc)->allFeatures();
        std::list<std::shared_ptr<ModelAPI_Feature> >::iterator aFeat = allFeats.begin();
        for(; aFeat != allFeats.end(); aFeat++) {
          if ((*aFeat)->data()->isValid() &&
            (*aFeat)->data()->execState() == ModelAPI_StateMustBeUpdated) {
            addModified(*aFeat, FeaturePtr());
          }
        }
      }
      processFeatures();
      myIsPreviewBlocked = aPreviewBlockedState;
    }
  }

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
      bool anUpdateState = myUpdateBlocked;
      myUpdateBlocked = false;
      myIsPreviewBlocked = false;
      processFeatures();
      myIsPreviewBlocked = true;
      myUpdateBlocked = anUpdateState;
    }
    return;
  }
  if (theMessage->eventID() == kUpdatedSel) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    updateSelection(aMsg->objects());
  }
  // creation is added to "update" to avoid recomputation twice:
  // on create and immediately after on update
  if (theMessage->eventID() == kCreatedEvent) {
    std::shared_ptr<ModelAPI_ObjectUpdatedMessage> aMsg =
        std::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    const std::set<ObjectPtr>& anObjs = aMsg->objects();
    std::set<ObjectPtr>::const_iterator anObjIter = anObjs.cbegin();
    std::list<ObjectPtr> aFeatures, aResults;
    for(; anObjIter != anObjs.cend(); anObjIter++) {
      if (std::dynamic_pointer_cast<Model_Document>((*anObjIter)->document())->executeFeatures()) {
        if ((*anObjIter)->groupName() == ModelAPI_Feature::group()) {
          // results creation means enabling, not update
          aFeatures.push_back(*anObjIter);
        } else {
          aResults.push_back(*anObjIter);
          ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*anObjIter);
          if (aPart.get() && aPart->data().get() && aPart->data()->isValid()) {
            aPart->shape(); // to update the shape on creation of the result
          }
        }
      }
    }
    ModelAPI_EventCreator::get()->sendUpdated(aFeatures, kUpdatedEvent);
    ModelAPI_EventCreator::get()->sendUpdated(aResults, kRedisplayEvent);
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
      std::wcout<<L">>> in event updated "<<Locale::Convert::toWString((*anObjIter)->groupName())
                <<L" "<<(*anObjIter)->data()->name()<<std::endl;
#endif
      if ((*anObjIter)->groupName() == ModelAPI_ResultParameter::group()) {
        myIsParamUpdated = true;
      }
      // on undo/redo, abort do not update persistent features
      FeaturePtr anUpdated = std::dynamic_pointer_cast<ModelAPI_Feature>(*anObjIter);
      if (anUpdated.get()) {
        if (addModified(anUpdated, FeaturePtr()))
          aSomeModified = true;
      } else {
        // process the updated result as update of features that refers to this result
        const std::set<std::shared_ptr<ModelAPI_Attribute> >&
          aRefs = (*anObjIter)->data()->refsToMe();
        std::set<std::shared_ptr<ModelAPI_Attribute> >::const_iterator aRefIter = aRefs.cbegin();
        FeaturePtr aReason;
        ResultPtr aReasonResult = std::dynamic_pointer_cast<ModelAPI_Result>(*anObjIter);
        if (aReasonResult.get())
          aReason = (*anObjIter)->document()->feature(aReasonResult);
        for(; aRefIter != aRefs.cend(); aRefIter++) {
          if (!(*aRefIter)->owner()->data()->isValid())
            continue;
          anUpdated = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIter)->owner());
          if (anUpdated.get()) {
            if (addModified(anUpdated, aReason))
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

    if (theMessage->eventID() == kOpFinishEvent) {// if update is blocked, skip
      myIsFinish = true;
      // add features that wait for finish as modified
      std::map<std::shared_ptr<ModelAPI_Feature>, std::set<std::shared_ptr<ModelAPI_Feature> > >::
        iterator aFeature = myProcessOnFinish.begin();
      for(; aFeature != myProcessOnFinish.end(); aFeature++) {
        if (aFeature->first->data()->isValid()) {// there may be already removed while wait
          if (aFeature->second.empty()) {
            addModified(aFeature->first, FeaturePtr());
            continue;
          }
          std::set<std::shared_ptr<ModelAPI_Feature> >::iterator aReasons;
          for(aReasons = aFeature->second.begin(); aReasons != aFeature->second.end(); aReasons++){
            addModified(aFeature->first, *aReasons);
          }
        }
      }
      myIsFinish = false;
    }
    // processed features must be only on finish, so clear anyway (to avoid re-import on load)
    myProcessOnFinish.clear();

    // #2156: current must be sketch, left after the macro execution
    DocumentPtr anActiveDoc = ModelAPI_Session::get()->activeDocument();
    FeaturePtr aCurrent;
    if (anActiveDoc.get())
      aCurrent = anActiveDoc->currentFeature(false);

    if (!(theMessage->eventID() == kOpStartEvent)) {
      processFeatures(false);
    }

    if (anActiveDoc.get() && aCurrent.get() && aCurrent->data()->isValid()) {
      if (anActiveDoc->currentFeature(false) != aCurrent &&
          ModelAPI_Tools::compositeOwner(anActiveDoc->currentFeature(false)) == aCurrent)
        anActiveDoc->setCurrentFeature(aCurrent, false); // #2156 make the current feature back
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
    // the redisplay signal should be flushed in order
    // to erase the feature presentation in the viewer
    // if should be done after removeFeature() of document,
    // by this reason, upper processFeatures() do not perform this flush
    Events_Loop::loop()->flush(kRedisplayEvent);

    // in the end of transaction everything is updated, so clear the old objects
    //myIsParamUpdated = false; // to avoid problems in sprocket.py parameter update
    myWaitForFinish.clear();
  } else if (theMessage->eventID() == kReorderEvent) {
    std::shared_ptr<ModelAPI_OrderUpdatedMessage> aMsg =
      std::dynamic_pointer_cast<ModelAPI_OrderUpdatedMessage>(theMessage);
    if (aMsg->reordered().get())
      addModified(aMsg->reordered(), aMsg->reordered()); // to update all attributes
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

    // to update the object browser if something is updated/created during executions
    static Events_Loop* aLoop = Events_Loop::loop();
    static const Events_ID kCreatedEvent= aLoop->eventByName(EVENT_OBJECT_CREATED);
    aLoop->flush(kCreatedEvent);
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

// collects all the features this feature depends on: reasons
static void allReasons(FeaturePtr theFeature, std::set<FeaturePtr>& theReasons) {
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
        if (aDepFeat.get() && aDepFeat->data()->isValid()) {
          theReasons.insert(aDepFeat);
        }
      }
    }
  }
  if (theFeature->getKind() == "Part") {
    // part is not depended on its subs directly, but subs must be iterated anyway
    CompositeFeaturePtr aPart = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
    int aNum = aPart->numberOfSubs();
    for(int a = 0; a < aNum; a++) {
      FeaturePtr aSub = aPart->subFeature(a);
      if (aSub.get() && aSub->data()->isValid()) {
        theReasons.insert(aSub);
      }
    }
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

  // check this feature is not yet checked or processed
  bool aIsModified = myModified.find(theFeature) != myModified.end();
  if (!aIsModified && myIsFinish) { // get info about the modification for features without preview
    if (theFeature->data()->execState() == ModelAPI_StateMustBeUpdated) {
      aIsModified = true;
      std::set<std::shared_ptr<ModelAPI_Feature> > aNewSet;
      // contains itself, so, we don't know which was the reason and the reason is any
      aNewSet.insert(theFeature);
      myModified[theFeature] = aNewSet;
    }
  }

  if (myProcessed.find(theFeature) == myProcessed.end()) {
    myProcessed[theFeature] = 0;
  } else if (aIsModified) {
    int aCount = myProcessed[theFeature];
    if (aCount > 100) {
      // too many repetition of processing (in VS it may crash on 330 with stack overflow)
      Events_InfoMessage("Model_Update",
        "Feature '%1' is updated in infinitive loop").arg(theFeature->data()->name()).send();
      // to stop iteration
      myModified.clear();
      return false;
    }
    myProcessed[theFeature] = aCount + 1;
  }

#ifdef DEB_UPDATE
    std::wcout<<L"* Process feature "<<theFeature->name()<<std::endl;
#endif

  // update the sketch plane before the sketch sub-elements are recomputed
  // (otherwise sketch will update plane, modify subs, after executed, but with old subs edges)
    if (aIsModified && theFeature->getKind() == "Sketch") {
#ifdef DEB_UPDATE
      std::wcout << L"****** Update sketch args " << theFeature->name() << std::endl;
#endif
      AttributeSelectionPtr anExtSel = theFeature->selection("External");
      if (anExtSel.get()) {
        ResultPtr aContext = anExtSel->context();
        if (aContext.get() && aContext->document().get()) {
          FeaturePtr anExtBase = aContext->document()->feature(aContext);
          if (anExtBase.get()) {
            processFeature(anExtBase);
          }
          std::shared_ptr<GeomDataAPI_Point> anOrigin =
            std::dynamic_pointer_cast<GeomDataAPI_Point>(theFeature->attribute("Origin"));
          double anOX = anOrigin->x(), anOY = anOrigin->y(), anOZ = anOrigin->z();
          std::shared_ptr<GeomDataAPI_Dir> aDir =
            std::dynamic_pointer_cast<GeomDataAPI_Dir>(theFeature->attribute("DirX"));
          double aDX = aDir->x(), aDY = aDir->y(), aDZ = aDir->z();
          std::shared_ptr<GeomDataAPI_Dir> aNorm =
            std::dynamic_pointer_cast<GeomDataAPI_Dir>(theFeature->attribute("Norm"));
          double aNX = aNorm->x(), aNY = aNorm->y(), aNZ = aNorm->z();
          // update sketch plane
          updateArguments(theFeature);
          theFeature->attributeChanged("External"); // to recompute origin, direction and normal
          // check it is updated, so all must be changed
          if (anOrigin->x() != anOX || anOrigin->y() != anOY || anOrigin->z() != anOZ ||
              aDir->x() != aDX || aDir->y() != aDY || aDir->z() != aDZ ||
              aNorm->x() != aNX || aNorm->y() != aNY || aNorm->z() != aNZ)
          {
            std::set<FeaturePtr> aWholeR;
            allReasons(theFeature, aWholeR);
            std::set<FeaturePtr>::iterator aRIter = aWholeR.begin();
            for (; aRIter != aWholeR.end(); aRIter++) {
              if ((*aRIter)->data()->selection("External").get())
                (*aRIter)->attributeChanged("External");
            }
          }
        }
      }
    }

  if (!aIsModified) { // no modification is needed
    return false;
  }

  // evaluate parameter before the sub-elements update:
  // it updates dependencies on evaluation (#1085)
  if (theFeature->getKind() == "Parameter") {
    theFeature->execute();
  }

  bool isReferencedInvalid = false;
  // check all features this feature depended on (recursive call of updateFeature)
  std::set<FeaturePtr>& aReasons = myModified[theFeature];
  bool allSubsUsed = aReasons.find(theFeature) != aReasons.end();
  if (allSubsUsed) {
    // add all subs in aReasons and temporary remove "theFeature" to avoid processing itself
    allReasons(theFeature, aReasons);
    aReasons.erase(theFeature);
  }
  // take reasons one by one (they may be added during the feature process
  // (circle by the radius of sketch)
  std::set<FeaturePtr> aProcessedReasons;
  while(!aReasons.empty()) {
    FeaturePtr aReason = *(aReasons.begin());
#ifdef DEB_UPDATE
    //cout<<theFeature->name()<<" process next reason "<<aReason->name()<<endl;
#endif
    if (aReason != theFeature && (aReason)->data()->isValid()) {
      if (processFeature(aReason))
        aIsModified = true;
      // check validity of aReason once again because it may be removed by dependent feature
      // (e.g. by SketchPlugin_IntersectionPoint)
      if (!aReason->data()->isValid() ||
          aReason->data()->execState() == ModelAPI_StateInvalidArgument)
        isReferencedInvalid = true;
    }
    // searching for the next not used reason
    aProcessedReasons.insert(aReason);
    // check theFeature is still in the list of modified, because it may be removed sometimes
    // while updating SketchPlugin_Ellipse
    if (myModified.find(theFeature) != myModified.end())
      aReasons.erase(aReason);
    else
      break;
  }
  // restore the modified reasons: they will be used in the update of arguments
  if (allSubsUsed) { // restore theFeature in this set
    aProcessedReasons.insert(theFeature);
  }
  myModified[theFeature] = aProcessedReasons;

  // do not execute the composite that contains the current
  bool isPostponedMain = false;
  CompositeFeaturePtr aCompos = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (theFeature->getKind() == "ExtrusionSketch" && aCompos.get()) {
    CompositeFeaturePtr aCurrentOwner =
      ModelAPI_Tools::compositeOwner(theFeature->document()->currentFeature(false));
    isPostponedMain = aCurrentOwner.get() && aCompos->isSub(aCurrentOwner);
  } else if (theFeature->getKind() == "Sketch" &&
    std::dynamic_pointer_cast<Model_Document>((theFeature)->document())->executeFeatures()) {
    // send event that sketch is prepared to be recomputed
    static Events_ID anID = Events_Loop::eventByName("SketchPrepared");
    std::shared_ptr<Events_Message> aMsg(new Events_Message(anID, this));
    Events_Loop* aLoop = Events_Loop::loop();
    // in case it is finish operation, flush for the sketch other events (#2450)
    aLoop->flush(aLoop->eventByName(EVENT_OBJECT_UPDATED));
    aLoop->send(aMsg);
    // check that sub-elements of sketch are updated => sketch must be re-processed
    std::set<FeaturePtr> aWholeR;
    allReasons(theFeature, aWholeR);
    std::set<FeaturePtr>::iterator aRIter = aWholeR.begin();
    for(; aRIter != aWholeR.end(); aRIter++) {
      if (myModified.find(*aRIter) != myModified.end()) {
        processFeature(theFeature);
        return true;
      }
    }
  }

#ifdef DEB_UPDATE
  std::wcout<<L"Update args "<<theFeature->name()<<std::endl;
#endif
  // TestImport.py : after arguments are updated, theFeature may be removed
  if (!theFeature->data()->isValid())
    return false;
  // Update selection and parameters attributes first, before sub-features analysis (sketch plane).
  updateArguments(theFeature);

  // add this feature to the processed right now to be able remove it from this list on
  // update signal during this feature execution
  myModified.erase(theFeature);
  if (myNotPersistentRefs.find(theFeature) != myNotPersistentRefs.end())
    myNotPersistentRefs.erase(theFeature);
  if (theFeature->data()->execState() == ModelAPI_StateMustBeUpdated)
    theFeature->data()->execState(ModelAPI_StateDone);

  // this checking must be after the composite feature sub-elements processing:
  // composite feature status may depend on it's sub-elements
  if ((theFeature->data()->execState() == ModelAPI_StateInvalidArgument || isReferencedInvalid) &&
    theFeature->getKind() != "Part") {
      // don't disable Part because it will make disabled all the features
      // (performance and problems with the current feature)
  #ifdef DEB_UPDATE
    std::wcout<<L"Invalid args "<<theFeature->name()<<std::endl;
  #endif
    theFeature->eraseResults(false);
    redisplayWithResults(theFeature, ModelAPI_StateInvalidArgument); // result also must be updated
    return true; // so, feature is modified (results are erased)
  }

  // execute feature if it must be updated
  ModelAPI_ExecState aState = theFeature->data()->execState();
  if (aFactory->validate(theFeature)) {
    if (!isPostponedMain) {
      bool aDoExecute = true;
      if (myUpdateBlocked) {
        if (!theFeature->isStable() || (theFeature->getKind().size() > 6 &&
            theFeature->getKind().substr(0, 6) == "Sketch")) { // automatic update sketch elements
          aDoExecute = true;
        } else if (theFeature->results().size()) { // execute only not persistent results features
          aDoExecute = !theFeature->isPersistentResult();
        } else {
          aDoExecute = aState != ModelAPI_StateInvalidArgument;
        }
      }
      if (aDoExecute) {
        executeFeature(theFeature);
      } else {
        // store information that this feature must be executed later
        theFeature->data()->execState(ModelAPI_StateMustBeUpdated);
      }
    }
  } else {
    #ifdef DEB_UPDATE
      std::wcout<<L"Feature is not valid, erase results "<<theFeature->name()<<std::endl;
    #endif
    theFeature->eraseResults(false);
    redisplayWithResults(theFeature, ModelAPI_StateInvalidArgument); // result also must be updated
  }
  return true;
}

void Model_Update::redisplayWithResults(
  FeaturePtr theFeature, const ModelAPI_ExecState theState, bool theUpdateState)
{
  // make updated and redisplay all results
  static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);

  std::list<ResultPtr> allResults;
  ModelAPI_Tools::allResults(theFeature, allResults);
  std::list<ResultPtr>::iterator aRIter = allResults.begin();
  for (; aRIter != allResults.cend(); aRIter++) {
    std::shared_ptr<ModelAPI_Result> aRes = *aRIter;
    if (!aRes->isDisabled()) {
      // update state only for enabled results
      // (Placement Result Part may make the original Part Result as invalid)
      if (theUpdateState)
        aRes->data()->execState(theState);
    }
    if (theFeature->data()->updateID() > aRes->data()->updateID()) {
      aRes->data()->setUpdateID(theFeature->data()->updateID());
    }
    ModelAPI_EventCreator::get()->sendUpdated(aRes, EVENT_DISP);
  }
  // to redisplay "presentable" feature (for ex. distance constraint)
  ModelAPI_EventCreator::get()->sendUpdated(theFeature, EVENT_DISP);
  if (theUpdateState)
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
  std::list<AttributePtr> aRefs =
    theFeature->data()->attributes(ModelAPI_AttributeSelection::typeId());
  std::list<AttributePtr>::iterator aRefsIter = aRefs.begin();
  for (; aRefsIter != aRefs.end(); aRefsIter++) {
    std::shared_ptr<ModelAPI_AttributeSelection> aSel =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*aRefsIter);
    ObjectPtr aContext = aSel->context();
    // update argument only if the referenced object is ready to use
    if (aContext.get() && !aContext->isDisabled()) {
      if (isReason(theFeature, aContext)) {
        if (!aSel->update()) { // this must be done on execution since it may be long operation
          bool isObligatory = !aFactory->isNotObligatory(
            theFeature->getKind(), theFeature->data()->id(aSel)) &&
            aFactory->isCase(theFeature, theFeature->data()->id(aSel));
          if (isObligatory ||
            // #24260 : sketch plane was selected, but selection becomes wrong, make it invalid
            (theFeature->getKind() == "Sketch" && aSel->id() == "External"))
            aState = ModelAPI_StateInvalidArgument;
        }
      }
    } else if (aContext.get()) {
      // here it may be not obligatory, but if the reference is wrong, it should not be correct
      bool isObligatory = aFactory->isCase(theFeature, theFeature->data()->id(aSel));
      if (isObligatory)
        aState = ModelAPI_StateInvalidArgument;
    } else if (aSel->isInitialized()) {
      // #19703 : if sketch plane was selected, but after context disappears, it must become invalid
      aSel->update();
      if (aSel->isInvalid()) {
          aState = ModelAPI_StateInvalidArgument;
      }
    }
  }
  // update the selection list attributes if any
  aRefs = theFeature->data()->attributes(ModelAPI_AttributeSelectionList::typeId());
  for (aRefsIter = aRefs.begin(); aRefsIter != aRefs.end(); aRefsIter++) {
    std::shared_ptr<ModelAPI_AttributeSelectionList> aSelList =
      std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(*aRefsIter);
    // #19071 : avoid sending of update event in cycle
    bool aWasBlocked = theFeature->data()->blockSendAttributeUpdated(true);
    // List to keep the shared pointers while update is blocked
    // In messages sent through blockSendAttributeUpdated(), raw pointers are used
    std::list<AttributeSelectionPtr> anAttrList;

    // Apply the filters if present
    if (aSelList->filters().get()) {
      aSelList->applyFilters();
    } else {
      for(int a = aSelList->size() - 1; a >= 0; a--) {
        std::shared_ptr<ModelAPI_AttributeSelection> aSelAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(aSelList->value(a));
        if (aSelAttr) {
          ObjectPtr aContext = aSelAttr->context();
          // update argument only if the referenced object is ready to use
          if (aContext.get() && !aContext->isDisabled()) {
            if (isReason(theFeature, aContext)) {
              anAttrList.push_back(aSelAttr);
              if (!aSelAttr->update()) {
                bool isObligatory = !aFactory->isNotObligatory(
                  theFeature->getKind(), theFeature->data()->id(aSelList)) &&
                  aFactory->isCase(theFeature, theFeature->data()->id(aSelList));
                if (isObligatory)
                  aState = ModelAPI_StateInvalidArgument;
              }
            }
          } else if (aContext.get()) {
            // here it may be not obligatory, but if the reference is wrong, it should not be correct
            bool isObligatory = aFactory->isCase(theFeature, theFeature->data()->id(aSelList));
            if (isObligatory)
              aState = ModelAPI_StateInvalidArgument;
          }
        }
      }
    }

    if (!aWasBlocked)
      theFeature->data()->blockSendAttributeUpdated(false);
  }

  if (aState != ModelAPI_StateDone)
    theFeature->data()->execState(aState);
}

bool Model_Update::isReason(std::shared_ptr<ModelAPI_Feature>& theFeature,
     std::shared_ptr<ModelAPI_Object> theReason)
{
  std::map<std::shared_ptr<ModelAPI_Feature>, std::set<std::shared_ptr<ModelAPI_Feature> > >
    ::iterator aReasonsIt = myModified.find(theFeature);
  if (aReasonsIt != myModified.end()) {
    if (aReasonsIt->second.find(theFeature) != aReasonsIt->second.end())
      return true; // any is reason if it contains itself
    FeaturePtr aReasFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(theReason);
    if (!aReasFeat.get()) { // try to get feature of this result
      ResultPtr aReasRes = std::dynamic_pointer_cast<ModelAPI_Result>(theReason);
      if (aReasRes.get())
        aReasFeat = theReason->document()->feature(aReasRes);
    }
    if (aReasonsIt->second.find(aReasFeat) != aReasonsIt->second.end())
      return true;
  }
  // another try: postponed modification by not-persistences
  std::map<std::shared_ptr<ModelAPI_Feature>, std::set<std::shared_ptr<ModelAPI_Feature> > >
    ::iterator aNotPersist = myNotPersistentRefs.find(theFeature);
  if (aNotPersist != myNotPersistentRefs.end()) {
    FeaturePtr aReasFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(theReason);
    if (!aReasFeat.get()) { // try to get feature of this result
      ResultPtr aReasRes = std::dynamic_pointer_cast<ModelAPI_Result>(theReason);
      if (aReasRes.get())
        aReasFeat = theReason->document()->feature(aReasRes);
    }
    if (aNotPersist->second.find(aReasFeat) != aNotPersist->second.end())
      return true;
  }

  // this case only for not-previewed items update state, nothing is changed in args for it
  return false;
}

void Model_Update::executeFeature(FeaturePtr theFeature)
{
#ifdef DEB_UPDATE
  std::wcout<<L"Execute Feature "<<theFeature->name()<<std::endl;
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
    Events_InfoMessage("Model_Update",
      "Feature %1 has failed during the execution").arg(theFeature->getKind()).send();
  }
  // The macro feature has to be deleted in any case even its execution is failed
  myWaitForFinish.insert(theFeature);
  if (aState != ModelAPI_StateDone) {
    theFeature->eraseResults(false);
  }
  theFeature->data()->setUpdateID(ModelAPI_Session::get()->transactionID());
  redisplayWithResults(theFeature, aState);
}

// it is called on GUI edit of feature start
// LCOV_EXCL_START
void Model_Update::updateStability(void* theSender)
{
  static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
  if (theSender) {
    bool added = false; // object may have been created
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
          std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator
            aRefIt = aRefs.begin();
          for(; aRefIt != aRefs.end(); aRefIt++) {
            if (!aFactory->isConcealed(aFeatureSender->getKind(), aRefIt->first))
              // take into account only concealed references
              // (do not remove the sketch constraint and the edge on constraint edit)
              continue;
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
// LCOV_EXCL_STOP

void Model_Update::updateSelection(const std::set<std::shared_ptr<ModelAPI_Object> >& theObjects)
{
  std::set<std::shared_ptr<ModelAPI_Object> >::iterator anObj = theObjects.begin();
  for(; anObj != theObjects.end(); anObj++) {
    std::list<AttributePtr> aRefs =
      (*anObj)->data()->attributes(ModelAPI_AttributeSelection::typeId());
    std::list<AttributePtr>::iterator aRefsIter = aRefs.begin();
    for (; aRefsIter != aRefs.end(); aRefsIter++) {
      AttributeSelectionPtr aSel =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*aRefsIter);
      bool aRemove = false;
      aSel->updateInHistory(aRemove);
    }
    // update the selection list attributes if any
    aRefs = (*anObj)->data()->attributes(ModelAPI_AttributeSelectionList::typeId());
    for (aRefsIter = aRefs.begin(); aRefsIter != aRefs.end(); aRefsIter++) {
      std::set<int> aRemoveSet;
      std::shared_ptr<ModelAPI_AttributeSelectionList> aSelList =
        std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(*aRefsIter);
      // If the selList is built based on filters: clear and reapply the filter
      if (aSelList->filters().get()) {
        aSelList->applyFilters();
      }
      for(int a = aSelList->size() - 1; a >= 0; a--) {
        AttributeSelectionPtr aSelAttr =
          std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(aSelList->value(a));

        if (aSelAttr.get()) {
          bool aRemove = false;
          aSelAttr->updateInHistory(aRemove);
          if (aRemove) {
            aRemoveSet.insert(a);
          }
        }
      }
      aSelList->remove(aRemoveSet);
    }
  }
}
