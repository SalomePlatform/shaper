// Copyright (C) 2014-2024  CEA, EDF
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

#include <Model_Objects.h>
#include <Model_Data.h>
#include <Model_Document.h>
#include <Model_Events.h>
#include <Model_Session.h>
#include <Model_ResultPart.h>
#include <Model_ResultConstruction.h>
#include <Model_ResultBody.h>
#include <Model_ResultGroup.h>
#include <Model_ResultField.h>
#include <Model_ResultParameter.h>
#include <Model_AttributeRefList.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Filter.h>


#include <Events_Loop.h>
#include <Events_InfoMessage.h>

#include <Locale_Convert.h>

#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_HLabelArray1.hxx>
#include <TDF_Reference.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDF_LabelMapHasher.hxx>
#include <TDF_LabelMap.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>

// for TDF_Label map usage
static Standard_Integer HashCode(const TDF_Label& theLab, const Standard_Integer theUpper);
static Standard_Boolean IsEqual(const TDF_Label& theLab1, const TDF_Label& theLab2);

int kUNDEFINED_FEATURE_INDEX = -1;

static const std::string& groupNameFoldering(const std::string& theGroupID,
                                             const bool theAllowFolder)
{
  if (theAllowFolder) {
    static const std::string anOutOfFolderName = std::string("__") + ModelAPI_Feature::group();
    static const std::string aDummyName;
    return theGroupID == ModelAPI_Feature::group() ? anOutOfFolderName : aDummyName;
  }
  return theGroupID;
}

// Check theFeature is a first or last feature in folder and return this folder
static FolderPtr inFolder(const FeaturePtr& theFeature, const std::string& theFolderAttr)
{
  const std::set<AttributePtr>& aRefs = theFeature->data()->refsToMe();
  for (std::set<AttributePtr>::iterator anIt = aRefs.begin(); anIt != aRefs.end(); ++anIt) {
    if ((*anIt)->id() != theFolderAttr)
      continue;

    ObjectPtr anOwner = (*anIt)->owner();
    FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(anOwner);
    if (aFolder.get())
      return aFolder;
  }
  return FolderPtr();
}


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
  synchronizeFeatures(aNoUpdated, true, false, true, true);
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
    aFeature->erase();
    myFeatures.UnBind(aFeaturesIter.Key());
  }
  while (!myFolders.IsEmpty()) {
    NCollection_DataMap<TDF_Label, ObjectPtr>::Iterator aFoldersIter(myFolders);
    ObjectPtr aFolder = aFoldersIter.Value();
    static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    ModelAPI_EventCreator::get()->sendDeleted(myDoc, ModelAPI_Folder::group());
    ModelAPI_EventCreator::get()->sendUpdated(aFolder, EVENT_DISP);
    aFolder->erase();
    myFolders.UnBind(aFoldersIter.Key());
  }
  myHistory.clear();
  aLoop->activateFlushes(isActive);
  // erase update, because features are destroyed and update should not performed for them anywhere
  aLoop->eraseMessages(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->eraseMessages(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  // deleted and redisplayed is correctly performed: they know that features are destroyed
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
    int aPassedPrev = 0; // previous feature is found and passed
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
    FolderPtr aParentFolder;
    if (theAfterThis.get()) { // searching for the previous feature label
      std::shared_ptr<Model_Data> aPrevData =
        std::dynamic_pointer_cast<Model_Data>(theAfterThis->data());
      if (aPrevData.get()) {
        aPrevFeateureLab = aPrevData->label().Father();
      }
      // Check if the previous feature is the last feature in a folder,
      // then the folder should be updated to contain additional feature.
      // Macro features are not stored in folder.
      if (!theFeature->isMacro()) {
        // If the last feature is a sub-feature of composite, use parent feature
        // to check belonging to a folder.
        FeaturePtr afterThis = ModelAPI_Tools::compositeOwner(theAfterThis);
        if (!afterThis)
          afterThis = theAfterThis;
        aParentFolder = inFolder(afterThis, ModelAPI_Folder::LAST_FEATURE_ID());
      }
    }
    AddToRefArray(aFeaturesLab, aFeatureLab, aPrevFeateureLab);

    // keep the feature ID to restore document later correctly
    TDataStd_Comment::Set(aFeatureLab, theFeature->getKind().c_str());
    myFeatures.Bind(aFeatureLab, theFeature);
    // must be before the event sending: for OB the feature is already added
    updateHistory(ModelAPI_Feature::group());
    // do not change the order:
    // initData()
    // sendUpdated()
    // during python script with fillet constraint feature data should be
    // initialized before using it in GUI

    // must be after binding to the map because of "Box" macro feature that
    // creates other features in "initData"
    initData(theFeature, aFeatureLab, TAG_FEATURE_ARGUMENTS);
    // put feature to the end of folder if it is added while
    // the history line is set to the last feature from the folder
    if (aParentFolder) {
      aParentFolder->reference(ModelAPI_Folder::LAST_FEATURE_ID())->setValue(theFeature);
      updateHistory(ModelAPI_Folder::group());
    }
    // event: feature is added, mist be before "initData" to update OB correctly on Duplicate:
    // first new part, then the content
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
    ModelAPI_EventCreator::get()->sendUpdated(theFeature, anEvent);
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
  std::list<ResultPtr> aResults;
  ModelAPI_Tools::allResults(theFeature, aResults);
  std::list<ResultPtr>::const_iterator aResIter = aResults.cbegin();
  for (; aResIter != aResults.cend(); aResIter++) {
    ResultPtr aResult = (*aResIter);
    std::shared_ptr<Model_Data> aData =
        std::dynamic_pointer_cast<Model_Data>(aResult->data());
    if (aData.get() != NULL) {
      const std::set<AttributePtr>& aRefs = aData->refsToMe();
      std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin(), aRefLast = aRefs.end();
      for (; aRefIt != aRefLast; aRefIt++) {
        FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIt)->owner());
        if (aFeature.get() != NULL)
          theRefs.insert(aFeature);
      }
    }
  }
  // the dependencies can be in the feature itself
  std::shared_ptr<Model_Data> aData =
      std::dynamic_pointer_cast<Model_Data>(theFeature->data());
  if (aData.get() && !aData->refsToMe().empty()) {
    const std::set<AttributePtr>& aRefs = aData->refsToMe();
    std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin(), aRefLast = aRefs.end();
    for (; aRefIt != aRefLast; aRefIt++) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRefIt)->owner());
      if (aFeature.get() != NULL)
        theRefs.insert(aFeature);
    }
  }

  if (!theRefs.empty() && isSendError) {
    Events_InfoMessage("Model_Objects",
      "Feature '%1' is used and can not be deleted").arg(theFeature->data()->name()).send();
  }
}

void Model_Objects::removeFeature(FeaturePtr theFeature)
{
  std::shared_ptr<Model_Data> aData = std::static_pointer_cast<Model_Data>(theFeature->data());
  if (aData.get() && aData->isValid()) {
    // checking that the sub-element of composite feature is removed: if yes, inform the owner
    std::set<std::shared_ptr<ModelAPI_Feature> > aRefs;
    refsToFeature(theFeature, aRefs, false);
    std::set<std::shared_ptr<ModelAPI_Feature> >::iterator aRefIter = aRefs.begin();
    for(; aRefIter != aRefs.end(); aRefIter++) {
      std::shared_ptr<ModelAPI_CompositeFeature> aComposite =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(*aRefIter);
      if (aComposite.get() && aComposite->data()->isValid() && aComposite->isSub(theFeature)) {
        aComposite->removeFeature(theFeature);
      }
    }
    // remove feature from folder
    removeFromFolder(std::list<FeaturePtr>(1, theFeature));
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
    updateHistory(ModelAPI_Feature::group());
  }
}

void Model_Objects::eraseAllFeatures()
{
  static Events_ID kDispEvent = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static const ModelAPI_EventCreator* kCreator = ModelAPI_EventCreator::get();
  // make all features invalid (like deleted)
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFIter(myFeatures);
  for(; aFIter.More(); aFIter.Next()) {
    FeaturePtr aFeature = aFIter.Value();
    std::list<ResultPtr> aResList;
    ModelAPI_Tools::allResults(aFeature, aResList);
    std::list<ResultPtr>::iterator aRIter = aResList.begin();
    for(; aRIter != aResList.end(); aRIter++) {
      ResultPtr aRes = *aRIter;
      if (aRes && aRes->data()->isValid()) {
        kCreator->sendDeleted(myDoc, aRes->groupName());
        kCreator->sendUpdated(aRes, kDispEvent);
        aRes->setData(aRes->data()->invalidPtr());

      }
    }
    kCreator->sendUpdated(aFeature, kDispEvent);
    aFeature->setData(aFeature->data()->invalidPtr());
  }
  kCreator->sendDeleted(myDoc, ModelAPI_Feature::group());
  myFeatures.Clear(); // just remove features without modification of DS
  myHistory.clear();
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

  // check whether some folder refers to the moved feature by start or end: if yes, remove from it
  removeFromFolder(std::list<FeaturePtr>(1, theMoved));

  Handle(TDataStd_HLabelArray1) aNewArray =
    new TDataStd_HLabelArray1(aRefs->Lower(), aRefs->Upper());
  int aPassedMovedFrom = 0; // the previous feature location is found and passed
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
      Events_InfoMessage("Model_Objects", aMovedFromError).send();
    } else {
      static std::string aMovedToError("The 'after' feature for movement is not found");
      Events_InfoMessage("Model_Objects", aMovedToError).send();
    }
    return;
  }
  // store the new array
  aRefs->SetInternalArray(aNewArray);
  // update the feature and the history
  clearHistory(theMoved);
  // make sure all (selection) attributes of moved feature will be updated
  static Events_ID kUpdateSelection = Events_Loop::loop()->eventByName(EVENT_UPDATE_SELECTION);
  ModelAPI_EventCreator::get()->sendUpdated(theMoved, kUpdateSelection, false);
  ModelAPI_EventCreator::get()->sendReordered(theMoved);
}

