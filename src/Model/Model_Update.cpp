// File:        Model_Update.cxx
// Created:     25 Jun 2014
// Author:      Mikhail PONIKAROV

#include <Model_Update.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Document.h>
#include <ModelAPI_Events.h>
#include <ModelAPI_AttributeReference.h>
#include <ModelAPI_AttributeRefList.h>
#include <Events_Loop.h>

using namespace std;

Model_Update MY_INSTANCE; /// the only one instance initialized on load of the library

Model_Update::Model_Update()
{
  Events_Loop::loop()->registerListener(this, Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
}

void Model_Update::processEvent(const Events_Message* theMessage)
{
  const ModelAPI_ObjectUpdatedMessage* aMsg = 
    dynamic_cast<const ModelAPI_ObjectUpdatedMessage*>(theMessage);
  myInitial = aMsg->features();
  // collect all documents involved into the update
  set<boost::shared_ptr<ModelAPI_Document> > aDocs;
  set<boost::shared_ptr<ModelAPI_Object> >::iterator aFIter = myInitial.begin();
  for(; aFIter != myInitial.end(); aFIter++) {
    aDocs.insert((*aFIter)->document());
  }
  // iterate all features of features-documents to update them
  set<boost::shared_ptr<ModelAPI_Document> >::iterator aDIter = aDocs.begin();
  for(; aDIter != aDocs.end(); aDIter++) {
    int aNbFeatures = (*aDIter)->size(FEATURES_GROUP);
    for(int aFIndex = 0; aFIndex < aNbFeatures; aFIndex++) {
      boost::shared_ptr<ModelAPI_Feature> aFeature = 
        boost::dynamic_pointer_cast<ModelAPI_Feature>((*aDIter)->object(FEATURES_GROUP, aFIndex));
      if (aFeature)
        updateFeature(aFeature);
    }
  }
  myUpdated.clear();
  // flush
  static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  Events_Loop::loop()->flush(EVENT_DISP);
}

bool Model_Update::updateFeature(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  // check it is already processed
  if (myUpdated.find(theFeature) != myUpdated.end())
    return myUpdated[theFeature];
  // check all features this feature depended on (recursive call of updateFeature)
  bool aMustbeUpdated = myInitial.find(theFeature) != myInitial.end();
  // references
  list<boost::shared_ptr<ModelAPI_Attribute> > aRefs = 
    theFeature->data()->attributes(ModelAPI_AttributeReference::type());
  list<boost::shared_ptr<ModelAPI_Attribute> >::iterator aRefsIter = aRefs.begin();
  for(; aRefsIter != aRefs.end(); aRefsIter++) {
    boost::shared_ptr<ModelAPI_Feature> aSub =
      boost::dynamic_pointer_cast<ModelAPI_AttributeReference>(*aRefsIter)->value();
    if (aSub && aSub != theFeature && updateFeature(aSub))
      aMustbeUpdated = true;
  }
  // lists of references
  aRefs = theFeature->data()->attributes(ModelAPI_AttributeRefList::type());
  for(aRefsIter = aRefs.begin(); aRefsIter != aRefs.end(); aRefsIter++) {
    list<FeaturePtr> aListRef = 
      boost::dynamic_pointer_cast<ModelAPI_AttributeRefList>(*aRefsIter)->list();
    list<FeaturePtr>::iterator aListIter = aListRef.begin();
    for(; aListIter != aListRef.end(); aListIter++) {
      boost::shared_ptr<ModelAPI_Feature> aSub = *aListIter;
      if (aSub && updateFeature(aSub))
        aMustbeUpdated = true;
    }
  }
  // execute feature if it must be updated
  bool anExecute = aMustbeUpdated || myInitial.find(theFeature) != myInitial.end();
  if (anExecute) {
    theFeature->execute();
    static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    ModelAPI_EventCreator::get()->sendUpdated(theFeature, EVENT_DISP);
  }
  myUpdated[theFeature] = anExecute;
  return anExecute;
}
