// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Objects.cxx
// Created:     15 May 2015
// Author:      Mikhail PONIKAROV

#include <Model_Objects.h>
#include <Model_Data.h>
#include <Model_Document.h>
#include <Model_Events.h>
#include <Model_Session.h>
#include <Model_ResultPart.h>
#include <Model_ResultConstruction.h>
#include <Model_ResultBody.h>
#include <Model_ResultCompSolid.h>
#include <Model_ResultGroup.h>
#include <Model_ResultParameter.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>

#include <Events_Loop.h>
#include <Events_Error.h>

#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_HLabelArray1.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Reference.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_LabelMapHasher.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>

static const int TAG_OBJECTS = 2;  // tag of the objects sub-tree (features, results)

// feature sub-labels
static const int TAG_FEATURE_ARGUMENTS = 1;  ///< where the arguments are located
static const int TAG_FEATURE_RESULTS = 2;  ///< where the results are located

///
/// 0:1:2 - where features are located
/// 0:1:2:N:1 - data of the feature N
/// 0:1:2:N:2:K:1 - data of the K result of the feature N

Model_Objects::Model_Objects(TDF_Label theMainLab) : myMain(theMainLab)
{
}

void Model_Objects::setOwner(DocumentPtr theDoc)
{
  myDoc = theDoc;
  // update all fields and recreate features and result objects if needed
  TDF_LabelList aNoUpdated;
  synchronizeFeatures(aNoUpdated, true, true);
  myHistory.clear();
}

Model_Objects::~Model_Objects()
{
  // delete all features of this document
  Events_Loop* aLoop = Events_Loop::loop();
  // erase one by one to avoid access from the feature destructor itself from he map
  // blocks the flush signals to avoid the temporary objects visualization in the viewer
  // they should not be shown in order to do not lose highlight by erasing them
  bool isActive = aLoop->activateFlushes(false);

  while(!myFeatures.IsEmpty()) {
    NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFeaturesIter(myFeatures);
    FeaturePtr aFeature = aFeaturesIter.Value();
    static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    ModelAPI_EventCreator::get()->sendDeleted(myDoc, ModelAPI_Feature::group());
    ModelAPI_EventCreator::get()->sendUpdated(aFeature, EVENT_DISP);
    aFeature->removeResults(0, false);
    //aFeature->eraseResults();
    aFeature->erase();
    myFeatures.UnBind(aFeaturesIter.Key());
  }
  aLoop->activateFlushes(isActive);
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));

}

/// Appends to the array of references a new referenced label
static void AddToRefArray(TDF_Label& theArrayLab, TDF_Label& theReferenced, TDF_Label& thePrevLab)
{
  Handle(TDataStd_ReferenceArray) aRefs;
  if (!theArrayLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    aRefs = TDataStd_ReferenceArray::Set(theArrayLab, 0, 0);
    aRefs->SetValue(0, theReferenced);
  } else {  // extend array by one more element
    Handle(TDataStd_HLabelArray1) aNewArray = new TDataStd_HLabelArray1(aRefs->Lower(),
                                                                        aRefs->Upper() + 1);
    int aPassedPrev = 0; // prev feature is found and passed
    if (thePrevLab.IsNull()) { // null means that inserted feature must be the first
      aNewArray->SetValue(aRefs->Lower(), theReferenced);
      aPassedPrev = 1;
    }
    for (int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
      aNewArray->SetValue(a + aPassedPrev, aRefs->Value(a));
      if (!aPassedPrev && aRefs->Value(a).IsEqual(thePrevLab)) {
        aPassedPrev = 1;
        aNewArray->SetValue(a + 1, theReferenced);
      }
    }
    if (!aPassedPrev) // not found: unknown situation
      aNewArray->SetValue(aRefs->Upper() + 1, theReferenced);
    aRefs->SetInternalArray(aNewArray);
  }
}

void Model_Objects::addFeature(FeaturePtr theFeature, const FeaturePtr theAfterThis)
{
  if (!theFeature->isAction()) {  // do not add action to the data model
    TDF_Label aFeaturesLab = featuresLabel();
    TDF_Label aFeatureLab = aFeaturesLab.NewChild();
    // store feature in the features array: before "initData" because in macro features
    // in initData it creates new features, appeared later than this
    TDF_Label aPrevFeateureLab;
    if (theAfterThis.get()) { // searching for the previous feature label
      std::shared_ptr<Model_Data> aPrevData = 
        std::dynamic_pointer_cast<Model_Data>(theAfterThis->data());
      if (aPrevData.get()) {
        aPrevFeateureLab = aPrevData->label().Father();
      }
    }
    AddToRefArray(aFeaturesLab, aFeatureLab, aPrevFeateureLab);

    // keep the feature ID to restore document later correctly
    TDataStd_Comment::Set(aFeatureLab, theFeature->getKind().c_str());
    myFeatures.Bind(aFeatureLab, theFeature);
    // must be after binding to the map because of "Box" macro feature that 
    // creates other features in "initData"
    initData(theFeature, aFeatureLab, TAG_FEATURE_ARGUMENTS);
    // event: feature is added
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
    ModelAPI_EventCreator::get()->sendUpdated(theFeature, anEvent);
    theFeature->setDisabled(false); // by default created feature is enabled
    updateHistory(ModelAPI_Feature::group());
  } else { // make feature has not-null data anyway
    theFeature->setData(Model_Data::invalidData());
    theFeature->setDoc(myDoc);
  }
}