void Model_Objects::clearHistory(ObjectPtr theObj)
{
  if (theObj.get()) {
    const std::string aGroup = theObj->groupName();
    updateHistory(aGroup);

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
    std::vector<ObjectPtr> aResult;
    std::vector<ObjectPtr> aResultOutOfFolder;
    FeaturePtr aLastFeatureInFolder;
    // iterate the array of references and get feature by feature from the array
    bool isFeature = theGroupID == ModelAPI_Feature::group();
    bool isFolder = theGroupID == ModelAPI_Folder::group();
    Handle(TDataStd_ReferenceArray) aRefs;
    if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
      for(int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
        FeaturePtr aFeature = feature(aRefs->Value(a));
        if (aFeature.get()) {
          // if feature is in sub-component, remove it from history:
          // it is in sub-tree of sub-component
          bool isSub = ModelAPI_Tools::compositeOwner(aFeature).get() != NULL;
          if (isFeature) { // here may be also disabled features
            if (!isSub && aFeature->isInHistory()) {
              aResult.push_back(aFeature);
              // the feature is out of the folders
              if (aLastFeatureInFolder.get() == NULL)
                aResultOutOfFolder.push_back(aFeature);
            }
          } else if (!aFeature->isDisabled()) { // iterate all results of not-disabled feature
            // construction results of sub-features should not be in the tree
            if (!isSub || theGroupID != ModelAPI_ResultConstruction::group()) {
              // do not use reference to the list here since results can be changed by "isConcealed"
              const std::list<std::shared_ptr<ModelAPI_Result> > aResults = aFeature->results();
              std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator
                aRIter = aResults.begin();
              for (; aRIter != aResults.cend(); aRIter++) {
                ResultPtr aRes = *aRIter;
                if (aRes->groupName() != theGroupID) break; // feature have only same group results
                if (!aRes->isDisabled() && aRes->isInHistory() && !aRes->isConcealed()) {
                  aResult.push_back(*aRIter);
                }
              }
            }
          }

          // the feature closes the folder, so the next features will be treated as out-of-folder
          if (aLastFeatureInFolder.get() && aLastFeatureInFolder == aFeature)
            aLastFeatureInFolder = FeaturePtr();

        } else {
          // it may be a folder
          const ObjectPtr& aFolder = folder(aRefs->Value(a));
          if (aFolder.get()) {
            // store folder information for the Features group only
            if (isFeature || isFolder) {
              aResult.push_back(aFolder);
              if (!isFolder)
                aResultOutOfFolder.push_back(aFolder);
            }

            // get the last feature in the folder
            AttributeReferencePtr aLastFeatAttr =
                aFolder->data()->reference(ModelAPI_Folder::LAST_FEATURE_ID());
            if (aLastFeatAttr)
              aLastFeatureInFolder = ModelAPI_Feature::feature(aLastFeatAttr->value());
          }
        }
      }
    }
    // to be sure that isConcealed did not update the history (issue 1089) during the iteration
    if (myHistory.find(theGroupID) == myHistory.end()) {
      myHistory[theGroupID] = aResult;

      // store the features placed out of any folder
      const std::string& anOutOfFolderGroupID = groupNameFoldering(theGroupID, true);
      if (!anOutOfFolderGroupID.empty())
        myHistory[anOutOfFolderGroupID] = aResultOutOfFolder;
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
  if (aHIter != myHistory.end()) {
    myHistory.erase(aHIter); // erase from map => this means that it is not synchronized

    // erase history for the group of objects placed out of any folder
    const std::string& anOutOfFolderGroupID = groupNameFoldering(theGroup, true);
    if (!anOutOfFolderGroupID.empty())
      myHistory.erase(anOutOfFolderGroupID);
  }
}

const ObjectPtr& Model_Objects::folder(TDF_Label theLabel) const
{
  if (myFolders.IsBound(theLabel))
    return myFolders.Find(theLabel);
  static ObjectPtr anEmptyResult;
  return anEmptyResult;
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
  if (aFeature.get())
    return feature(theLabel);
  TDF_Label aFeatureLabel = theLabel;  // let's suppose it is result of this feature
  TDF_LabelList aSubLabs; // sub - labels from higher level to lower level of result
  while(!aFeature.get() && aFeatureLabel.Depth() > 1) {
    aSubLabs.Prepend(aFeatureLabel);
    aFeatureLabel = aFeatureLabel.Father().Father();
    aFeature = feature(aFeatureLabel);
  }
  if (aFeature.get()) {
    ResultPtr aCurrentResult;
    // searching for results then sub-results label by label
    for(TDF_ListIteratorOfLabelList aSubLab(aSubLabs); aSubLab.More(); aSubLab.Next()) {
      if (aCurrentResult.get()) { // iterate sub-results of result
        ResultBodyPtr anOwner = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aCurrentResult);
        if (!anOwner)
          return ObjectPtr(); // only Body can have sub-results
        int a, aNumSubs = anOwner->numberOfSubs();
        for(a = 0; a < aNumSubs; a++) {
          ResultPtr aSub = anOwner->subResult(a);
          if (aSub.get()) {
            std::shared_ptr<Model_Data> aSubData = std::dynamic_pointer_cast<Model_Data>(
              aSub->data());
            const TDF_Label& aSubLabVal = aSubLab.ChangeValue();
            if (aSubData->label().Father().IsEqual(aSubLabVal)) {
              aCurrentResult = aSub;
              break;
            }
          }
        }
        if (a == aNumSubs) // not found an appropriate sub-result of result
          return ObjectPtr();
      } else { // iterate results of feature
        const std::list<ResultPtr>& aResults = aFeature->results();
        std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.cbegin();
        for(; aRIter != aResults.cend(); aRIter++) {
          std::shared_ptr<Model_Data> aResData =
            std::dynamic_pointer_cast<Model_Data>((*aRIter)->data());
          if (aResData->label().Father().IsEqual(aSubLab.ChangeValue())) {
            aCurrentResult = *aRIter;
            break;
          }
        }
        if (aRIter == aResults.cend()) // not found an appropriate result of feature
          return ObjectPtr();
      }
    }
    return aCurrentResult;
  }
  return ObjectPtr();  // not found
}

ObjectPtr Model_Objects::object(const std::string& theGroupID,
                                const int theIndex,
                                const bool theAllowFolder)
{
  if (theIndex == -1)
    return ObjectPtr();
  createHistory(theGroupID);
  const std::string& aGroupID = groupNameFoldering(theGroupID, theAllowFolder);
  return aGroupID.empty() ? myHistory[theGroupID][theIndex] : myHistory[aGroupID][theIndex];
}

std::shared_ptr<ModelAPI_Object> Model_Objects::objectByName(
    const std::string& theGroupID, const std::wstring& theName)
{
  createHistory(theGroupID);
  if (theGroupID == ModelAPI_Feature::group()) { // searching among features (in history or not)
    std::list<std::shared_ptr<ModelAPI_Feature> > allObjs = allFeatures();
    // from the end to find the latest result with such name
    std::list<std::shared_ptr<ModelAPI_Feature> >::reverse_iterator anObjIter = allObjs.rbegin();
    for(; anObjIter != allObjs.rend(); anObjIter++) {
      if ((*anObjIter)->data()->name() == theName)
        return *anObjIter;
    }
  } else { // searching among results (concealed or not)
    std::list<std::shared_ptr<ModelAPI_Feature> > allObjs = allFeatures();
    // from the end to find the latest result with such name
    std::list<std::shared_ptr<ModelAPI_Feature> >::reverse_iterator anObjIter = allObjs.rbegin();
    for(; anObjIter != allObjs.rend(); anObjIter++) {
      std::list<ResultPtr> allRes;
      ModelAPI_Tools::allResults(*anObjIter, allRes);
      for(std::list<ResultPtr>::iterator aRes = allRes.begin(); aRes != allRes.end(); aRes++) {
        if (aRes->get() && (*aRes)->groupName() == theGroupID) {
          if ((*aRes)->data()->name() == theName)
            return *aRes;
        }
      }
    }
  }
  // not found
  return ObjectPtr();
}

