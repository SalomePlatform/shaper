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
#include <ModelAPI_Validator.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Session.h>
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
                                   Config_Prop::Bool, "false");
  isAutomatic = Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
}

void Model_Update::processEvent(const boost::shared_ptr<Events_Message>& theMessage)
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
  bool isAutomaticChanged = false;
  if (theMessage->eventID() == kChangedEvent) { // automatic and manual rebuild flag is changed
    isAutomatic = 
      Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
  } else if (theMessage->eventID() == kRebuildEvent) { // the rebuild command
    if (isAutomatic == false) {
      isAutomaticChanged = true;
      isAutomatic = true;
    }
  } else if (theMessage->eventID() == kCreatedEvent || theMessage->eventID() == kUpdatedEvent ||
    theMessage->eventID() == kMovedEvent) {
    boost::shared_ptr<ModelAPI_ObjectUpdatedMessage> aMsg =
        boost::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
    const std::set<ObjectPtr>& anObjs = aMsg->objects();
    std::set<ObjectPtr>::const_iterator anObjIter = anObjs.cbegin();
    for(; anObjIter != anObjs.cend(); anObjIter++) {
      myJustCreatedOrUpdated.insert(*anObjIter);
    }
    if (theMessage->eventID() == kMovedEvent)
      return; // this event is for solver update, not here
  } else if (theMessage->eventID() == kOpStartEvent) {
    myJustCreatedOrUpdated.clear();
    return; // we don't need the update only on operation start (caused problems in PartSet_Listener::processEvent)
  } else if (theMessage->eventID() == kOpFinishEvent || theMessage->eventID() == kOpAbortEvent) {
    if (isAutomatic == false) { // Apply button now works as "Rebuild"
      isAutomaticChanged = true;
      isAutomatic = true;
    }
  }

  if (isExecuted)
    return;  // nothing to do: it is executed now

  //Events_LongOp::start(this);
  isExecuted = true;
  std::list<boost::shared_ptr<ModelAPI_Document> > aDocs;
  boost::shared_ptr<ModelAPI_ObjectUpdatedMessage> aMsg =
      boost::dynamic_pointer_cast<ModelAPI_ObjectUpdatedMessage>(theMessage);
  if (aMsg) myInitial = aMsg->objects();
  else {
    myInitial.clear();
    // on change flag all documents must be updated
    if (isAutomatic) {
      aDocs = ModelAPI_Session::get()->allOpenedDocuments();
    }
  }
  // collect all documents involved into the update process
  set<boost::shared_ptr<ModelAPI_Object> >::iterator aFIter = myInitial.begin();
  for (; aFIter != myInitial.end(); aFIter++) {
    aDocs.push_back((*aFIter)->document());
  }
  // iterate all features of features-documents to update them (including hidden)
  std::set<boost::shared_ptr<ModelAPI_Document> > alreadyUsed;
  list<boost::shared_ptr<ModelAPI_Document> >::iterator aDIter = aDocs.begin();
  for (; aDIter != aDocs.end(); aDIter++) {
    if (alreadyUsed.find(*aDIter) != alreadyUsed.end())
      continue;
    alreadyUsed.insert(*aDIter);
    int aNbFeatures = (*aDIter)->size(ModelAPI_Feature::group(), true);
    for (int aFIndex = 0; aFIndex < aNbFeatures; aFIndex++) {
      FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(
          (*aDIter)->object(ModelAPI_Feature::group(), aFIndex, true));
      if (aFeature)
        updateFeature(aFeature);
    }
  }
  myUpdated.clear();
  // flush to update display
  static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  aLoop->flush(EVENT_DISP);
  //Events_LongOp::end(this);
  if (isAutomaticChanged) isAutomatic = false;

  if (theMessage->eventID() == kOpFinishEvent || theMessage->eventID() == kOpAbortEvent) {
    myJustCreatedOrUpdated.clear();
  }

  isExecuted = false;
}

void Model_Update::redisplayWithResults(FeaturePtr theFeature) {
  // maske updated and redisplay all results
  static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
  std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
  for (; aRIter != aResults.cend(); aRIter++) {
    boost::shared_ptr<ModelAPI_Result> aRes = *aRIter;
    aRes->data()->mustBeUpdated(false);
    myUpdated[aRes] = true;
    ModelAPI_EventCreator::get()->sendUpdated(aRes, EVENT_DISP);
  }
  // to redisplay "presentable" feature (for ex. distance constraint)
  ModelAPI_EventCreator::get()->sendUpdated(theFeature, EVENT_DISP);
  theFeature->data()->mustBeUpdated(false);
}