/// Appends to the array of references a new referenced label.
/// If theIndex is not -1, removes element at this index, not theReferenced.
/// \returns the index of removed element
static int RemoveFromRefArray(TDF_Label theArrayLab, TDF_Label theReferenced, 
  const int theIndex = -1)
{
  int aResult = -1;  // no returned
  Handle(TDataStd_ReferenceArray) aRefs;
  if (theArrayLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    if (aRefs->Length() == 1) {  // just erase an array
      if ((theIndex == -1 && aRefs->Value(0) == theReferenced) || theIndex == 0) {
        theArrayLab.ForgetAttribute(TDataStd_ReferenceArray::GetID());
      }
      aResult = 0;
    } else {  // reduce the array
      Handle(TDataStd_HLabelArray1) aNewArray = new TDataStd_HLabelArray1(aRefs->Lower(),
                                                                          aRefs->Upper() - 1);
      int aCount = aRefs->Lower();
      for (int a = aCount; a <= aRefs->Upper(); a++, aCount++) {
        if ((theIndex == -1 && aRefs->Value(a) == theReferenced) || theIndex == a) {
          aCount--;
          aResult = a;
        } else {
          aNewArray->SetValue(aCount, aRefs->Value(a));
        }
      }
      aRefs->SetInternalArray(aNewArray);
    }
  }
  return aResult;
}

void Model_Objects::refsToFeature(FeaturePtr theFeature,
  std::set<std::shared_ptr<ModelAPI_Feature> >& theRefs, const bool isSendError)
{
  // check the feature: it must have no depended objects on it
  // the dependencies can be in the feature results
  std::list<ResultPtr>::const_iterator aResIter = theFeature->results().cbegin();
  for(; aResIter != theFeature->results().cend(); aResIter++) {
    ResultPtr aResult = (*aResIter);
    std::shared_ptr<Model_Data> aData = 
      std::dynamic_pointer_cast<Model_Data>(aResult->data());
    if (aData.get() != NULL) {
      const std::set<AttributePtr>& aRefs = aData->refsToMe();
      std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin(), aRefLast = aRefs.end();
      for(; aRefIt != aRefLast; aRefIt++) {
        FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIt)->owner());
        if (aFeature.get() != NULL)
          theRefs.insert(aFeature);
      }
    }
  }
  // the dependencies can be in the feature itself
  std::shared_ptr<Model_Data> aData = 
      std::dynamic_pointer_cast<Model_Data>(theFeature->data());
  if (aData && !aData->refsToMe().empty()) {
    const std::set<AttributePtr>& aRefs = aData->refsToMe();
    std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin(), aRefLast = aRefs.end();
    for(; aRefIt != aRefLast; aRefIt++) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIt)->owner());
      if (aFeature.get() != NULL)
        theRefs.insert(aFeature);
    }
  }

  if (!theRefs.empty() && isSendError) {
    Events_Error::send(
      "Feature '" + theFeature->data()->name() + "' is used and can not be deleted");
  }
}

void Model_Objects::removeFeature(FeaturePtr theFeature)
{
  std::shared_ptr<Model_Data> aData = std::static_pointer_cast<Model_Data>(theFeature->data());
  if (aData && aData->isValid()) {
    // checking that the sub-element of composite feature is removed: if yes, inform the owner
    std::set<std::shared_ptr<ModelAPI_Feature> > aRefs;
    refsToFeature(theFeature, aRefs, false);
    std::set<std::shared_ptr<ModelAPI_Feature> >::iterator aRefIter = aRefs.begin();
    for(; aRefIter != aRefs.end(); aRefIter++) {
      std::shared_ptr<ModelAPI_CompositeFeature> aComposite = 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*aRefIter);
      if (aComposite.get()) {
        aComposite->removeFeature(theFeature);
      }
    }
    // this must be before erase since theFeature erasing removes all information about
    // the feature results and groups of results
    // To reproduce: create sketch, extrusion, remove sketch => constructions tree is not updated
    clearHistory(theFeature);
    // erase fields
    theFeature->erase();

    TDF_Label aFeatureLabel = aData->label().Father();
    if (myFeatures.IsBound(aFeatureLabel))
      myFeatures.UnBind(aFeatureLabel);

    static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    ModelAPI_EventCreator::get()->sendUpdated(theFeature, EVENT_DISP);
    // erase all attributes under the label of feature
    aFeatureLabel.ForgetAllAttributes();
    // remove it from the references array
    RemoveFromRefArray(featuresLabel(), aFeatureLabel);
    // event: feature is deleted
    ModelAPI_EventCreator::get()->sendDeleted(theFeature->document(), ModelAPI_Feature::group());
    // the redisplay signal should be flushed in order to erase the feature presentation in the viewer
    Events_Loop::loop()->flush(EVENT_DISP);
    updateHistory(ModelAPI_Feature::group());
  }
}