const int Model_Objects::index(std::shared_ptr<ModelAPI_Object> theObject,
                               const bool theAllowFolder)
{
  std::string aGroup = theObject->groupName();
  // treat folder as feature
  if (aGroup == ModelAPI_Folder::group())
    aGroup = ModelAPI_Feature::group();
  createHistory(aGroup);

  // get the group of features out of folder (if enabled)
  if (theAllowFolder && !groupNameFoldering(aGroup, theAllowFolder).empty())
    aGroup = groupNameFoldering(aGroup, theAllowFolder);

  std::vector<ObjectPtr>& allObjs = myHistory[aGroup];
  std::vector<ObjectPtr>::iterator anObjIter = allObjs.begin(); // iterate to search object
  for(int anIndex = 0; anObjIter != allObjs.end(); anObjIter++, anIndex++) {
    if ((*anObjIter) == theObject)
      return anIndex;
  }
  // not found
  return -1;
}

int Model_Objects::size(const std::string& theGroupID, const bool theAllowFolder)
{
  createHistory(theGroupID);
  const std::string& aGroupID = groupNameFoldering(theGroupID, theAllowFolder);
  return aGroupID.empty() ? int(myHistory[theGroupID].size()) : int(myHistory[aGroupID].size());
}

std::shared_ptr<ModelAPI_Object> Model_Objects::parent(
  const std::shared_ptr<ModelAPI_Object> theChild)
{
  if (theChild.get()) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theChild->data());
    TDF_Label aLab = aData->label();
    if (!aLab.IsNull() && aLab.Depth() > 2) {
      ObjectPtr anObj = object(aLab.Father().Father().Father());
      return anObj;
    }
  }
  return ObjectPtr();
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

static std::wstring composeName(const std::string& theFeatureKind, const int theIndex)
{
  std::stringstream aNameStream;
  aNameStream << theFeatureKind << "_" << theIndex;
  return Locale::Convert::toWString(aNameStream.str());
}

void Model_Objects::setUniqueName(FeaturePtr theFeature)
{
  if (!theFeature->data()->name().empty())
    return;  // not needed, name is already defined
  std::wstring aName;  // result
  // first count all features of such kind to start with index = count + 1
  int aNumObjects = -1; // this feature is already in this map
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFIter(myFeatures);
  std::set<std::wstring> allNames;
  for (; aFIter.More(); aFIter.Next()) {
    if (aFIter.Value()->getKind() == theFeature->getKind())
      aNumObjects++;
    allNames.insert(aFIter.Value()->data()->name());
  }
  // generate candidate name
  aName = composeName(theFeature->getKind(), aNumObjects + 1);
  // check this is unique, if not, increase index by 1
  for(aNumObjects++; allNames.find(aName) != allNames.end(); aNumObjects++) {
    aName = composeName(theFeature->getKind(), aNumObjects + 1);
  }
  theFeature->data()->setName(aName);
}

void Model_Objects::setUniqueName(FolderPtr theFolder)
{
  if (!theFolder->name().empty())
    return; // name is already defined

  int aNbFolders = myFolders.Size();
  std::wstring aName = composeName(ModelAPI_Folder::ID(), aNbFolders);

  // check the uniqueness of the name
  NCollection_DataMap<TDF_Label, ObjectPtr>::Iterator anIt(myFolders);
  while (anIt.More()) {
    if (anIt.Value()->data()->name() == aName) {
      aName = composeName(ModelAPI_Folder::ID(), ++aNbFolders);
      // reinitialize iterator to make sure a new name is unique
      anIt.Initialize(myFolders);
    } else
      anIt.Next();
  }

  theFolder->data()->setName(aName);
}

void Model_Objects::initData(ObjectPtr theObj, TDF_Label theLab, const int theTag)
{
  std::shared_ptr<Model_Data> aData(new Model_Data);
  aData->setLabel(theLab.FindChild(theTag));
  aData->setObject(theObj);
  theObj->setDoc(myDoc);
  theObj->setData(aData);
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if (aFeature.get()) {
    setUniqueName(aFeature);  // must be before "initAttributes" because duplicate part uses name
  } else { // is it a folder?
    FolderPtr aFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(theObj);
    if (aFolder)
      setUniqueName(aFolder);
  }
  theObj->initAttributes();
}

void Model_Objects::synchronizeFeatures(
  const TDF_LabelList& theUpdated, const bool theUpdateReferences,
  const bool theExecuteFeatures, const bool theOpen, const bool theFlush)
{
  Model_Document* anOwner = std::dynamic_pointer_cast<Model_Document>(myDoc).get();
  if (!anOwner) // this may happen on creation of document: nothing there, so nothing to synchronize
    return;
  // after all updates, sends a message that groups of features were created or updated
  Events_Loop* aLoop = Events_Loop::loop();
  //static Events_ID aDispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static Events_ID aCreateEvent = aLoop->eventByName(EVENT_OBJECT_CREATED);
  static Events_ID anUpdateEvent = aLoop->eventByName(EVENT_OBJECT_UPDATED);
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static Events_ID aDeleteEvent = aLoop->eventByName(EVENT_OBJECT_DELETED);
  static Events_ID aToHideEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  bool isActive = aLoop->activateFlushes(false);

  // collect all updated labels map
  TDF_LabelMap anUpdatedMap;
  TDF_ListIteratorOfLabelList anUpdatedIter(theUpdated);
  for(; anUpdatedIter.More(); anUpdatedIter.Next()) {
    TDF_Label& aFeatureLab = anUpdatedIter.Value();
    while(aFeatureLab.Depth() > 3)
      aFeatureLab = aFeatureLab.Father();
    if (myFeatures.IsBound(aFeatureLab) || myFolders.IsBound(aFeatureLab))
      anUpdatedMap.Add(aFeatureLab);
  }

  // update all objects by checking are they on labels or not
  std::set<ObjectPtr> aNewFeatures, aKeptFeatures;
  TDF_ChildIDIterator aLabIter(featuresLabel(), TDataStd_Comment::GetID());
  for (; aLabIter.More(); aLabIter.Next()) {
    TDF_Label aFeatureLabel = aLabIter.Value()->Label();
    if (!myFeatures.IsBound(aFeatureLabel) && !myFolders.IsBound(aFeatureLabel)) {
      // a new feature or folder is inserted

      std::string aFeatureID = TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(
                               aLabIter.Value())->Get()).ToCString();
      bool isFolder = aFeatureID == ModelAPI_Folder::ID();

      std::shared_ptr<Model_Session> aSession =
          std::dynamic_pointer_cast<Model_Session>(ModelAPI_Session::get());

      // create a feature
      ObjectPtr aFeature = isFolder ? ObjectPtr(new ModelAPI_Folder)
                                    : ObjectPtr(aSession->createFeature(aFeatureID, anOwner));
      if (!aFeature.get()) {
        // something is wrong, most probably, the opened document has invalid structure
        Events_InfoMessage("Model_Objects", "Invalid type of object in the document").send();
        aLabIter.Value()->Label().ForgetAllAttributes();
        continue;
      }
      aFeature->init();
      // this must be before "setData" to redo the sketch line correctly
      if (isFolder)
        myFolders.Bind(aFeatureLabel, aFeature);
      else
        myFeatures.Bind(aFeatureLabel, std::dynamic_pointer_cast<ModelAPI_Feature>(aFeature));
      aNewFeatures.insert(aFeature);
      initData(aFeature, aFeatureLabel, TAG_FEATURE_ARGUMENTS);
      updateHistory(aFeature);

      // event: model is updated
      ModelAPI_EventCreator::get()->sendUpdated(aFeature, aCreateEvent);
    } else {  // nothing is changed, both iterators are incremented
      ObjectPtr anObject;
      FeaturePtr aFeature;
      if (myFeatures.Find(aFeatureLabel, aFeature)) {
        aKeptFeatures.insert(aFeature);
        anObject = aFeature;
      } else
        if (myFolders.Find(aFeatureLabel, anObject))
          aKeptFeatures.insert(anObject);

      if (anUpdatedMap.Contains(aFeatureLabel)) {
        if (!theOpen) { // on abort/undo/redo reinitialize attributes if something is changed
          FiltersFeaturePtr aFilter = std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(anObject);
          if (aFilter.get()) { // for filters attributes may be added/removed on undo/redo
            std::dynamic_pointer_cast<Model_Data>(aFilter->data())->clearAttributes();
            aFilter->initAttributes();
          } else {
            std::list<std::shared_ptr<ModelAPI_Attribute> > anAttrs =
              anObject->data()->attributes("");
            std::list<std::shared_ptr<ModelAPI_Attribute> >::iterator anAttr = anAttrs.begin();
            for(; anAttr != anAttrs.end(); anAttr++)
              (*anAttr)->reinit();
            // if feature contains results, re-init them too
            if (aFeature.get()) {
              std::list<ResultPtr> aResults;
              ModelAPI_Tools::allResults(aFeature, aResults);
              std::list<ResultPtr>::iterator aResIter = aResults.begin();
              for(; aResIter != aResults.end(); aResIter++) {
                anAttrs = (*aResIter)->data()->attributes("");
                for(anAttr = anAttrs.begin(); anAttr != anAttrs.end(); anAttr++)
                  (*anAttr)->reinit();
              }
            }
          }
        }
        ModelAPI_EventCreator::get()->sendUpdated(anObject, anUpdateEvent);
        if (aFeature && aFeature->getKind() == "Parameter") {
          // if parameters are changed, update the results (issue 937)
          const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
          std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
          for (; aRIter != aResults.cend(); aRIter++) {
            std::shared_ptr<ModelAPI_Result> aRes = *aRIter;
            if (aRes->data()->isValid() && !aRes->isDisabled()) {
              ModelAPI_EventCreator::get()->sendUpdated(aRes, anUpdateEvent);
            }
          }
        }
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

        // unbind after the "erase" call: on abort sketch
        // is removes sub-objects that corrupts aFIter
        myFeatures.UnBind(aFIter.Key());
        // reinitialize iterator because unbind may corrupt the previous order in the map
        aFIter.Initialize(myFeatures);
    } else
      aFIter.Next();
  }
  // verify folders are checked: if not => is was removed
  for (NCollection_DataMap<TDF_Label, ObjectPtr>::Iterator aFldIt(myFolders);
       aFldIt.More(); aFldIt.Next()) {
    ObjectPtr aCurObj = aFldIt.Value();
    if (aKeptFeatures.find(aCurObj) == aKeptFeatures.end() &&
        aNewFeatures.find(aCurObj) == aNewFeatures.end()) {
      ModelAPI_EventCreator::get()->sendDeleted(myDoc, ModelAPI_Folder::group());
      // results of this feature must be redisplayed (hided)
      // redisplay also removed feature (used for sketch and AISObject)
      ModelAPI_EventCreator::get()->sendUpdated(aCurObj, aRedispEvent);
      updateHistory(aCurObj);
      aCurObj->erase();

      // unbind after the "erase" call: on abort sketch
      // is removes sub-objects that corrupts aFIter
      myFolders.UnBind(aFldIt.Key());
      // reinitialize iterator because unbind may corrupt the previous order in the map
      aFldIt.Initialize(myFolders);
    }
  }

  if (theUpdateReferences) {
    synchronizeBackRefs();
  }
  // update results of the features (after features created because
  // they may be connected, like sketch and sub elements)
  // After synchronization of back references because sketch
  // must be set in sub-elements before "execute" by updateResults
  std::set<FeaturePtr> aProcessed; // composites must be updated after their subs (issue 360)
  TDF_ChildIDIterator aLabIter2(featuresLabel(), TDataStd_Comment::GetID());
  for (; aLabIter2.More(); aLabIter2.Next()) {
    TDF_Label aFeatureLabel = aLabIter2.Value()->Label();
    if (myFeatures.IsBound(aFeatureLabel)) {  // a new feature is inserted
      FeaturePtr aFeature = myFeatures.Find(aFeatureLabel);
      updateResults(aFeature, aProcessed);
    }
  }
  // the synchronize should be done after updateResults
  // in order to correct back references of updated results
  if (theUpdateReferences) {
    synchronizeBackRefs();
  }
  if (!theUpdated.IsEmpty()) {
    // this means there is no control what was modified => remove history cash
    myHistory.clear();
  }

  if (!theExecuteFeatures)
    anOwner->setExecuteFeatures(false);
  aLoop->activateFlushes(isActive);

  if (theFlush) {
    aLoop->flush(aDeleteEvent);
    // delete should be emitted before create to reacts to aborted feature
    aLoop->flush(aCreateEvent);
    aLoop->flush(anUpdateEvent);
    aLoop->flush(aCreateEvent); // after update of features, there could be results created
    aLoop->flush(aDeleteEvent); // or deleted
    aLoop->flush(aRedispEvent);
    aLoop->flush(aToHideEvent);
  }
  if (!theExecuteFeatures)
    anOwner->setExecuteFeatures(true);
}

