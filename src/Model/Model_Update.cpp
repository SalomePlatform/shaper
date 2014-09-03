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
#include <ModelAPI_Result.h>
#include <Events_Loop.h>
#include <Events_LongOp.h>

using namespace std;

Model_Update MY_INSTANCE;  /// the only one instance initialized on load of the library

Model_Update::Model_Update()
{
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
}

void Model_Update::processEvent(const Events_Message* theMessage)
{
  if (isExecuted)
    return;  // nothing to do: it is executed now
  //Events_LongOp::start(this);
  isExecuted = true;
  const ModelAPI_ObjectUpdatedMessage* aMsg =
      dynamic_cast<const ModelAPI_ObjectUpdatedMessage*>(theMessage);
  myInitial = aMsg->objects();
  // collect all documents involved into the update
  set<boost::shared_ptr<ModelAPI_Document> > aDocs;
  set<boost::shared_ptr<ModelAPI_Object> >::iterator aFIter = myInitial.begin();
  for (; aFIter != myInitial.end(); aFIter++) {
    aDocs.insert((*aFIter)->document());
  }
  // iterate all features of features-documents to update them (including hidden)
  set<boost::shared_ptr<ModelAPI_Document> >::iterator aDIter = aDocs.begin();
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
          !theFeature->isPersistentResult())
        theFeature->execute();
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