void Model_Objects::moveFeature(FeaturePtr theMoved, FeaturePtr theAfterThis)
{
  TDF_Label aFeaturesLab = featuresLabel();
  Handle(TDataStd_ReferenceArray) aRefs;
  if (!aFeaturesLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
    return;
  TDF_Label anAfterLab, aMovedLab = 
    std::dynamic_pointer_cast<Model_Data>(theMoved->data())->label().Father();
  if (theAfterThis.get())
    anAfterLab = std::dynamic_pointer_cast<Model_Data>(theAfterThis->data())->label().Father();

  Handle(TDataStd_HLabelArray1) aNewArray = 
    new TDataStd_HLabelArray1(aRefs->Lower(), aRefs->Upper());
  int aPassedMovedFrom = 0; // the prev feature location is found and passed
  int aPassedMovedTo = 0; // the feature is added and this location is passed
  if (!theAfterThis.get()) { // null means that inserted feature must be the first
    aNewArray->SetValue(aRefs->Lower(), aMovedLab);
    aPassedMovedTo = 1;
  }
  for (int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
    if (aPassedMovedTo == 0 && aRefs->Value(a) == anAfterLab) { // add two
      aPassedMovedTo++;
      aNewArray->SetValue(a - aPassedMovedFrom, anAfterLab);
      if (a + 1 - aPassedMovedFrom <= aRefs->Upper())
        aNewArray->SetValue(a + 1 - aPassedMovedFrom, aMovedLab);
    } else if (aPassedMovedFrom == 0 && aRefs->Value(a) == aMovedLab) { // skip
      aPassedMovedFrom++;
    } else { // just copy one
      if (a - aPassedMovedFrom + aPassedMovedTo <= aRefs->Upper())
        aNewArray->SetValue(a - aPassedMovedFrom + aPassedMovedTo, aRefs->Value(a));
    }
  }
  if (!aPassedMovedFrom || !aPassedMovedTo) {// not found: unknown situation
    if (!aPassedMovedFrom) {
      static std::string aMovedFromError("The moved feature is not found");
      Events_Error::send(aMovedFromError);
    } else {
      static std::string aMovedToError("The 'after' feature for movement is not found");
      Events_Error::send(aMovedToError);
    }
    return;
  }
  // store the new array
  aRefs->SetInternalArray(aNewArray);
  // update the feature and the history
  clearHistory(theMoved);
  // make sure all (selection) attributes of moved feature will be updated
  theMoved->data()->setUpdateID(0);
  static Events_ID EVENT_UPD = Events_Loop::loop()->eventByName(EVENT_OBJECT_UPDATED);
  ModelAPI_EventCreator::get()->sendUpdated(theMoved, EVENT_UPD);
  ModelAPI_EventCreator::get()->sendReordered(theMoved->document(), theMoved->groupName());
}

void Model_Objects::clearHistory(ObjectPtr theObj)
{
  if (theObj) {
    const std::string aGroup = theObj->groupName();
    std::map<std::string, std::vector<ObjectPtr> >::iterator aHIter = myHistory.find(aGroup);
    if (aHIter != myHistory.end())
      myHistory.erase(aHIter); // erase from map => this means that it is not synchronized
    if (theObj->groupName() == ModelAPI_Feature::group()) { // clear results group of the feature
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
      std::string aResultGroup = featureResultGroup(aFeature);
      if (!aResultGroup.empty()) {
        std::map<std::string, std::vector<ObjectPtr> >::iterator aHIter = 
          myHistory.find(aResultGroup);
        if (aHIter != myHistory.end())
          myHistory.erase(aHIter); // erase from map => this means that it is not synchronized
      }
    }
  }
}

void Model_Objects::createHistory(const std::string& theGroupID)
{
  std::map<std::string, std::vector<ObjectPtr> >::iterator aHIter = myHistory.find(theGroupID);
  if (aHIter == myHistory.end()) {
    myHistory[theGroupID] = std::vector<ObjectPtr>();
    std::vector<ObjectPtr>& aResult = myHistory[theGroupID];
    // iterate the array of references and get feature by feature from the array
    bool isFeature = theGroupID == ModelAPI_Feature::group();
    Handle(TDataStd_ReferenceArray) aRefs;
    if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
      for(int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
        FeaturePtr aFeature = feature(aRefs->Value(a));
        if (aFeature.get()) {
          // if feature is in sub-component, remove it from history: it is in sub-tree of sub-component
          if (!ModelAPI_Tools::compositeOwner(aFeature).get()) {
            if (isFeature) { // here may be also disabled features
              if (aFeature->isInHistory()) {
                aResult.push_back(aFeature);
              }
            } else if (!aFeature->isDisabled()) { // iterate all results of not-disabled feature
              const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
              std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
              for (; aRIter != aResults.cend(); aRIter++) {
                ResultPtr aRes = *aRIter;
                if (aRes->groupName() != theGroupID) break; // feature have only same group results
                if (!aRes->isDisabled() && aRes->isInHistory() && !aRes->isConcealed()) {
                  aResult.push_back(*aRIter);
                }
              }
            }
          }
        }
      }
    }
  }
}

void Model_Objects::updateHistory(const std::shared_ptr<ModelAPI_Object> theObject)
{
  clearHistory(theObject);
}

void Model_Objects::updateHistory(const std::string theGroup)
{
  std::map<std::string, std::vector<ObjectPtr> >::iterator aHIter = myHistory.find(theGroup);
  if (aHIter != myHistory.end())
    myHistory.erase(aHIter); // erase from map => this means that it is not synchronized
}

FeaturePtr Model_Objects::feature(TDF_Label theLabel) const
{
  if (myFeatures.IsBound(theLabel))
    return myFeatures.Find(theLabel);
  return FeaturePtr();  // not found
}

ObjectPtr Model_Objects::object(TDF_Label theLabel)
{
  // try feature by label
  FeaturePtr aFeature = feature(theLabel);
  if (aFeature)
    return feature(theLabel);
  TDF_Label aFeatureLabel = theLabel.Father().Father();  // let's suppose it is result
  aFeature = feature(aFeatureLabel);
  bool isSubResult = false;
  if (!aFeature.get() && aFeatureLabel.Depth() > 1) { // let's suppose this is sub-result of result
    aFeatureLabel = aFeatureLabel.Father().Father();
    aFeature = feature(aFeatureLabel);
    isSubResult = true;
  }
  if (aFeature) {
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.cbegin();
    for (; aRIter != aResults.cend(); aRIter++) {
      if (isSubResult) {
        ResultCompSolidPtr aCompRes = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aRIter);
        if (aCompRes) {
          int aNumSubs = aCompRes->numberOfSubs();
          for(int a = 0; a < aNumSubs; a++) {
            ResultPtr aSub = aCompRes->subResult(a);
            if (aSub.get()) {
              std::shared_ptr<Model_Data> aSubData = std::dynamic_pointer_cast<Model_Data>(
                  aSub->data());
              if (aSubData->label().Father().IsEqual(theLabel))
                return aSub;
            }
          }
        }
      } else {
        std::shared_ptr<Model_Data> aResData = std::dynamic_pointer_cast<Model_Data>(
            (*aRIter)->data());
        if (aResData->label().Father().IsEqual(theLabel))
          return *aRIter;
      }
    }
  }
  return FeaturePtr();  // not found
}