/// synchronizes back references for the given object basing on the collected data
void Model_Objects::synchronizeBackRefsForObject(const std::set<AttributePtr>& theNewRefs,
  ObjectPtr theObject)
{
  if (!theObject.get() || !theObject->data()->isValid())
    return; // invalid
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
  // iterate new list to compare with current
  std::set<AttributePtr>::iterator aNewIter = theNewRefs.begin();
  for(; aNewIter != theNewRefs.end(); aNewIter++) {
    // for the Model_AttributeRefList erase cash (issue #2819)
    std::shared_ptr<Model_AttributeRefList> aRefList =
      std::dynamic_pointer_cast<Model_AttributeRefList>(*aNewIter);
    if (aRefList)
      aRefList->eraseHash();

    if (aData->refsToMe().find(*aNewIter) == aData->refsToMe().end()) {
      FeaturePtr aRefFeat = std::dynamic_pointer_cast<ModelAPI_Feature>((*aNewIter)->owner());
      if (aRefFeat)
        aData->addBackReference(aRefFeat, (*aNewIter)->id());
      else // add back reference to a folder
        aData->addBackReference((*aNewIter)->owner(), (*aNewIter)->id());
    }
  }
  if (theNewRefs.size() != aData->refsToMe().size()) { // some back ref must be removed
    std::set<AttributePtr>::iterator aCurrentIter = aData->refsToMe().begin();
    while(aCurrentIter != aData->refsToMe().end()) {
      if (theNewRefs.find(*aCurrentIter) == theNewRefs.end()) {
        // for external references from other documents this system
        // is not working: refs are collected from
        // different Model_Objects, so before remove check this
        // external object exists and still referenced
        bool aLeaveIt = false;
        if ((*aCurrentIter)->owner().get() && (*aCurrentIter)->owner()->document() != myDoc &&
            (*aCurrentIter)->owner()->data().get() && (*aCurrentIter)->owner()->data()->isValid()) {
          std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> > > > aRefs;
          (*aCurrentIter)->owner()->data()->referencesToObjects(aRefs);
          std::list<std::pair<std::string, std::list<std::shared_ptr<ModelAPI_Object> >>>::iterator
            aRefIter = aRefs.begin();
          for(; aRefIter != aRefs.end(); aRefIter++) {
            if ((*aCurrentIter)->id() == aRefIter->first) {
              std::list<std::shared_ptr<ModelAPI_Object> >::iterator anOIt;
              for(anOIt = aRefIter->second.begin(); anOIt != aRefIter->second.end(); anOIt++) {
                if (*anOIt == theObject) {
                  aLeaveIt = true;
                }
              }
            }
          }
        }
        if (!aLeaveIt) {
          aData->removeBackReference(*aCurrentIter);
          aCurrentIter = aData->refsToMe().begin(); // reinitialize iteration after delete
        } else aCurrentIter++;
      } else aCurrentIter++;
    }
  }
  // for the last feature in the folder, check if it is a sub-feature,
  // then refer the folder to a top-level parent composite feature
  const std::set<AttributePtr>& aRefs = aData->refsToMe();
  std::set<AttributePtr>::iterator anIt = aRefs.begin();
  for (; anIt != aRefs.end(); ++anIt)
    if ((*anIt)->id() == ModelAPI_Folder::LAST_FEATURE_ID())
      break;
  if (anIt != aRefs.end()) {
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature) {
      CompositeFeaturePtr aParent;
      CompositeFeaturePtr aGrandParent = ModelAPI_Tools::compositeOwner(aFeature);
      do {
        aParent = aGrandParent;
        if (aGrandParent)
          aGrandParent = ModelAPI_Tools::compositeOwner(aParent);
      } while (aGrandParent.get());
      if (aParent) {
        ObjectPtr aFolder = (*anIt)->owner();
        // remove reference from the current feature
        aData->removeBackReference(aFolder, ModelAPI_Folder::LAST_FEATURE_ID());
        // set reference to a top-level parent
        aFolder->data()->reference(ModelAPI_Folder::LAST_FEATURE_ID())->setValue(aParent);
        std::shared_ptr<Model_Data> aParentData =
            std::dynamic_pointer_cast<Model_Data>(aParent->data());
        aParentData->addBackReference(aFolder, ModelAPI_Folder::LAST_FEATURE_ID());
      }
    }
  }
  aData->updateConcealmentFlag();
}

static void collectReferences(std::shared_ptr<ModelAPI_Data> theData,
                              std::map<ObjectPtr, std::set<AttributePtr> >& theRefs)
{
  if (theData.get()) {
    std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
    theData->referencesToObjects(aRefs);
    std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRefsIt = aRefs.begin();
    for(; aRefsIt != aRefs.end(); aRefsIt++) {
      std::list<ObjectPtr>::iterator aRefTo = aRefsIt->second.begin();
      for(; aRefTo != aRefsIt->second.end(); aRefTo++) {
        if (*aRefTo) {
          std::map<ObjectPtr, std::set<AttributePtr> >::iterator aFound = theRefs.find(*aRefTo);
          if (aFound == theRefs.end()) {
            theRefs[*aRefTo] = std::set<AttributePtr>();
            aFound = theRefs.find(*aRefTo);
          }
          aFound->second.insert(theData->attribute(aRefsIt->first));
        }
      }
    }
  }
}

