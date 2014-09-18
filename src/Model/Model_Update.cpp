// File:        Model_Update.cxx
// Created:     25 Jun 2014
// Author:      Mikhail PONIKAROV

#include <Model_Update.h>
#include <Model_Document.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Result.h>
#include <ModelAPI_Validator.h>
#include <Events_Loop.h>
#include <Events_LongOp.h>
#include <Events_Error.h>
#include <Config_PropManager.h>

using namespace std;

Model_Update MY_INSTANCE;  /// the only one instance initialized on load of the library

Model_Update::Model_Update() : isCreated(false)
{
  static const Events_ID kChangedEvent = Events_Loop::loop()->eventByName("PreferenceChanged");
  Events_Loop::loop()->registerListener(this, kChangedEvent);
  static const Events_ID kRebuildEvent = Events_Loop::loop()->eventByName("Rebuild");
  Events_Loop::loop()->registerListener(this, kRebuildEvent);
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  Config_PropManager::registerProp("Model update", "automatic_rebuild", "Rebuild automatically",
                                   Config_Prop::Bool, "false");
  isAutomatic = Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
}

void Model_Update::processEvent(const boost::shared_ptr<Events_Message>& theMessage)
{
  static const Events_ID kChangedEvent = Events_Loop::loop()->eventByName("PreferenceChanged");
  static const Events_ID kRebuildEvent = Events_Loop::loop()->eventByName("Rebuild");
  bool isAutomaticChanged = false;
  if (theMessage->eventID() == kChangedEvent) { // automatic and manual rebuild flag is changed
    isAutomatic = 
      Config_PropManager::findProp("Model update", "automatic_rebuild")->value() == "true";
  } else if (theMessage->eventID() == kRebuildEvent) { // the rebuild command
    if (isAutomatic == false) {
      isAutomaticChanged = true;
      isAutomatic = true;
    }
  }

  if (isExecuted)
    return;  // nothing to do: it is executed now
  // execute just created features anyway
  isCreated = theMessage->eventID() == Events_Loop::eventByName(EVENT_OBJECT_CREATED);

  //Events_LongOp::start(this);
  isExecuted = true;
  list<boost::shared_ptr<ModelAPI_Document> > aDocs;
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
  // collect all documents involved into the update
  set<boost::shared_ptr<ModelAPI_Object> >::iterator aFIter = myInitial.begin();
  for (; aFIter != myInitial.end(); aFIter++) {
    aDocs.push_back((*aFIter)->document());
  }
  // iterate all features of features-documents to update them (including hidden)
  list<boost::shared_ptr<ModelAPI_Document> >::iterator aDIter = aDocs.begin();
  for (; aDIter != aDocs.end(); aDIter++) {
    int aNbFeatures = (*aDIter)->size(ModelAPI_Feature::group(), true);
    for (int aFIndex = 0; aFIndex < aNbFeatures; aFIndex++) {
      FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(
          (*aDIter)->object(ModelAPI_Feature::group(), aFIndex, true));
      if (aFeature)
        updateFeature(aFeature);
    }
  }
  myUpdated.clear();
  // flush
  static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  Events_Loop::loop()->flush(EVENT_DISP);
  //Events_LongOp::end(this);
  if (isAutomaticChanged) isAutomatic = false;
  isCreated = false;
  isExecuted = false;
}

bool Model_Update::updateFeature(FeaturePtr theFeature)
{
  // check it is already processed
  if (myUpdated.find(theFeature) != myUpdated.end())
    return myUpdated[theFeature];
  // check all features this feature depended on (recursive call of updateFeature)
  bool aMustbeUpdated = myInitial.find(theFeature) != myInitial.end();
  if (theFeature) {  // only real feature contains references to other objects
    if (theFeature->data()->mustBeUpdated()) aMustbeUpdated = true;
    // references
    list<boost::shared_ptr<ModelAPI_Attribute> > aRefs = theFeature->data()->attributes(
        ModelAPI_AttributeReference::type());
    list<boost::shared_ptr<ModelAPI_Attribute> >::iterator aRefsIter = aRefs.begin();
    for (; aRefsIter != aRefs.end(); aRefsIter++) {
      boost::shared_ptr<ModelAPI_Object> aSub = boost::dynamic_pointer_cast<
          ModelAPI_AttributeReference>(*aRefsIter)->value();
      if (updateObject(aSub)) {
        aMustbeUpdated = true;
      }
    }
    // reference to attribute or object
    list<boost::shared_ptr<ModelAPI_Attribute> > aRefAttrs = theFeature->data()->attributes(
        ModelAPI_AttributeRefAttr::type());
    for (aRefsIter = aRefAttrs.begin(); aRefsIter != aRefAttrs.end(); aRefsIter++) {
      boost::shared_ptr<ModelAPI_AttributeRefAttr> aRef = 
        boost::dynamic_pointer_cast<ModelAPI_AttributeRefAttr>(*aRefsIter);
      if (!aRef) continue;
      if (aRef->isObject()) {
        boost::shared_ptr<ModelAPI_Object> aSub = aRef->object();
        if (updateObject(aSub)) {
          aMustbeUpdated = true;
        }
      } else if (aRef->attr()) { // reference to the attribute
        boost::shared_ptr<ModelAPI_Object> aSub = aRef->attr()->owner();
        if (updateObject(aSub)) {
          aMustbeUpdated = true;
        }
      }
    }
    // lists of references
    aRefs = theFeature->data()->attributes(ModelAPI_AttributeRefList::type());
    for (aRefsIter = aRefs.begin(); aRefsIter != aRefs.end(); aRefsIter++) {
      list<ObjectPtr> aListRef = boost::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*aRefsIter)
          ->list();
      list<ObjectPtr>::iterator aListIter = aListRef.begin();
      for (; aListIter != aListRef.end(); aListIter++) {
        boost::shared_ptr<ModelAPI_Object> aSub = *aListIter;
        if (updateObject(aSub)) {
          aMustbeUpdated = true;
        }
      }
    }
    // execute feature if it must be updated
    if (aMustbeUpdated) {

      if (boost::dynamic_pointer_cast<Model_Document>(theFeature->document())->executeFeatures() ||
          !theFeature->isPersistentResult()) {
        ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
        if (aFactory->validate(theFeature)) {
          if (isAutomatic || (isCreated && myInitial.find(theFeature) != myInitial.end()) ||
            !theFeature->isPersistentResult() /* execute quick, not persistent results */) {
            try {
              theFeature->execute();
              theFeature->data()->mustBeUpdated(false);
            } catch(...) {
              Events_Error::send(
                "Feature " + theFeature->getKind() + " has failed during the execution");
              theFeature->eraseResults();
            }
          } else {
            theFeature->data()->mustBeUpdated(true);
            aMustbeUpdated = false;
          }
        } else {
          theFeature->eraseResults();
        }
      }
      // redisplay all results
      static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
      const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
      std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        boost::shared_ptr<ModelAPI_Result> aRes = *aRIter;
        myUpdated[aRes] = true;
        ModelAPI_EventCreator::get()->sendUpdated(aRes, EVENT_DISP);
      }
      // to redisplay "presentable" feature (for ex. distance constraint)
      ModelAPI_EventCreator::get()->sendUpdated(theFeature, EVENT_DISP);
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

bool Model_Update::updateObject(boost::shared_ptr<ModelAPI_Object> theObject)
{
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
  if (myInitial.find(theObject) != myInitial.end())
    return true;
  return false;  // nothing is known
}