ObjectPtr Model_Objects::object(const std::string& theGroupID, const int theIndex)
{
  if (theIndex == -1)
    return ObjectPtr();
  createHistory(theGroupID);
  return myHistory[theGroupID][theIndex];
}

std::shared_ptr<ModelAPI_Object> Model_Objects::objectByName(
    const std::string& theGroupID, const std::string& theName)
{
  createHistory(theGroupID);
  std::vector<ObjectPtr>& allObjs = myHistory[theGroupID];
  std::vector<ObjectPtr>::iterator anObjIter = allObjs.begin();
  for(; anObjIter != allObjs.end(); anObjIter++) {
    if ((*anObjIter)->data()->name() == theName)
      return *anObjIter;
  }
  // not found
  return ObjectPtr();
}

const int Model_Objects::index(std::shared_ptr<ModelAPI_Object> theObject)
{
  std::string aGroup = theObject->groupName();
  createHistory(aGroup);
  std::vector<ObjectPtr>& allObjs = myHistory[aGroup];
  std::vector<ObjectPtr>::iterator anObjIter = allObjs.begin(); // iterate to search object
  for(int anIndex = 0; anObjIter != allObjs.end(); anObjIter++, anIndex++) {
    if ((*anObjIter) == theObject)
      return anIndex;
  }
  // not found
  return -1;
}

int Model_Objects::size(const std::string& theGroupID)
{
  createHistory(theGroupID);
  return myHistory[theGroupID].size();
}

void Model_Objects::allResults(const std::string& theGroupID, std::list<ResultPtr>& theResults)
{
  // iterate the array of references and get feature by feature from the array
  Handle(TDataStd_ReferenceArray) aRefs;
  if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    for(int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
      FeaturePtr aFeature = feature(aRefs->Value(a));
      if (aFeature.get()) {
        const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
        std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
        for (; aRIter != aResults.cend(); aRIter++) {
          ResultPtr aRes = *aRIter;
          if (aRes->groupName() != theGroupID) break; // feature have only same group results
          // iterate also concealed: ALL RESULTS (for translation parts undo/redo management)
          //if (aRes->isInHistory() && !aRes->isConcealed()) {
            theResults.push_back(*aRIter);
          //}
        }
      }
    }
  }
}


TDF_Label Model_Objects::featuresLabel() const
{
  return myMain.FindChild(TAG_OBJECTS);
}

void Model_Objects::setUniqueName(FeaturePtr theFeature)
{
  if (!theFeature->data()->name().empty())
    return;  // not needed, name is already defined
  std::string aName;  // result
  // first count all features of such kind to start with index = count + 1
  int aNumObjects = -1; // this feature is already in this map
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFIter(myFeatures);
  for (; aFIter.More(); aFIter.Next()) {
    if (aFIter.Value()->getKind() == theFeature->getKind())
      aNumObjects++;
  }
  // generate candidate name
  std::stringstream aNameStream;
  aNameStream << theFeature->getKind() << "_" << aNumObjects + 1;
  aName = aNameStream.str();
  // check this is unique, if not, increase index by 1
  for (aFIter.Initialize(myFeatures); aFIter.More();) {
    FeaturePtr aFeature = aFIter.Value();
    bool isSameName = aFeature->data()->name() == aName;
    if (!isSameName) {  // check also results to avoid same results names (actual for Parts)
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        isSameName = (*aRIter)->data()->name() == aName;
      }
    }
    if (isSameName) {
      aNumObjects++;
      std::stringstream aNameStream;
      aNameStream << theFeature->getKind() << "_" << aNumObjects + 1;
      aName = aNameStream.str();
      // reinitialize iterator to make sure a new name is unique
      aFIter.Initialize(myFeatures);
    } else
      aFIter.Next();
  }
  theFeature->data()->setName(aName);
}

void Model_Objects::initData(ObjectPtr theObj, TDF_Label theLab, const int theTag)
{
  std::shared_ptr<Model_Data> aData(new Model_Data);
  aData->setLabel(theLab.FindChild(theTag));
  aData->setObject(theObj);
  theObj->setDoc(myDoc);
  theObj->setData(aData);
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if (aFeature) {
    setUniqueName(aFeature);  // must be before "initAttributes" because duplicate part uses name
  }
  theObj->initAttributes();
}