void Model_Objects::synchronizeBackRefs()
{
  // collect all back references in the separated container: to update everything at once,
  // without additional Concealment switching on and off: only the final modification

  // referenced (slave) objects to referencing attributes
  std::map<ObjectPtr, std::set<AttributePtr> > allRefs;
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFeatures(myFeatures);
  for(; aFeatures.More(); aFeatures.Next()) {
    FeaturePtr aFeature = aFeatures.Value();
    collectReferences(aFeature->data(), allRefs);
  }
  NCollection_DataMap<TDF_Label, ObjectPtr>::Iterator aFolders(myFolders);
  for(; aFolders.More(); aFolders.Next()) {
    ObjectPtr aFolder = aFolders.Value();
    collectReferences(aFolder->data(), allRefs);
  }
  // second iteration: just compare back-references with existing in features and results
  for(aFeatures.Initialize(myFeatures); aFeatures.More(); aFeatures.Next()) {
    FeaturePtr aFeature = aFeatures.Value();
    static std::set<AttributePtr> anEmpty;
    std::map<ObjectPtr, std::set<AttributePtr> >::iterator aFound = allRefs.find(aFeature);
    if (aFound == allRefs.end()) { // not found => erase all back references
      synchronizeBackRefsForObject(anEmpty, aFeature);
    } else {
      synchronizeBackRefsForObject(aFound->second, aFeature);
      allRefs.erase(aFound); // to check that all refs are counted
    }
    // also for results
    std::list<ResultPtr> aResults;
    ModelAPI_Tools::allResults(aFeature, aResults);
    std::list<ResultPtr>::iterator aRIter = aResults.begin();
    for(; aRIter != aResults.cend(); aRIter++) {
      aFound = allRefs.find(*aRIter);
      if (aFound == allRefs.end()) { // not found => erase all back references
        synchronizeBackRefsForObject(anEmpty, *aRIter);
      } else {
        synchronizeBackRefsForObject(aFound->second, *aRIter);
        allRefs.erase(aFound); // to check that all refs are counted
      }
    }
  }
  for(aFeatures.Initialize(myFeatures); aFeatures.More(); aFeatures.Next()) {
    FeaturePtr aFeature = aFeatures.Value();
    std::list<ResultPtr> aResults;
    ModelAPI_Tools::allResults(aFeature, aResults);
    // update the concealment status for display in isConcealed of ResultBody
    std::list<ResultPtr>::iterator aRIter = aResults.begin();
    for(; aRIter != aResults.cend(); aRIter++) {
      (*aRIter)->isConcealed();
    }
  }
  // the rest all refs means that feature references to the external document feature:
  // process also them
  std::map<ObjectPtr, std::set<AttributePtr> >::iterator anExtIter = allRefs.begin();
  for(; anExtIter != allRefs.end(); anExtIter++) {
    synchronizeBackRefsForObject(anExtIter->second, anExtIter->first);
  }
}

TDF_Label Model_Objects::resultLabel(
  const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theResultIndex)
{
  const std::shared_ptr<Model_Data>& aData =
    std::dynamic_pointer_cast<Model_Data>(theFeatureData);
  return aData->label().Father().FindChild(TAG_FEATURE_RESULTS).FindChild(theResultIndex + 1);
}

bool Model_Objects::hasCustomName(DataPtr theFeatureData,
                                  ResultPtr theResult,
                                  int /*theResultIndex*/,
                                  std::wstring& theParentName) const
{
  ResultBodyPtr aBodyRes = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theFeatureData->owner());
  if (aBodyRes) {
    // only for top-results (works for the cases when results are not yet added to the feature)
    FeaturePtr anOwner = ModelAPI_Feature::feature(theResult);

    // names of sub-solids in CompSolid should be default (for example,
    // result of boolean operation 'Boolean_1' is a CompSolid which is renamed to 'MyBOOL',
    // however, sub-elements of 'MyBOOL' should be named 'Boolean_1_1', 'Boolean_1_2' etc.)
    if (std::dynamic_pointer_cast<Model_Data>(aBodyRes->data())->label().Depth() == 6) {
      std::wostringstream aDefaultName;
      // compute default name of CompSolid (name of feature + index of CompSolid's result)
      int aBodyResultIndex = 0;
      const std::list<ResultPtr>& aResults = anOwner->results();
      std::list<ResultPtr>::const_iterator anIt = aResults.begin();
      for (; anIt != aResults.end(); ++anIt, ++aBodyResultIndex)
        if (aBodyRes == *anIt)
          break;
      aDefaultName << anOwner->name();
      aDefaultName << "_" << (aBodyResultIndex + 1);
      theParentName = aDefaultName.str();
    } else { // just name of the parent result if it is deeper than just a sub-result
      theParentName = aBodyRes->data()->name();
    }
    return false;
  }

  std::pair<std::wstring, bool> aName = ModelAPI_Tools::getDefaultName(theResult);
  if (aName.second)
    theParentName = aName.first;
  return aName.second;
}

void Model_Objects::storeResult(std::shared_ptr<ModelAPI_Data> theFeatureData,
                                std::shared_ptr<ModelAPI_Result> theResult,
                                const int theResultIndex,
                                const std::wstring& theNameShape)
{
  theResult->init();
  theResult->setDoc(myDoc);
  initData(theResult, resultLabel(theFeatureData, theResultIndex), TAG_FEATURE_ARGUMENTS);
  if (theResult->data()->name().empty()) {
    // if was not initialized, generate event and set a name
    std::wstring aNewName = theFeatureData->name();
    if (hasCustomName(theFeatureData, theResult, theResultIndex, aNewName)) {
      // if the name of result is user-defined, then, at first time, assign name of the result
      // by empty string to be sure that corresponding flag in the data model is set
      theResult->data()->setName(L"");
    } else {
      std::wstringstream aName;
      if ( theNameShape != L"" ){
        aName << theNameShape;
      } else {
        aName << aNewName;
        // if there are several results (issue #899: any number of result),
        // add unique prefix starting from second
        if (theResultIndex > 0 || theResult->groupName() == ModelAPI_ResultBody::group())
          aName << "_" << theResultIndex + 1;
      }
      aNewName = aName.str();
    }
    theResult->data()->setName(aNewName);
  }
}

std::shared_ptr<ModelAPI_ResultConstruction> Model_Objects::createConstruction(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  TDF_Label aLab = resultLabel(theFeatureData, theIndex);
  TDataStd_Comment::Set(aLab, ModelAPI_ResultConstruction::group().c_str());
  ObjectPtr anOldObject = object(aLab);
  std::shared_ptr<ModelAPI_ResultConstruction> aResult;
  if (anOldObject.get()) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(anOldObject);
  }
  if (!aResult.get()) {
    aResult = std::shared_ptr<ModelAPI_ResultConstruction>(new Model_ResultConstruction);
    storeResult(theFeatureData, aResult, theIndex);
  }
  return aResult;
}

std::shared_ptr<ModelAPI_ResultBody> Model_Objects::createBody(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData,
    const int theIndex,
    const std::wstring& theNameShape)
{
  TDF_Label aLab = resultLabel(theFeatureData, theIndex);
  TDataStd_Comment::Set(aLab, ModelAPI_ResultBody::group().c_str());
  ObjectPtr anOldObject = object(aLab);
  std::shared_ptr<ModelAPI_ResultBody> aResult;
  if (anOldObject.get()) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(anOldObject);
  }
  if (!aResult.get()) {
    aResult = std::shared_ptr<ModelAPI_ResultBody>(new Model_ResultBody);
    storeResult(theFeatureData, aResult, theIndex, theNameShape);
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
  if (anOldObject.get()) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultPart>(anOldObject);
  }
  if (!aResult.get()) {
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
  if (anOldObject.get()) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultGroup>(anOldObject);
  }
  if (!aResult.get()) {
    aResult = std::shared_ptr<ModelAPI_ResultGroup>(new Model_ResultGroup(theFeatureData));
    storeResult(theFeatureData, aResult, theIndex);
  }
  return aResult;
}

std::shared_ptr<ModelAPI_ResultField> Model_Objects::createField(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  TDF_Label aLab = resultLabel(theFeatureData, theIndex);
  TDataStd_Comment::Set(aLab, ModelAPI_ResultField::group().c_str());
  ObjectPtr anOldObject = object(aLab);
  std::shared_ptr<ModelAPI_ResultField> aResult;
  if (anOldObject.get()) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultField>(anOldObject);
  }
  if (!aResult.get()) {
    aResult = std::shared_ptr<ModelAPI_ResultField>(new Model_ResultField(theFeatureData));
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
  if (anOldObject.get()) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultParameter>(anOldObject);
  }
  if (!aResult.get()) {
    aResult = std::shared_ptr<ModelAPI_ResultParameter>(new Model_ResultParameter);
    storeResult(theFeatureData, aResult, theIndex);
  }
  return aResult;
}