bool Model_Update::updateFeature(FeaturePtr theFeature)
{
  // check it is already processed
  if (myUpdated.find(theFeature) != myUpdated.end())
    return myUpdated[theFeature];
  // check all features this feature depended on (recursive call of updateFeature)
  ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
  bool aMustbeUpdated = myInitial.find(theFeature) != myInitial.end();
  if (theFeature) {  // only real feature contains references to other objects
    if (theFeature->data()->mustBeUpdated()) aMustbeUpdated = true;

    // composite feature must be executed after sub-features execution
    CompositeFeaturePtr aComposite = 
      boost::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
    if (aComposite) {
      int aSubsNum = aComposite->numberOfSubs();
      for(int a = 0; a < aSubsNum; a++) {
        if (updateFeature(aComposite->subFeature(a)))
          aMustbeUpdated = true;
      }
      if (aMustbeUpdated) {
        for(int a = 0; a < aSubsNum; a++) {
          if (aComposite->subFeature(a) && aFactory->validate(aComposite->subFeature(a)))
            aComposite->subFeature(a)->execute();
        }
      }
    }
    // check all references: if referenced objects are updated, this object also must be updated
    std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
    aData->referencesToObjects(aRefs);
    std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRef = aRefs.begin();
    for(; aRef != aRefs.end(); aRef++) {
      std::list<ObjectPtr>::iterator aRefObj = aRef->second.begin();
      for(; aRefObj != aRef->second.end(); aRefObj++) {
        if (updateObject(*aRefObj)) {
          aMustbeUpdated = true;
        }
      }
    }

    // execute feature if it must be updated
    if (aMustbeUpdated) {
      if (boost::dynamic_pointer_cast<Model_Document>(theFeature->document())->executeFeatures() ||
          !theFeature->isPersistentResult()) {
        if (aFactory->validate(theFeature)) {
          if (isAutomatic || (myJustCreatedOrUpdated.find(theFeature) != myJustCreatedOrUpdated.end()) ||
            !theFeature->isPersistentResult() /* execute quick, not persistent results */) 
          {
            // before execution update the selection attributes if any
            list<AttributePtr> aRefs = 
              theFeature->data()->attributes(ModelAPI_AttributeSelection::type());
            list<AttributePtr>::iterator aRefsIter = aRefs.begin();
            for (; aRefsIter != aRefs.end(); aRefsIter++) {
              boost::shared_ptr<ModelAPI_AttributeSelection> aSel =
                boost::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*aRefsIter);
              aSel->update(); // this must be done on execution since it may be long operation
            }
            aRefs = theFeature->data()->attributes(ModelAPI_AttributeSelectionList::type());
            for (aRefsIter = aRefs.begin(); aRefsIter != aRefs.end(); aRefsIter++) {
              boost::shared_ptr<ModelAPI_AttributeSelectionList> aSel =
                boost::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(*aRefsIter);
              for(int a = aSel->size() - 1; a >= 0; a--) {
                  aSel->value(a)->update();
              }
            }
            // execute in try-catch to avoid internal problems of the feature
            try {
              theFeature->execute();
            } catch(...) {
              Events_Error::send(
                "Feature " + theFeature->getKind() + " has failed during the execution");
              theFeature->eraseResults();
            }
            redisplayWithResults(theFeature);
          } else { // must be updatet, but not updated yet
            theFeature->data()->mustBeUpdated(true);
            const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
            std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
            for (; aRIter != aResults.cend(); aRIter++) {
              boost::shared_ptr<ModelAPI_Result> aRes = *aRIter;
              aRes->data()->mustBeUpdated(true);
            }
          }
        } else {
          theFeature->eraseResults();
          redisplayWithResults(theFeature); // result also must be updated
        }
      } else { // for automatically updated features (on abort, etc) it is necessary to redisplay anyway
        redisplayWithResults(theFeature);
      }
    } else {  // returns also true is results were updated: for sketch that refers to sub-features but results of sub-features were changed
      const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
      std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        if (myInitial.find(*aRIter) != myInitial.end()) {
          aMustbeUpdated = true;
          break;
        }
      }
    }
  }
  myUpdated[theFeature] = aMustbeUpdated;
  return aMustbeUpdated;
}

bool Model_Update::updateObject(boost::shared_ptr<ModelAPI_Object> theObject, const bool theCyclic)
{
  if (myUpdated.find(theObject) != myUpdated.end())
    return myUpdated[theObject];  // already processed

  /*
  if (theCyclic) { // algorithm for update of all features by dependencies tree
    if (!theObject)
      return false;
    FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(theObject);
    if (aFeature) {  // for feature just call update Feature
      return updateFeature(aFeature);
    }
    // check general object, possible just a result
    if (myUpdated.find(theObject) != myUpdated.end())
      return myUpdated[theObject];  // already processed
    // check the feature of this object must be executed
    ResultPtr aResult = boost::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult) {
      FeaturePtr aResFeature = aResult->document()->feature(aResult);
      if (aResFeature) {
        return updateFeature(aResFeature);
      }
    }
  }
  */
  return myInitial.find(theObject) != myInitial.end();
}