void Model_Objects::synchronizeFeatures(
  const TDF_LabelList& theUpdated, const bool theUpdateReferences, const bool theFlush)
{
  Model_Document* anOwner = std::dynamic_pointer_cast<Model_Document>(myDoc).get();
  if (!anOwner) // this may happen on creation of document: nothing there, so nothing to synchronize
    return;
  // after all updates, sends a message that groups of features were created or updated
  Events_Loop* aLoop = Events_Loop::loop();
  static Events_ID aDispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static Events_ID aCreateEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
  static Events_ID anUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static Events_ID aDeleteEvent = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
  static Events_ID aToHideEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  bool isActive = aLoop->activateFlushes(false);

  // collect all updated labels map
  TDF_LabelMap anUpdatedMap;
  TDF_ListIteratorOfLabelList anUpdatedIter(theUpdated);
  for(; anUpdatedIter.More(); anUpdatedIter.Next()) {
    TDF_Label& aFeatureLab = anUpdatedIter.Value();
    while(aFeatureLab.Depth() > 3)
      aFeatureLab = aFeatureLab.Father();
    if (myFeatures.IsBound(aFeatureLab))
      anUpdatedMap.Add(aFeatureLab);
  }

  // update all objects by checking are they on labels or not
  std::set<FeaturePtr> aNewFeatures, aKeptFeatures;
  TDF_ChildIDIterator aLabIter(featuresLabel(), TDataStd_Comment::GetID());
  for (; aLabIter.More(); aLabIter.Next()) {
    TDF_Label aFeatureLabel = aLabIter.Value()->Label();
    FeaturePtr aFeature;
    if (!myFeatures.IsBound(aFeatureLabel)) {  // a new feature is inserted
      // create a feature
      aFeature = std::dynamic_pointer_cast<Model_Session>(ModelAPI_Session::get())->createFeature(
        TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(aLabIter.Value())->Get())
        .ToCString(), anOwner);
      if (!aFeature) {  // somethig is wrong, most probably, the opened document has invalid structure
        Events_Error::send("Invalid type of object in the document");
        aLabIter.Value()->Label().ForgetAllAttributes();
        continue;
      }
      // this must be before "setData" to redo the sketch line correctly
      myFeatures.Bind(aFeatureLabel, aFeature);
      aNewFeatures.insert(aFeature);
      initData(aFeature, aFeatureLabel, TAG_FEATURE_ARGUMENTS);
      updateHistory(aFeature);
      aFeature->setDisabled(false); // by default created feature is enabled (this allows to recreate the results before "setCurrent" is called)

      // event: model is updated
      ModelAPI_EventCreator::get()->sendUpdated(aFeature, aCreateEvent);
    } else {  // nothing is changed, both iterators are incremented
      aFeature = myFeatures.Find(aFeatureLabel);
      aKeptFeatures.insert(aFeature);
      if (anUpdatedMap.Contains(aFeatureLabel)) {
        ModelAPI_EventCreator::get()->sendUpdated(aFeature, anUpdateEvent);
      }
    }
  }

  // check all features are checked: if not => it was removed
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFIter(myFeatures);
  while (aFIter.More()) {
    if (aKeptFeatures.find(aFIter.Value()) == aKeptFeatures.end()
      && aNewFeatures.find(aFIter.Value()) == aNewFeatures.end()) {
        FeaturePtr aFeature = aFIter.Value();
        // event: model is updated
        //if (aFeature->isInHistory()) {
        ModelAPI_EventCreator::get()->sendDeleted(myDoc, ModelAPI_Feature::group());
        //}
        // results of this feature must be redisplayed (hided)
        // redisplay also removed feature (used for sketch and AISObject)
        ModelAPI_EventCreator::get()->sendUpdated(aFeature, aRedispEvent);
        updateHistory(aFeature);
        aFeature->erase();
        // unbind after the "erase" call: on abort sketch is removes sub-objects that corrupts aFIter
        myFeatures.UnBind(aFIter.Key());
        // reinitialize iterator because unbind may corrupt the previous order in the map
        aFIter.Initialize(myFeatures);
    } else
      aFIter.Next();
  }

  if (theUpdateReferences) {
    synchronizeBackRefs();
  }
  // update results of the features (after features created because they may be connected, like sketch and sub elements)
  // After synchronisation of back references because sketch must be set in sub-elements before "execute" by updateResults
  std::list<FeaturePtr> aComposites; // composites must be updated after their subs (issue 360)
  TDF_ChildIDIterator aLabIter2(featuresLabel(), TDataStd_Comment::GetID());
  for (; aLabIter2.More(); aLabIter2.Next()) {
    TDF_Label aFeatureLabel = aLabIter2.Value()->Label();
    if (myFeatures.IsBound(aFeatureLabel)) {  // a new feature is inserted
      FeaturePtr aFeature = myFeatures.Find(aFeatureLabel);
      if (std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature).get())
        aComposites.push_back(aFeature);
      updateResults(aFeature);
    }
  }
  std::list<FeaturePtr>::iterator aComposite = aComposites.begin();
  for(; aComposite != aComposites.end(); aComposite++) {
    updateResults(*aComposite);
  }

  // the synchronize should be done after updateResults in order to correct back references of updated results
  if (theUpdateReferences) {
    synchronizeBackRefs();
  }
  if (!theUpdated.IsEmpty()) { // this means there is no control what was modified => remove history cash
    myHistory.clear();
  }

  anOwner->executeFeatures() = false;
  aLoop->activateFlushes(isActive);

  if (theFlush) {
    aLoop->flush(aCreateEvent);
    aLoop->flush(aDeleteEvent);
    aLoop->flush(anUpdateEvent);
    aLoop->flush(aCreateEvent); // after update of features, there could be results created
    aLoop->flush(aDeleteEvent); // or deleted
    aLoop->flush(aRedispEvent);
    aLoop->flush(aToHideEvent);
  }
  anOwner->executeFeatures() = true;
}