std::shared_ptr<ModelAPI_Folder> Model_Objects::createFolder(
    const std::shared_ptr<ModelAPI_Feature>& theBeforeThis)
{
  FolderPtr aFolder(new ModelAPI_Folder);
  if (!aFolder)
    return aFolder;

  TDF_Label aFeaturesLab = featuresLabel();
  TDF_Label aFolderLab = aFeaturesLab.NewChild();
  // store feature in the features array: before "initData" because in macro features
  // in initData it creates new features, appeared later than this
  TDF_Label aPrevFeatureLab;
  if (theBeforeThis.get()) { // searching for the previous feature label
    std::shared_ptr<Model_Data> aPrevData =
        std::dynamic_pointer_cast<Model_Data>(theBeforeThis->data());
    if (aPrevData.get()) {
      int anIndex = kUNDEFINED_FEATURE_INDEX;
      aPrevFeatureLab = nextLabel(aPrevData->label().Father(), anIndex, true);
    }
  } else { // find the label of the last feature
    Handle(TDataStd_ReferenceArray) aRefs;
    if (aFeaturesLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
      aPrevFeatureLab = aRefs->Value(aRefs->Upper());
  }
  AddToRefArray(aFeaturesLab, aFolderLab, aPrevFeatureLab);

  // keep the feature ID to restore document later correctly
  TDataStd_Comment::Set(aFolderLab, ModelAPI_Folder::ID().c_str());
  myFolders.Bind(aFolderLab, aFolder);
  // must be before the event sending: for OB the feature is already added
  updateHistory(ModelAPI_Folder::group());
  updateHistory(ModelAPI_Feature::group());

  // must be after binding to the map because of "Box" macro feature that
  // creates other features in "initData"
  initData(aFolder, aFolderLab, TAG_FEATURE_ARGUMENTS);
  // event: folder is added, must be before "initData" to update OB correctly on Duplicate:
  // first new part, then the content
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
  ModelAPI_EventCreator::get()->sendUpdated(aFolder, anEvent);

  return aFolder;
}

void Model_Objects::removeFolder(std::shared_ptr<ModelAPI_Folder> theFolder)
{
  std::shared_ptr<Model_Data> aData = std::static_pointer_cast<Model_Data>(theFolder->data());
  if (!aData.get() || !aData->isValid())
    return;

  // this must be before erase since theFolder erasing removes all information about it
  clearHistory(theFolder);
  // erase fields
  theFolder->erase();

  TDF_Label aFolderLabel = aData->label().Father();
  if (myFolders.IsBound(aFolderLabel))
    myFolders.UnBind(aFolderLabel);

  static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  ModelAPI_EventCreator::get()->sendUpdated(theFolder, EVENT_DISP);
  // erase all attributes under the label of feature
  aFolderLabel.ForgetAllAttributes();
  // remove it from the references array
  RemoveFromRefArray(featuresLabel(), aFolderLabel);
  // event: feature is deleted
  ModelAPI_EventCreator::get()->sendDeleted(theFolder->document(), ModelAPI_Folder::group());
  updateHistory(ModelAPI_Folder::group());
  updateHistory(ModelAPI_Feature::group());
}

// Returns one of the limiting features of the list
static FeaturePtr limitingFeature(std::list<FeaturePtr>& theFeatures, const bool isLast)
{
  FeaturePtr aFeature;
  if (isLast) {
    aFeature = theFeatures.back();
    theFeatures.pop_back();
  } else {
    aFeature = theFeatures.front();
    theFeatures.pop_front();
  }
  return aFeature;
}

// Verify the feature is sub-element in composite feature or it is not used in the history
static bool isSkippedFeature(FeaturePtr theFeature)
{
  bool isSub = ModelAPI_Tools::compositeOwner(theFeature).get() != NULL;
  return isSub || (theFeature && !theFeature->isInHistory());
}

std::shared_ptr<ModelAPI_Folder> Model_Objects::findFolder(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
      const bool theBelow)
{
  if (theFeatures.empty())
    return FolderPtr(); // nothing to move

  TDF_Label aFeaturesLab = featuresLabel();
  Handle(TDataStd_ReferenceArray) aRefs;
  if (!aFeaturesLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
    return FolderPtr(); // no reference array (something is wrong)

  std::list<std::shared_ptr<ModelAPI_Feature> > aFeatures = theFeatures;
  std::shared_ptr<ModelAPI_Feature> aLimitingFeature = limitingFeature(aFeatures, theBelow);

  std::shared_ptr<Model_Data> aData =
      std::static_pointer_cast<Model_Data>(aLimitingFeature->data());
  if (!aData || !aData->isValid())
    return FolderPtr(); // invalid feature

  // label of the first feature in the list for fast searching
  TDF_Label aFirstFeatureLabel = aData->label().Father();

  // find a folder above the features and
  // check the given features represent a sequential list of objects following the folder
  FolderPtr aFoundFolder;
  TDF_Label aLastFeatureInFolder;
  int aRefIndex = aRefs->Lower();
  for(; aRefIndex <= aRefs->Upper(); ++aRefIndex) { // iterate all existing features
    TDF_Label aCurLabel = aRefs->Value(aRefIndex);
    if (IsEqual(aCurLabel, aFirstFeatureLabel))
      break; // no need to continue searching

    // searching the folder below, just continue to search last feature from the list
    if (theBelow)
      continue;

    // issue #18733: check for the last feature in folder before checking the sub-feature,
    //               because the folder may end by the feature which is
    //               neither a sub-feature nor a feature in history.
    if (!aLastFeatureInFolder.IsNull()) {
      if (IsEqual(aCurLabel, aLastFeatureInFolder))
        aLastFeatureInFolder.Nullify(); // the last feature in the folder is achieved
      continue;
    }

    // if feature is in sub-component, skip it
    FeaturePtr aCurFeature = feature(aCurLabel);
    if (isSkippedFeature(aCurFeature))
      continue;

    const ObjectPtr& aFolderObj = folder(aCurLabel);
    if (aFolderObj.get()) {
      aFoundFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(aFolderObj);
      AttributeReferencePtr aLastFeatAttr =
          aFoundFolder->reference(ModelAPI_Folder::LAST_FEATURE_ID());
      if (aLastFeatAttr) {
        // setup iterating inside a folder to find last feature
        ObjectPtr aLastFeature = aLastFeatAttr->value();
        if (aLastFeature) {
          aData = std::static_pointer_cast<Model_Data>(aLastFeature->data());
          if (aData && aData->isValid())
            aLastFeatureInFolder = aData->label().Father();
        }
      }
    }
  }

  if (theBelow && aRefIndex < aRefs->Upper()) {
    TDF_Label aLabel;
    // skip following features which are sub-components or not in history
    for (int anIndex = aRefIndex + 1; anIndex <= aRefs->Upper(); ++anIndex) {
      aLabel = aRefs->Value(anIndex);
      FeaturePtr aCurFeature = feature(aLabel);
      if (!isSkippedFeature(aCurFeature))
        break;
    }
    // check the next object is a folder
    aFoundFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(folder(aLabel));
  }

  if (!aLastFeatureInFolder.IsNull() || // the last feature of the folder above is not found
      !aFoundFolder)
    return FolderPtr();

  // check the given features are sequential list
  int aStep = theBelow ? -1 : 1;
  for (aRefIndex += aStep;
       !aFeatures.empty() && aRefIndex >= aRefs->Lower() && aRefIndex <= aRefs->Upper();
       aRefIndex += aStep) {
    TDF_Label aCurLabel = aRefs->Value(aRefIndex);
    // if feature is in sub-component, skip it
    FeaturePtr aCurFeature = feature(aCurLabel);
    if (isSkippedFeature(aCurFeature))
      continue;

    aLimitingFeature = limitingFeature(aFeatures, theBelow);
    if (!aCurFeature->data()->isEqual(aLimitingFeature->data()))
      return FolderPtr(); // not a sequential list
  }

  return aFoundFolder;
}

bool Model_Objects::moveToFolder(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
      const std::shared_ptr<ModelAPI_Folder>& theFolder)
{
  if (theFeatures.empty() || !theFolder)
    return false;

  // labels for the folder and last feature in the list
  TDF_Label aFolderLabel, aLastFeatureLabel;
  std::shared_ptr<Model_Data> aData =
      std::static_pointer_cast<Model_Data>(theFolder->data());
  if (aData && aData->isValid())
    aFolderLabel = aData->label().Father();
  aData = std::static_pointer_cast<Model_Data>(theFeatures.back()->data());
  if (aData && aData->isValid())
    aLastFeatureLabel = aData->label().Father();

  if (aFolderLabel.IsNull() || aLastFeatureLabel.IsNull())
    return false;

  AttributeReferencePtr aFirstFeatAttr =
      theFolder->reference(ModelAPI_Folder::FIRST_FEATURE_ID());
  AttributeReferencePtr aLastFeatAttr =
      theFolder->reference(ModelAPI_Folder::LAST_FEATURE_ID());
  bool initFirstAttr = !aFirstFeatAttr->value().get();
  bool initLastAttr  = !aLastFeatAttr->value().get();

  // check the folder is below the list of features
  bool isFolderBelow = false;
  TDF_Label aFeaturesLab = featuresLabel();
  Handle(TDataStd_ReferenceArray) aRefs;
  if (!aFeaturesLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
    return false; // no reference array (something is wrong)
  for (int aRefIndex = aRefs->Lower(); aRefIndex <= aRefs->Upper(); ++aRefIndex) {
    TDF_Label aCurLabel = aRefs->Value(aRefIndex);
    if (aCurLabel == aFolderLabel)
      break; // folder is above the features
    else if (aCurLabel == aLastFeatureLabel) {
      isFolderBelow = true;
      break;
    }
  }

  if (isFolderBelow) {
    aData = std::static_pointer_cast<Model_Data>(theFeatures.front()->data());
    if (!aData || !aData->isValid())
      return false;
    TDF_Label aPrevFeatureLabel = aData->label().Father();
    // label of the feature before the first feature in the list
    for (int aRefIndex = aRefs->Lower(); aRefIndex <= aRefs->Upper(); ++aRefIndex)
      if (aPrevFeatureLabel == aRefs->Value(aRefIndex)) {
        if (aRefIndex == aRefs->Lower())
          aPrevFeatureLabel.Nullify();
        else
          aPrevFeatureLabel = aRefs->Value(aRefIndex - 1);
        break;
      }

    // move the folder in the list of references before the first feature
    RemoveFromRefArray(aFeaturesLab, aFolderLabel);
    AddToRefArray(aFeaturesLab, aFolderLabel, aPrevFeatureLabel);
    // update first feature of the folder
    initFirstAttr = true;
  } else {
    // update last feature of the folder
    initLastAttr = true;
  }

  if (initFirstAttr)
    aFirstFeatAttr->setValue(theFeatures.front());
  if (initLastAttr)
    aLastFeatAttr->setValue(theFeatures.back());

  updateHistory(ModelAPI_Feature::group());
  return true;
}

static FolderPtr isExtractionCorrect(const FolderPtr& theFirstFeatureFolder,
                                     const FolderPtr& theLastFeatureFolder,
                                     bool& isExtractBefore)
{
  if (theFirstFeatureFolder.get()) {
    if (theLastFeatureFolder.get())
      return theFirstFeatureFolder == theLastFeatureFolder ? theFirstFeatureFolder : FolderPtr();
    else
      isExtractBefore = true;
    return theFirstFeatureFolder;
  } else if (theLastFeatureFolder.get()) {
    isExtractBefore = false;
    return theLastFeatureFolder;
  }
  // no folder found
  return FolderPtr();
}

bool Model_Objects::removeFromFolder(
      const std::list<std::shared_ptr<ModelAPI_Feature> >& theFeatures,
      const bool theBefore)
{
  if (theFeatures.empty())
    return false;

  FolderPtr aFirstFeatureFolder =
      inFolder(theFeatures.front(), ModelAPI_Folder::FIRST_FEATURE_ID());
  FolderPtr aLastFeatureFolder =
      inFolder(theFeatures.back(),  ModelAPI_Folder::LAST_FEATURE_ID());

  bool isExtractBeforeFolder = theBefore;
  FolderPtr aFoundFolder =
      isExtractionCorrect(aFirstFeatureFolder, aLastFeatureFolder, isExtractBeforeFolder);
  if (!aFoundFolder)
    return false; // list of features cannot be extracted

  // references of the current folder
  ObjectPtr aFolderStartFeature;
  ObjectPtr aFolderEndFeature;
  if (aFirstFeatureFolder != aLastFeatureFolder) {
    aFolderStartFeature = aFoundFolder->reference(ModelAPI_Folder::FIRST_FEATURE_ID())->value();
    aFolderEndFeature   = aFoundFolder->reference(ModelAPI_Folder::LAST_FEATURE_ID())->value();
  }

  FeaturePtr aFeatureToFind = isExtractBeforeFolder ? theFeatures.back() : theFeatures.front();
  std::shared_ptr<Model_Data> aData =
      std::static_pointer_cast<Model_Data>(aFeatureToFind->data());
  if (!aData || !aData->isValid())
    return false;
  TDF_Label aLabelToFind = aData->label().Father();

  // search the label in the list of references
  TDF_Label aFeaturesLab = featuresLabel();
  Handle(TDataStd_ReferenceArray) aRefs;
  if (!aFeaturesLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
    return false; // no reference array (something is wrong)
  int aRefIndex = aRefs->Lower();
  for (; aRefIndex <= aRefs->Upper(); ++aRefIndex)
    if (aRefs->Value(aRefIndex) == aLabelToFind)
      break;

  // update folder position
  if (isExtractBeforeFolder) {
    aData = std::dynamic_pointer_cast<Model_Data>(aFoundFolder->data());
    TDF_Label aFolderLabel = aData->label().Father();
    TDF_Label aPrevFeatureLabel = aRefs->Value(aRefIndex);
    // update start reference of the folder
    if (aFolderStartFeature.get()) {
      FeaturePtr aNewStartFeature;
      do { // skip all features placed in the composite features
        aPrevFeatureLabel = aRefs->Value(aRefIndex++);
        aNewStartFeature =
            aRefIndex <= aRefs->Upper() ? feature(aRefs->Value(aRefIndex)) : FeaturePtr();
      } while (aNewStartFeature && isSkippedFeature(aNewStartFeature));
      aFolderStartFeature = aNewStartFeature;
    }
    // move the folder in the list of references after the last feature from the list
    RemoveFromRefArray(aFeaturesLab, aFolderLabel);
    AddToRefArray(aFeaturesLab, aFolderLabel, aPrevFeatureLabel);
  } else {
    // update end reference of the folder
    if (aFolderEndFeature.get()) {
      FeaturePtr aNewEndFeature;
      do { // skip all features placed in the composite features
        --aRefIndex;
        aNewEndFeature =
            aRefIndex >= aRefs->Lower() ? feature(aRefs->Value(aRefIndex)) : FeaturePtr();
      } while (aNewEndFeature && isSkippedFeature(aNewEndFeature));
      aFolderEndFeature = aNewEndFeature;
    }
  }

  // update folder references
  aFoundFolder->reference(ModelAPI_Folder::FIRST_FEATURE_ID())->setValue(aFolderStartFeature);
  aFoundFolder->reference(ModelAPI_Folder::LAST_FEATURE_ID())->setValue(aFolderEndFeature);

  updateHistory(ModelAPI_Feature::group());
  return true;
}

FolderPtr Model_Objects::findContainingFolder(const FeaturePtr& theFeature, int& theIndexInFolder)
{
  // search the label in the list of references
  TDF_Label aFeaturesLab = featuresLabel();
  Handle(TDataStd_ReferenceArray) aRefs;
  if (!aFeaturesLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
    return FolderPtr(); // no reference array (something is wrong)

  std::shared_ptr<Model_Data> aData =
      std::static_pointer_cast<Model_Data>(theFeature->data());
  if (!aData || !aData->isValid())
    return FolderPtr();
  TDF_Label aLabelToFind = aData->label().Father();

  theIndexInFolder = -1;
  FolderPtr aFoundFolder;
  TDF_Label aLastFeatureLabel;

  for (int aRefIndex = aRefs->Lower(); aRefIndex <= aRefs->Upper(); ++aRefIndex) {
    TDF_Label aCurLabel = aRefs->Value(aRefIndex);

    if (aFoundFolder)
      ++theIndexInFolder;

    if (aCurLabel == aLabelToFind) { // the feature is reached
      if (aFoundFolder) {
        if (isSkippedFeature(theFeature)) {
          theIndexInFolder = -1;
          return FolderPtr();
        }
        // decrease the index of the feature in the folder by the number of skipped features
        for (int anIndex = theIndexInFolder - 1; anIndex > 0; anIndex--) {
          aCurLabel = aRefs->Value(aRefIndex - anIndex);
          if (isSkippedFeature(feature(aCurLabel)))
            theIndexInFolder--;
        }
      }
      return aFoundFolder;
    }

    if (!aFoundFolder) {
      // if the current label refers to a folder, feel all necessary data
      const ObjectPtr& aFolderObj = folder(aCurLabel);
      if (aFolderObj.get()) {
        aFoundFolder = std::dynamic_pointer_cast<ModelAPI_Folder>(aFolderObj);
        theIndexInFolder = -1;

        AttributeReferencePtr aLastRef =
            aFoundFolder->reference(ModelAPI_Folder::LAST_FEATURE_ID());
        if (aLastRef->value()) {
          aData = std::static_pointer_cast<Model_Data>(aLastRef->value()->data());
          if (aData && aData->isValid())
            aLastFeatureLabel = aData->label().Father();
        } else // folder is empty
          aFoundFolder = FolderPtr();
      }
    } else if (aLastFeatureLabel == aCurLabel) {
      // folder is finished, clear all stored data
      theIndexInFolder = -1;
      aFoundFolder = FolderPtr();
    }
  }

  // folder is not found
  theIndexInFolder = -1;
  return FolderPtr();
}


std::shared_ptr<ModelAPI_Feature> Model_Objects::feature(
    const std::shared_ptr<ModelAPI_Result>& theResult)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theResult->data());
  if (aData.get() && aData->isValid()) {
    TDF_Label aFeatureLab = aData->label().Father().Father().Father();
    FeaturePtr aFeature = feature(aFeatureLab);
    while(!aFeature.get() && aFeatureLab.Depth() > 1) { // this may be sub-result of result
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
      if (anArgLab.FindAttribute(TDataStd_Comment::GetID(), aGroup)) {
        return TCollection_AsciiString(aGroup->Get()).ToCString();
      }
    }
  }
  static std::string anEmpty;
  return anEmpty; // not found
}

void Model_Objects::updateResults(FeaturePtr theFeature, std::set<FeaturePtr>& theProcessed)
{
  if (theProcessed.find(theFeature) != theProcessed.end())
    return;
  theProcessed.insert(theFeature);
  // for composites update subs recursively (sketch elements results are needed for the sketch)
  CompositeFeaturePtr aComp = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theFeature);
  if (aComp.get() && aComp->getKind() != "Part") { // don't go inside of parts sub-features
    // update subs of composites first
    int aSubNum = aComp->numberOfSubs();
    for(int a = 0; a < aSubNum; a++) {
      FeaturePtr aSub = aComp->subFeature(a);
      updateResults(aComp->subFeature(a), theProcessed);
    }
  }

  // it may be on undo
  if (!theFeature->data() || !theFeature->data()->isValid() || theFeature->isDisabled())
    return;

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
  // check that results are presented on all labels
  int aResSize = int(theFeature->results().size());
  TDF_ChildIterator aLabIter(resultLabel(theFeature->data(), 0).Father());
  for(; aLabIter.More(); aLabIter.Next()) {
    // here must be GUID of the feature
    int aResIndex = aLabIter.Value().Tag() - 1;
    ResultPtr aNewBody;
    if (aResSize <= aResIndex) {
      TDF_Label anArgLab = aLabIter.Value();
      Handle(TDataStd_Comment) aGroup;
      if (anArgLab.FindAttribute(TDataStd_Comment::GetID(), aGroup)) {
        if (aGroup->Get() == ModelAPI_ResultBody::group().c_str()) {
          aNewBody = createBody(theFeature->data(), aResIndex);
        } else if (aGroup->Get() == ModelAPI_ResultPart::group().c_str()) {
          if (aResIndex <= (int)theFeature->results().size()) {// to avoid crash if previous execute
            // for index = 0 erases result
            std::shared_ptr<ModelAPI_ResultPart> aNewP = createPart(theFeature->data(), aResIndex);
            if (!aNewP->data()->isDeleted()) {
              theFeature->setResult(aNewP, aResIndex);
              if (!aNewP->partDoc().get())
                // create the part result: it is better to restore the previous result if possible
                theFeature->execute();
            }
          }
        } else if (aGroup->Get() == ModelAPI_ResultConstruction::group().c_str()) {
          ResultConstructionPtr aConstr = createConstruction(theFeature->data(), aResIndex);
          if (!aConstr->data()->isDeleted()) {
            if (!aConstr->updateShape())
              theFeature->execute(); // not stored shape in the data structure, execute to have it
            else
              theFeature->setResult(aConstr, aResIndex); // result is ready without execution
          }
        } else if (aGroup->Get() == ModelAPI_ResultGroup::group().c_str()) {
          aNewBody = createGroup(theFeature->data(), aResIndex);
        } else if (aGroup->Get() == ModelAPI_ResultField::group().c_str()) {
          ResultFieldPtr aField = createField(theFeature->data(), aResIndex);
          aField->updateSteps(); // to refresh the internal data
          aNewBody = aField;
        } else if (aGroup->Get() == ModelAPI_ResultParameter::group().c_str()) {
          theFeature->attributeChanged("expression"); // just produce a value
        } else {
          Events_InfoMessage("Model_Objects", "Unknown type of result is found in the document:")
            .arg(TCollection_AsciiString(aGroup->Get()).ToCString()).send();
        }
      }
      if (aNewBody && !aNewBody->data()->isDeleted()) {
        theFeature->setResult(aNewBody, aResIndex);
      }
    }
  }
  if (aResSize > 0) { // check there exist a body that must be updated
    std::list<ResultPtr>::const_iterator aRes = theFeature->results().cbegin();
    for (; aResSize && aRes != theFeature->results().cend(); aRes++, aResSize++) {
      if ((*aRes)->data()->isValid()) {
        if ((*aRes)->groupName() == ModelAPI_ResultBody::group()) {
          ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aRes);
          aBody->updateSubs(aBody->shape(), false);
        } else if ((*aRes)->groupName() == ModelAPI_ResultConstruction::group()) {
          // update the cashed myShape presented in construction
          ResultConstructionPtr aConstr =
            std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
          aConstr->updateShape();
        }
      }
    }
  }
}

ResultPtr Model_Objects::findByName(const std::wstring theName)
{
  ResultPtr aResult;
  FeaturePtr aResFeature; // keep feature to return the latest one
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator anObjIter(myFeatures);
  for(; anObjIter.More(); anObjIter.Next()) {
    FeaturePtr& aFeature = anObjIter.ChangeValue();
    if (!aFeature.get() || aFeature->isDisabled()) // may be on close
      continue;
    std::list<ResultPtr> allResults;
    ModelAPI_Tools::allResults(aFeature, allResults);
    std::list<ResultPtr>::iterator aRIter = allResults.begin();
    for (; aRIter != allResults.cend(); aRIter++) {
      ResultPtr aRes = *aRIter;
      if (aRes.get() && aRes->data() && aRes->data()->isValid() && !aRes->isDisabled() &&
          aRes->data()->name() == theName)
      {
        if (!aResult.get() || isLater(aFeature, aResFeature)) { // select the latest
          aResult = aRes;
          aResFeature = aFeature;
        }
      }
    }
  }
  return aResult;
}

TDF_Label Model_Objects::nextLabel(TDF_Label theCurrent, int& theIndex, const bool theReverse)
{
  Handle(TDataStd_ReferenceArray) aRefs;
  if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    int aStart = theIndex == kUNDEFINED_FEATURE_INDEX ? aRefs->Lower() : theIndex;
    for(int a = aStart; a <= aRefs->Upper(); a++) { // iterate all existing features
      TDF_Label aCurLab = aRefs->Value(a);
      if (aCurLab.IsEqual(theCurrent)) {
        a += theReverse ? -1 : 1;
        if (a >= aRefs->Lower() && a <= aRefs->Upper()) {
          theIndex = a;
          return aRefs->Value(a);
        }
        break; // finish iteration: it's last feature
      }
    }
  }
  return TDF_Label();
}