void Model_Objects::synchronizeBackRefs()
{
  // keeps the concealed flags of result to catch the change and create created/deleted events
  std::list<std::pair<ResultPtr, bool> > aConcealed;
  // first cycle: erase all data about back-references
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFeatures(myFeatures);
  for(; aFeatures.More(); aFeatures.Next()) {
    FeaturePtr aFeature = aFeatures.Value();
    std::shared_ptr<Model_Data> aFData = 
      std::dynamic_pointer_cast<Model_Data>(aFeature->data());
    if (aFData) {
      aFData->eraseBackReferences();
    }
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
    for (; aRIter != aResults.cend(); aRIter++) {
      std::shared_ptr<Model_Data> aResData = 
        std::dynamic_pointer_cast<Model_Data>((*aRIter)->data());
      if (aResData.get()) {
        aConcealed.push_back(std::pair<ResultPtr, bool>(*aRIter, (*aRIter)->isConcealed()));
        aResData->eraseBackReferences();
      }
      // iterate sub-bodies of compsolid
      ResultCompSolidPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aRIter);
      if (aComp.get()) {
        int aNumSub = aComp->numberOfSubs();
        for(int a = 0; a < aNumSub; a++) {
          ResultPtr aSub = aComp->subResult(a);
          std::shared_ptr<Model_Data> aResData = 
            std::dynamic_pointer_cast<Model_Data>(aSub->data());
          if (aResData.get()) {
            aConcealed.push_back(std::pair<ResultPtr, bool>(aSub, aSub->isConcealed()));
            aResData->eraseBackReferences();
          }
        }
      }
    }
  }

  // second cycle: set new back-references: only features may have reference, iterate only them
  ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();
  for(aFeatures.Initialize(myFeatures); aFeatures.More(); aFeatures.Next()) {
    FeaturePtr aFeature = aFeatures.Value();
    std::shared_ptr<Model_Data> aFData = 
      std::dynamic_pointer_cast<Model_Data>(aFeature->data());
    if (aFData) {
      std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
      aFData->referencesToObjects(aRefs);
      std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator 
        aRefsIter = aRefs.begin();
      for(; aRefsIter != aRefs.end(); aRefsIter++) {
        std::list<ObjectPtr>::iterator aRefTo = aRefsIter->second.begin();
        for(; aRefTo != aRefsIter->second.end(); aRefTo++) {
          if (*aRefTo) {
            std::shared_ptr<Model_Data> aRefData = 
              std::dynamic_pointer_cast<Model_Data>((*aRefTo)->data());
            aRefData->addBackReference(aFeature, aRefsIter->first); // here the Concealed flag is updated
            // update enable/disable status: the nested status must be equal to the composite
            CompositeFeaturePtr aComp = 
              std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
            if (aComp.get()) {
              FeaturePtr aReferenced = std::dynamic_pointer_cast<ModelAPI_Feature>(*aRefTo);
              if (aReferenced.get()) {
                aReferenced->setDisabled(aComp->isDisabled());
              }
            }
          }
        }
      }
    }
  }
  std::list<std::pair<ResultPtr, bool> >::iterator aCIter = aConcealed.begin();
  for(; aCIter != aConcealed.end(); aCIter++) {
    if (aCIter->first->isConcealed() != aCIter->second) { // somethign is changed => produce event
      if (aCIter->second) { // was concealed become not => creation event
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
        ModelAPI_EventCreator::get()->sendUpdated(aCIter->first, anEvent);
      } else { // was not concealed become concealed => delete event
        ModelAPI_EventCreator::get()->sendDeleted(myDoc, aCIter->first->groupName());
        // redisplay for the viewer (it must be disappeared also)
        static Events_ID EVENT_DISP = 
          Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
        ModelAPI_EventCreator::get()->sendUpdated(aCIter->first, EVENT_DISP);
      }
    }
  }
}

TDF_Label Model_Objects::resultLabel(
  const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theResultIndex) 
{
  const std::shared_ptr<Model_Data>& aData = 
    std::dynamic_pointer_cast<Model_Data>(theFeatureData);
  return aData->label().Father().FindChild(TAG_FEATURE_RESULTS).FindChild(theResultIndex + 1);
}

void Model_Objects::storeResult(std::shared_ptr<ModelAPI_Data> theFeatureData,
                                 std::shared_ptr<ModelAPI_Result> theResult,
                                 const int theResultIndex)
{
  theResult->setDoc(myDoc);
  initData(theResult, resultLabel(theFeatureData, theResultIndex), TAG_FEATURE_ARGUMENTS);
  if (theResult->data()->name().empty()) {  // if was not initialized, generate event and set a name
    std::stringstream aNewName;
    aNewName<<theFeatureData->name();
    // if there are several results (issue #899: any number of result), add unique prefix starting from second
    if (theResultIndex > 0 || theResult->groupName() == ModelAPI_ResultBody::group())
      aNewName<<"_"<<theResultIndex + 1;
    theResult->data()->setName(aNewName.str());
  }
}

std::shared_ptr<ModelAPI_ResultConstruction> Model_Objects::createConstruction(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  TDF_Label aLab = resultLabel(theFeatureData, theIndex);
  TDataStd_Comment::Set(aLab, ModelAPI_ResultConstruction::group().c_str());
  ObjectPtr anOldObject = object(aLab);
  std::shared_ptr<ModelAPI_ResultConstruction> aResult;
  if (anOldObject) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(anOldObject);
  }
  if (!aResult) {
    aResult = std::shared_ptr<ModelAPI_ResultConstruction>(new Model_ResultConstruction);
    storeResult(theFeatureData, aResult, theIndex);
  }
  return aResult;
}

std::shared_ptr<ModelAPI_ResultBody> Model_Objects::createBody(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  TDF_Label aLab = resultLabel(theFeatureData, theIndex);
  // for feature create compsolid, but for result sub create body: 
  // only one level of recursion is supported now
  ResultPtr aResultOwner = std::dynamic_pointer_cast<ModelAPI_Result>(theFeatureData->owner());
  ObjectPtr anOldObject;
  if (aResultOwner.get()) {
    TDataStd_Comment::Set(aLab, ModelAPI_ResultBody::group().c_str());
  } else { // in compsolid (higher level result) old object probably may be found
    TDataStd_Comment::Set(aLab, ModelAPI_ResultCompSolid::group().c_str());
    anOldObject = object(aLab);
  }
  std::shared_ptr<ModelAPI_ResultBody> aResult;
  if (anOldObject) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(anOldObject);
  }
  if (!aResult) {
    // create compsolid anyway; if it is compsolid, it will create sub-bodies internally
    if (aResultOwner.get()) {
      aResult = std::shared_ptr<ModelAPI_ResultBody>(new Model_ResultBody);
    } else {
      aResult = std::shared_ptr<ModelAPI_ResultBody>(new Model_ResultCompSolid);
    }
    storeResult(theFeatureData, aResult, theIndex);
  }
  return aResult;
}

std::shared_ptr<ModelAPI_ResultPart> Model_Objects::createPart(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  TDF_Label aLab = resultLabel(theFeatureData, theIndex);
  TDataStd_Comment::Set(aLab, ModelAPI_ResultPart::group().c_str());
  ObjectPtr anOldObject = object(aLab);
  std::shared_ptr<ModelAPI_ResultPart> aResult;
  if (anOldObject) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(anOldObject);
  }
  if (!aResult) {
    aResult = std::shared_ptr<ModelAPI_ResultPart>(new Model_ResultPart);
    storeResult(theFeatureData, aResult, theIndex);
  }
  return aResult;
}

std::shared_ptr<ModelAPI_ResultPart> Model_Objects::copyPart(
    const std::shared_ptr<ModelAPI_ResultPart>& theOrigin,
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  std::shared_ptr<ModelAPI_ResultPart> aResult = createPart(theFeatureData, theIndex);
  aResult->data()->reference(Model_ResultPart::BASE_REF_ID())->setValue(theOrigin);
  return aResult;
}

std::shared_ptr<ModelAPI_ResultGroup> Model_Objects::createGroup(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  TDF_Label aLab = resultLabel(theFeatureData, theIndex);
  TDataStd_Comment::Set(aLab, ModelAPI_ResultGroup::group().c_str());
  ObjectPtr anOldObject = object(aLab);
  std::shared_ptr<ModelAPI_ResultGroup> aResult;
  if (anOldObject) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultGroup>(anOldObject);
  }
  if (!aResult) {
    aResult = std::shared_ptr<ModelAPI_ResultGroup>(new Model_ResultGroup(theFeatureData));
    storeResult(theFeatureData, aResult, theIndex);
  }
  return aResult;
}

std::shared_ptr<ModelAPI_ResultParameter> Model_Objects::createParameter(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  TDF_Label aLab = resultLabel(theFeatureData, theIndex);
  TDataStd_Comment::Set(aLab, ModelAPI_ResultParameter::group().c_str());
  ObjectPtr anOldObject = object(aLab);
  std::shared_ptr<ModelAPI_ResultParameter> aResult;
  if (anOldObject) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(anOldObject);
  }
  if (!aResult) {
    aResult = std::shared_ptr<ModelAPI_ResultParameter>(new Model_ResultParameter);
    storeResult(theFeatureData, aResult, theIndex);
  }
  return aResult;
}

std::shared_ptr<ModelAPI_Feature> Model_Objects::feature(
    const std::shared_ptr<ModelAPI_Result>& theResult)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theResult->data());
  if (aData) {
    TDF_Label aFeatureLab = aData->label().Father().Father().Father();
    FeaturePtr aFeature = feature(aFeatureLab);
    if (!aFeature.get() && aFeatureLab.Depth() > 1) { // this may be sub-result of result
      aFeatureLab = aFeatureLab.Father().Father();
      aFeature = feature(aFeatureLab);
    }
    return aFeature;
  }
  return FeaturePtr();
}

std::string Model_Objects::featureResultGroup(FeaturePtr theFeature)
{
  if (theFeature->data()->isValid()) {
    TDF_ChildIterator aLabIter(resultLabel(theFeature->data(), 0).Father());
    if (aLabIter.More()) {
      TDF_Label anArgLab = aLabIter.Value();
      Handle(TDataStd_Comment) aGroup;
      if (aLabIter.Value().FindAttribute(TDataStd_Comment::GetID(), aGroup)) {
        return TCollection_AsciiString(aGroup->Get()).ToCString();
      }
    }
  }
  static std::string anEmpty;
  return anEmpty; // not found
}

void Model_Objects::updateResults(FeaturePtr theFeature)
{
  // for not persistent is will be done by parametric updater automatically
  //if (!theFeature->isPersistentResult()) return;
  // check the existing results and remove them if there is nothing on the label
  std::list<ResultPtr>::const_iterator aResIter = theFeature->results().cbegin();
  while(aResIter != theFeature->results().cend()) {
    ResultPtr aBody = std::dynamic_pointer_cast<ModelAPI_Result>(*aResIter);
    if (aBody.get()) {
      std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(aBody->data());
      if (!aData.get() || !aData->isValid() || (!aBody->isDisabled() && aData->isDeleted())) { 
        // found a disappeared result => remove it
        theFeature->eraseResultFromList(aBody);
        // start iterate from beginning because iterator is corrupted by removing
        aResIter = theFeature->results().cbegin();
        continue;
      }
    }
    aResIter++;
  }
  // it may be on undo
  if (!theFeature->data() || !theFeature->data()->isValid() || theFeature->isDisabled())
    return;
  // check that results are presented on all labels
  int aResSize = theFeature->results().size();
  TDF_ChildIterator aLabIter(resultLabel(theFeature->data(), 0).Father());
  for(; aLabIter.More(); aLabIter.Next()) {
    // here must be GUID of the feature
    int aResIndex = aLabIter.Value().Tag() - 1;
    ResultPtr aNewBody;
    if (aResSize <= aResIndex) {
      TDF_Label anArgLab = aLabIter.Value();
      Handle(TDataStd_Comment) aGroup;
      if (anArgLab.FindAttribute(TDataStd_Comment::GetID(), aGroup)) {
        if (aGroup->Get() == ModelAPI_ResultBody::group().c_str() || 
            aGroup->Get() == ModelAPI_ResultCompSolid::group().c_str()) {
          aNewBody = createBody(theFeature->data(), aResIndex);
        } else if (aGroup->Get() == ModelAPI_ResultPart::group().c_str()) {
          std::shared_ptr<ModelAPI_ResultPart> aNewP = createPart(theFeature->data(), aResIndex); 
          theFeature->setResult(aNewP, aResIndex);
          if (!aNewP->partDoc().get())
            theFeature->execute(); // create the part result: it is better to restore the previous result if it is possible
          break;
        } else if (aGroup->Get() == ModelAPI_ResultConstruction::group().c_str()) {
          theFeature->execute(); // construction shapes are needed for sketch solver
          break;
        } else if (aGroup->Get() == ModelAPI_ResultGroup::group().c_str()) {
          aNewBody = createGroup(theFeature->data(), aResIndex);
        } else if (aGroup->Get() == ModelAPI_ResultParameter::group().c_str()) {
          theFeature->attributeChanged("expression"); // just produce a value
          break;
        } else {
          Events_Error::send(std::string("Unknown type of result is found in the document:") +
            TCollection_AsciiString(aGroup->Get()).ToCString());
        }
      }
      if (aNewBody && !aNewBody->data()->isDeleted()) {
        theFeature->setResult(aNewBody, aResIndex);
      }
    }
  }
}