FeaturePtr Model_Objects::nextFeature(FeaturePtr theCurrent, int& theIndex, const bool theReverse)
{
  std::shared_ptr<Model_Data> aData = std::static_pointer_cast<Model_Data>(theCurrent->data());
  if (aData.get() && aData->isValid()) {
    TDF_Label aFeatureLabel = aData->label().Father();
    do {
      TDF_Label aNextLabel = nextLabel(aFeatureLabel, theIndex, theReverse);
      if (aNextLabel.IsNull())
        break; // the last or something is wrong
      FeaturePtr aFound = feature(aNextLabel);
      if (aFound)
        return aFound; // the feature is found
      // if the next label is a folder, skip it
      aFeatureLabel = folder(aNextLabel).get() ? aNextLabel : TDF_Label();
    } while (!aFeatureLabel.IsNull());
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
    // comment this because of #2674 - features are removed from array on deactivation of Part
    /*FeaturePtr aLast = feature(aRefs->Value(aRefs->Upper()));
    if (!aLast.get() && aRefs->Length() != 0) { // erase the invalid feature from the array
      RemoveFromRefArray(featuresLabel(), aRefs->Value(aRefs->Upper()));
      return lastFeature(); // try once again, after the last was removed
    }*/
    return feature(aRefs->Value(aRefs->Upper()));
  }
  return FeaturePtr(); // no features at all
}

bool Model_Objects::isLater(FeaturePtr theLater, FeaturePtr theCurrent) const
{
  if (theLater->getKind() == "InternalSelectionInPartFeature")
    return true;
  std::shared_ptr<Model_Data> aLaterD = std::static_pointer_cast<Model_Data>(theLater->data());
  std::shared_ptr<Model_Data> aCurrentD = std::static_pointer_cast<Model_Data>(theCurrent->data());
  if (aLaterD.get() && aLaterD->isValid() && aCurrentD.get() && aCurrentD->isValid()) {
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

std::list<std::shared_ptr<ModelAPI_Object> > Model_Objects::allObjects()
{
  std::list<std::shared_ptr<ModelAPI_Object> > aResult;
  Handle(TDataStd_ReferenceArray) aRefs;
  if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    for(int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
      ObjectPtr anObject = object(aRefs->Value(a));
      if (!anObject.get()) // is it a folder?
        anObject = folder(aRefs->Value(a));
      if (anObject.get())
        aResult.push_back(anObject);
    }
  }
  return aResult;
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