ResultPtr Model_Objects::findByName(const std::string theName)
{
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator anObjIter(myFeatures);
  for(; anObjIter.More(); anObjIter.Next()) {
    FeaturePtr& aFeature = anObjIter.ChangeValue();
    if (!aFeature.get() || aFeature->isDisabled()) // may be on close
      continue;
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
    for (; aRIter != aResults.cend(); aRIter++) {
      ResultPtr aRes = *aRIter;
      if (aRes.get() && aRes->data() && aRes->data()->isValid() && !aRes->isDisabled() &&
          aRes->data()->name() == theName) {
        return aRes;
      }
    }
  }
  // not found
  return ResultPtr();
}

FeaturePtr Model_Objects::nextFeature(FeaturePtr theCurrent, const bool theReverse)
{
  std::shared_ptr<Model_Data> aData = std::static_pointer_cast<Model_Data>(theCurrent->data());
  if (aData && aData->isValid()) {
    TDF_Label aFeatureLabel = aData->label().Father();
    Handle(TDataStd_ReferenceArray) aRefs;
    if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
      for(int a = aRefs->Lower(); a <= aRefs->Upper(); a++) { // iterate all existing features
        TDF_Label aCurLab = aRefs->Value(a);
        if (aCurLab.IsEqual(aFeatureLabel)) {
          a += theReverse ? -1 : 1;
          if (a >= aRefs->Lower() && a <= aRefs->Upper())
            return feature(aRefs->Value(a));
          break; // finish iiteration: it's last feature
        }
      }
    }
  }
  return FeaturePtr(); // not found, last, or something is wrong
}

FeaturePtr Model_Objects::firstFeature()
{
  Handle(TDataStd_ReferenceArray) aRefs;
  if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    return feature(aRefs->Value(aRefs->Lower()));
  }
  return FeaturePtr(); // no features at all
}

FeaturePtr Model_Objects::lastFeature()
{
  Handle(TDataStd_ReferenceArray) aRefs;
  if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    return feature(aRefs->Value(aRefs->Upper()));
  }
  return FeaturePtr(); // no features at all
}

bool Model_Objects::isLater(FeaturePtr theLater, FeaturePtr theCurrent) const
{
  std::shared_ptr<Model_Data> aLaterD = std::static_pointer_cast<Model_Data>(theLater->data());
  std::shared_ptr<Model_Data> aCurrentD = std::static_pointer_cast<Model_Data>(theCurrent->data());
  if (aLaterD && aLaterD->isValid() && aCurrentD && aCurrentD->isValid()) {
    TDF_Label aLaterL = aLaterD->label().Father();
    TDF_Label aCurrentL = aCurrentD->label().Father();
    int aLaterI = -1, aCurentI = -1; // not found yet state
    Handle(TDataStd_ReferenceArray) aRefs;
    if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
      for(int a = aRefs->Lower(); a <= aRefs->Upper(); a++) { // iterate all existing features
        TDF_Label aCurLab = aRefs->Value(a);
        if (aCurLab.IsEqual(aLaterL)) {
          aLaterI = a;
        } else if (aCurLab.IsEqual(aCurrentL)) {
          aCurentI = a;
        } else continue;
        if (aLaterI != -1 && aCurentI != -1) // both are found
          return aLaterI > aCurentI;
      }
    }
  }
  return false; // not found, or something is wrong
}

std::list<std::shared_ptr<ModelAPI_Feature> > Model_Objects::allFeatures()
{
  std::list<std::shared_ptr<ModelAPI_Feature> > aResult;
  Handle(TDataStd_ReferenceArray) aRefs;
  if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    for(int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
      FeaturePtr aFeature = feature(aRefs->Value(a));
      if (aFeature.get())
        aResult.push_back(aFeature);
    }
  }
  return aResult;
}

int Model_Objects::numInternalFeatures()
{
  Handle(TDataStd_ReferenceArray) aRefs;
  if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    return aRefs->Upper() - aRefs->Lower() + 1;
  }
  return 0; // invalid
}

std::shared_ptr<ModelAPI_Feature> Model_Objects::internalFeature(const int theIndex)
{
  Handle(TDataStd_ReferenceArray) aRefs;
  if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    return feature(aRefs->Value(aRefs->Lower() + theIndex));
  }
  return FeaturePtr(); // invalid
}

Standard_Integer HashCode(const TDF_Label& theLab, const Standard_Integer theUpper)
{
  return TDF_LabelMapHasher::HashCode(theLab, theUpper);

}
Standard_Boolean IsEqual(const TDF_Label& theLab1, const TDF_Label& theLab2)
{
  return TDF_LabelMapHasher::IsEqual(theLab1, theLab2);
}
