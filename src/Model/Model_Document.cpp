// File:        Model_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#include <Model_Document.h>
#include <Model_Data.h>
#include <Model_Application.h>
#include <Model_PluginManager.h>
#include <Model_Events.h>
#include <Model_ResultPart.h>
#include <Model_ResultConstruction.h>
#include <Model_ResultBody.h>
#include <Events_Loop.h>
#include <Events_Error.h>

#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_HLabelArray1.hxx>
#include <TDataStd_Name.hxx>
#include <TDF_Reference.hxx>

#include <climits>
#ifndef WIN32
#include <sys/stat.h>
#endif

#ifdef WIN32
# define _separator_ '\\'
#else
# define _separator_ '/'
#endif

static const int UNDO_LIMIT = 10; // number of possible undo operations

static const int TAG_GENERAL = 1; // general properties tag
static const int TAG_OBJECTS = 2; // tag of the objects sub-tree (features, results)
static const int TAG_HISTORY = 3; // tag of the history sub-tree (python dump)

// feature sub-labels
static const int TAG_FEATURE_ARGUMENTS = 1; ///< where the arguments are located
static const int TAG_FEATURE_RESULTS = 2; ///< where the results are located

/// Returns the file name of this document by the nameof directory and identifuer of a document
static TCollection_ExtendedString DocFileName(const char* theFileName, const std::string& theID)
{
  TCollection_ExtendedString aPath ((const Standard_CString)theFileName);
  aPath += _separator_;
  aPath += theID.c_str();
  aPath += ".cbf"; // standard binary file extension
  return aPath;
}

bool Model_Document::load(const char* theFileName)
{
  Handle(Model_Application) anApp = Model_Application::getApplication();
  if (this == Model_PluginManager::get()->rootDocument().get()) {
    anApp->setLoadPath(theFileName);
  }
  TCollection_ExtendedString aPath (DocFileName(theFileName, myID));
  PCDM_ReaderStatus aStatus = (PCDM_ReaderStatus) -1;
  try
  {
    aStatus = anApp->Open(aPath, myDoc);
  }
  catch (Standard_Failure)
  {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    Events_Error::send(std::string("Exception in opening of document: ") + aFail->GetMessageString());
    return false;
  }
  bool isError = aStatus != PCDM_RS_OK;
  if (isError)
  {
    switch (aStatus)
    {
    case PCDM_RS_UnknownDocument: 
      Events_Error::send(std::string("Can not open document: PCDM_RS_UnknownDocument")); break;
    case PCDM_RS_AlreadyRetrieved: 
      Events_Error::send(std::string("Can not open document: PCDM_RS_AlreadyRetrieved")); break;
    case PCDM_RS_AlreadyRetrievedAndModified:
      Events_Error::send(
        std::string("Can not open document: PCDM_RS_AlreadyRetrievedAndModified"));
      break;
    case PCDM_RS_NoDriver:
      Events_Error::send(std::string("Can not open document: PCDM_RS_NoDriver")); break;
    case PCDM_RS_UnknownFileDriver:
      Events_Error::send(std::string("Can not open document: PCDM_RS_UnknownFileDriver")); break;
    case PCDM_RS_OpenError:
      Events_Error::send(std::string("Can not open document: PCDM_RS_OpenError")); break;
    case PCDM_RS_NoVersion:
      Events_Error::send(std::string("Can not open document: PCDM_RS_NoVersion")); break;
    case PCDM_RS_NoModel:
      Events_Error::send(std::string("Can not open document: PCDM_RS_NoModel")); break;
    case PCDM_RS_NoDocument:
      Events_Error::send(std::string("Can not open document: PCDM_RS_NoDocument")); break;
    case PCDM_RS_FormatFailure:
      Events_Error::send(std::string("Can not open document: PCDM_RS_FormatFailure")); break;
    case PCDM_RS_TypeNotFoundInSchema:
      Events_Error::send(std::string("Can not open document: PCDM_RS_TypeNotFoundInSchema")); 
      break;
    case PCDM_RS_UnrecognizedFileFormat:
      Events_Error::send(std::string("Can not open document: PCDM_RS_UnrecognizedFileFormat")); 
      break;
    case PCDM_RS_MakeFailure:
      Events_Error::send(std::string("Can not open document: PCDM_RS_MakeFailure")); break;
    case PCDM_RS_PermissionDenied:
      Events_Error::send(std::string("Can not open document: PCDM_RS_PermissionDenied")); break;
    case PCDM_RS_DriverFailure:
      Events_Error::send(std::string("Can not open document: PCDM_RS_DriverFailure")); break;
    default:
      Events_Error::send(std::string("Can not open document: unknown error")); break;
    }
  }
  if (!isError) {
    myDoc->SetUndoLimit(UNDO_LIMIT);
    synchronizeFeatures();
  }
  return !isError;
}

bool Model_Document::save(const char* theFileName)
{
  // create a directory in the root document if it is not yet exist
  if (this == Model_PluginManager::get()->rootDocument().get()) {
#ifdef WIN32
    CreateDirectory(theFileName, NULL);
#else
    mkdir(theFileName, 0x1ff); 
#endif
  }
  // filename in the dir is id of document inside of the given directory
  TCollection_ExtendedString aPath(DocFileName(theFileName, myID));
  PCDM_StoreStatus aStatus;
  try {
    aStatus = Model_Application::getApplication()->SaveAs(myDoc, aPath);
  }
  catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    Events_Error::send(std::string("Exception in saving of document: ") + aFail->GetMessageString());
    return false;
  }
  bool isDone = aStatus == PCDM_SS_OK || aStatus == PCDM_SS_No_Obj;
  if (!isDone)
  {
    switch (aStatus)
    {
    case PCDM_SS_DriverFailure:
      Events_Error::send(std::string("Can not save document: PCDM_SS_DriverFailure"));
      break;
    case PCDM_SS_WriteFailure:
      Events_Error::send(std::string("Can not save document: PCDM_SS_WriteFailure"));
      break;
    case PCDM_SS_Failure:
    default:
      Events_Error::send(std::string("Can not save document: PCDM_SS_Failure"));
      break;
    }
  }
  myTransactionsAfterSave = 0;
  if (isDone) { // save also sub-documents if any
    std::set<std::string>::iterator aSubIter = mySubs.begin();
    for(; aSubIter != mySubs.end() && isDone; aSubIter++)
      isDone = subDocument(*aSubIter)->save(theFileName);
  }
  return isDone;
}

void Model_Document::close()
{
  boost::shared_ptr<ModelAPI_PluginManager> aPM = Model_PluginManager::get();
  if (this != aPM->rootDocument().get() && 
      this == aPM->currentDocument().get()) {
    aPM->setCurrentDocument(aPM->rootDocument());
  }
  // close all subs
  std::set<std::string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->close();
  mySubs.clear();
  // close this
  /* do not close because it can be undoed
  if (myDoc->CanClose() == CDM_CCS_OK)
    myDoc->Close();
  Model_Application::getApplication()->deleteDocument(myID);
  */
}

void Model_Document::startOperation()
{
  if (myDoc->HasOpenCommand()) { // start of nested command
    if (myNestedNum == -1) {
      myNestedNum = 0;
      myDoc->InitDeltaCompaction();
    }
    myIsEmptyTr[myTransactionsAfterSave] = false;
    myTransactionsAfterSave++;
    myDoc->NewCommand();
  } else { // start of simple command
    myDoc->NewCommand();
  }
  // new command for all subs
  std::set<std::string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->startOperation();
}

void Model_Document::compactNested() {
  while(myNestedNum != -1) {
    myTransactionsAfterSave--;
    myIsEmptyTr.erase(myTransactionsAfterSave);
    myNestedNum--;
  }
  myIsEmptyTr[myTransactionsAfterSave] = false;
  myTransactionsAfterSave++;
  myDoc->PerformDeltaCompaction();
}

void Model_Document::finishOperation()
{
  // just to be sure that everybody knows that changes were performed
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));

  if (myNestedNum != -1) // this nested transaction is owervritten
    myNestedNum++;
  if (!myDoc->HasOpenCommand()) {
    if (myNestedNum != -1) {
      myNestedNum--;
      compactNested();
    }
  } else {
    // returns false if delta is empty and no transaction was made
    myIsEmptyTr[myTransactionsAfterSave] = !myDoc->CommitCommand() && (myNestedNum == -1);
    myTransactionsAfterSave++;
  }

  // finish for all subs
  std::set<std::string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->finishOperation();
}

void Model_Document::abortOperation()
{
  if (myNestedNum > 0 && !myDoc->HasOpenCommand()) { // abort all what was done in nested
    // first compact all nested
    compactNested();
    // for nested it is undo and clear redos
    myDoc->Undo();
    myDoc->ClearRedos();
    myTransactionsAfterSave--;
    myIsEmptyTr.erase(myTransactionsAfterSave);
  } else {
    if (myNestedNum == 0) // abort only high-level
      myNestedNum = -1;
    myDoc->AbortCommand();
  }
  synchronizeFeatures(true);
  // abort for all subs
  std::set<std::string>::iterator aSubIter = mySubs.begin();
    for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->abortOperation();
}

bool Model_Document::isOperation()
{
  // operation is opened for all documents: no need to check subs
  return myDoc->HasOpenCommand() == Standard_True;
}

bool Model_Document::isModified()
{
  // is modified if at least one operation was commited and not undoed
  return myTransactionsAfterSave > 0;
}

bool Model_Document::canUndo()
{
  if (myDoc->GetAvailableUndos() > 0 && myNestedNum != 0 && myTransactionsAfterSave != 0 /* for omitting the first useless transaction */)
    return true;
  // check other subs contains operation that can be undoed
  std::set<std::string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    if (subDocument(*aSubIter)->canUndo())
      return true;
  return false;
}

void Model_Document::undo()
{
  myTransactionsAfterSave--;
  if (myNestedNum > 0) myNestedNum--;
  if (!myIsEmptyTr[myTransactionsAfterSave])
    myDoc->Undo();
  synchronizeFeatures(true);
  // undo for all subs
  std::set<std::string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->undo();
}

bool Model_Document::canRedo()
{
  if (myDoc->GetAvailableRedos() > 0)
    return true;
  // check other subs contains operation that can be redoed
  std::set<std::string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    if (subDocument(*aSubIter)->canRedo())
      return true;
  return false;
}

void Model_Document::redo()
{
  if (myNestedNum != -1) myNestedNum++;
  if (!myIsEmptyTr[myTransactionsAfterSave])
    myDoc->Redo();
  myTransactionsAfterSave++;
  synchronizeFeatures(true);
  // redo for all subs
  std::set<std::string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->redo();
}

/// Appenad to the array of references a new referenced label
static void AddToRefArray(TDF_Label& theArrayLab, TDF_Label& theReferenced) {
  Handle(TDataStd_ReferenceArray) aRefs;
  if (!theArrayLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    aRefs = TDataStd_ReferenceArray::Set(theArrayLab, 0, 0);
    aRefs->SetValue(0, theReferenced);
  } else { // extend array by one more element
    Handle(TDataStd_HLabelArray1) aNewArray = 
      new TDataStd_HLabelArray1(aRefs->Lower(), aRefs->Upper() + 1);
    for(int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
      aNewArray->SetValue(a, aRefs->Value(a));
    }
    aNewArray->SetValue(aRefs->Upper() + 1, theReferenced);
    aRefs->SetInternalArray(aNewArray);
  }
}

FeaturePtr Model_Document::addFeature(std::string theID)
{
  TDF_Label anEmptyLab;
  FeaturePtr anEmptyFeature;
  FeaturePtr aFeature = ModelAPI_PluginManager::get()->createFeature(theID);
  if (aFeature) {
    TDF_Label aFeatureLab;
    if (!aFeature->isAction()) {// do not add action to the data model
      TDF_Label aFeaturesLab = groupLabel(ModelAPI_Feature::group());
      aFeatureLab = aFeaturesLab.NewChild();
      initData(aFeature, aFeatureLab, TAG_FEATURE_ARGUMENTS);
      // keep the feature ID to restore document later correctly
      TDataStd_Comment::Set(aFeatureLab, aFeature->getKind().c_str());
      setUniqueName(aFeature);
      myObjs[ModelAPI_Feature::group()].push_back(aFeature);
      // store feature in the history of features array
      if (aFeature->isInHistory()) {
        AddToRefArray(aFeaturesLab, aFeatureLab);
      }
    }
    if (!aFeature->isAction()) {// do not add action to the data model
      // event: feature is added
      static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
      ModelAPI_EventCreator::get()->sendUpdated(aFeature, anEvent);
    }
  }
  return aFeature;
}

/// Appenad to the array of references a new referenced label.
/// If theIndex is not -1, removes element at thisindex, not theReferenced.
/// \returns the index of removed element
static int RemoveFromRefArray(
  TDF_Label theArrayLab, TDF_Label theReferenced, const int theIndex = -1) {
  int aResult = -1; // no returned
  Handle(TDataStd_ReferenceArray) aRefs;
  if (theArrayLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    if (aRefs->Length() == 1) { // just erase an array
      if ((theIndex == -1 && aRefs->Value(0) == theReferenced) || theIndex == 0) {
        theArrayLab.ForgetAttribute(TDataStd_ReferenceArray::GetID());
      }
      aResult = 0;
    } else { // reduce the array
      Handle(TDataStd_HLabelArray1) aNewArray = 
        new TDataStd_HLabelArray1(aRefs->Lower(), aRefs->Upper() - 1);
      int aCount = aRefs->Lower();
      for(int a = aCount; a <= aRefs->Upper(); a++, aCount++) {
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

void Model_Document::removeFeature(FeaturePtr theFeature)
{
  boost::shared_ptr<Model_Data> aData = boost::static_pointer_cast<Model_Data>(theFeature->data());
  TDF_Label aFeatureLabel = aData->label().Father();
  // remove feature from the myObjects list
  std::vector<ObjectPtr>& aVec = myObjs[ModelAPI_Feature::group()];
  std::vector<ObjectPtr>::iterator anIter = aVec.begin();
  while(anIter != aVec.end()) {
    if (*anIter == theFeature) {
      anIter = aVec.erase(anIter);
    } else {
      anIter++;
    }
  }
  // erase all attributes under the label of feature
  aFeatureLabel.ForgetAllAttributes();
  // remove it from the references array
  RemoveFromRefArray(groupLabel(ModelAPI_Feature::group()), aData->label());

  // event: feature is deleted
  ModelAPI_EventCreator::get()->sendDeleted(theFeature->document(), ModelAPI_Feature::group());
  // results of this feature must be redisplayed
  static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = theFeature->results();
  std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
  for(; aRIter != aResults.cend(); aRIter++) {
    boost::shared_ptr<ModelAPI_Result> aRes = *aRIter;
    aRes->setData(boost::shared_ptr<ModelAPI_Data>()); // deleted flag
    ModelAPI_EventCreator::get()->sendUpdated(aRes, EVENT_DISP);
  }
}

/// returns the object group name by the object label
static std::string groupName(TDF_Label theObjectLabel) {
  TDF_Label aGroupLab = theObjectLabel.Father();
  Handle(TDataStd_Comment) aComment;
  if (aGroupLab.FindAttribute(TDataStd_Comment::GetID(), aComment))
    return std::string(TCollection_AsciiString(aComment->Get()).ToCString());
  return ""; // not found
}

FeaturePtr Model_Document::feature(TDF_Label& theLabel)
{
  // iterate all features, may be optimized later by keeping labels-map
  std::vector<ObjectPtr>& aVec = myObjs[ModelAPI_Feature::group()];
  std::vector<ObjectPtr>::iterator aFIter = aVec.begin();
  for(; aFIter != aVec.end(); aFIter++) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>((*aFIter)->data());
    if (aData->label().IsEqual(theLabel))
      return boost::dynamic_pointer_cast<ModelAPI_Feature>(*aFIter);
  }
  return FeaturePtr(); // not found
}

ObjectPtr Model_Document::object(TDF_Label& theLabel)
{
  // iterate all features, may be optimized later by keeping labels-map
  std::vector<ObjectPtr>& aVec = myObjs[ModelAPI_Feature::group()];
  std::vector<ObjectPtr>::iterator aFIter = aVec.begin();
  for(; aFIter != aVec.end(); aFIter++) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>((*aFIter)->data());
    if (aData->label().IsEqual(theLabel))
      return *aFIter;
    std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = 
      boost::dynamic_pointer_cast<ModelAPI_Feature>(*aFIter)->results();
    std::list<boost::shared_ptr<ModelAPI_Result> >::iterator aRIter = aResults.begin();
    for(; aRIter != aResults.end(); aRIter++) {
      boost::shared_ptr<Model_Data> aResData = 
        boost::dynamic_pointer_cast<Model_Data>((*aRIter)->data());
      if (aResData->label().IsEqual(theLabel))
        return *aRIter;
    }
  }
  return FeaturePtr(); // not found
}

boost::shared_ptr<ModelAPI_Document> Model_Document::subDocument(std::string theDocID)
{
  // just store sub-document identifier here to manage it later
  if (mySubs.find(theDocID) == mySubs.end())
    mySubs.insert(theDocID);
  return Model_Application::getApplication()->getDocument(theDocID);
}

ObjectPtr Model_Document::object(const std::string& theGroupID, const int theIndex)
{
  if (theGroupID == ModelAPI_Feature::group()) {
    // features may be not in history but in the myObjs, so, iterate all
    int anIndex = 0;
    std::map<std::string, std::vector<ObjectPtr> >::iterator aFind = 
      myObjs.find(ModelAPI_Feature::group());
    if (aFind != myObjs.end()) {
      std::vector<ObjectPtr>::iterator aFIter = aFind->second.begin();
      for(; aFIter != aFind->second.end(); aFIter++) {
        if ((*aFIter)->isInHistory()) {
          if (theIndex == anIndex)
            return *aFIter;
          anIndex++;
        }
      }
    }
  } else {
    // iterate all features in order to find the needed result
    std::map<std::string, std::vector<ObjectPtr> >::iterator aFind = 
      myObjs.find(ModelAPI_Feature::group());
    if (aFind != myObjs.end()) {
      std::vector<ObjectPtr>::iterator aFIter = aFind->second.begin();
      for(int anIndex = 0; aFIter != aFind->second.end(); aFIter++) {
        const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = 
          boost::dynamic_pointer_cast<ModelAPI_Feature>(*aFIter)->results();
        std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
        for(; aRIter != aResults.cend(); aRIter++) {
          if ((*aRIter)->isInHistory() && (*aRIter)->groupName() == theGroupID) {
            if (anIndex == theIndex)
              return *aRIter;
            anIndex++;
          }
        }
      }
    }
  }
  // not found
  return ObjectPtr();
}

int Model_Document::size(const std::string& theGroupID) 
{
  int aResult = 0;
  if (theGroupID == ModelAPI_Feature::group()) {
    // features may be not in history but in the myObjs, so, iterate all
    std::map<std::string, std::vector<ObjectPtr> >::iterator aFind = 
      myObjs.find(ModelAPI_Feature::group());
    if (aFind != myObjs.end()) {
      std::vector<ObjectPtr>::iterator aFIter = aFind->second.begin();
      for(; aFIter != aFind->second.end(); aFIter++) {
        if ((*aFIter)->isInHistory()) {
          aResult++;
        }
      }
    }
  } else {
    // iterate all features in order to find the needed result
    std::map<std::string, std::vector<ObjectPtr> >::iterator aFind = 
      myObjs.find(ModelAPI_Feature::group());
    if (aFind != myObjs.end()) {
      std::vector<ObjectPtr>::iterator aFIter = aFind->second.begin();
      for(; aFIter != aFind->second.end(); aFIter++) {
        const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = 
          boost::dynamic_pointer_cast<ModelAPI_Feature>(*aFIter)->results();
        std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
        for(; aRIter != aResults.cend(); aRIter++) {
          if ((*aRIter)->isInHistory() && (*aRIter)->groupName() == theGroupID) {
            aResult++;
          }
        }
      }
    }
  }
  // group is not found
  return aResult;
}

Model_Document::Model_Document(const std::string theID)
    : myID(theID), myDoc(new TDocStd_Document("BinOcaf")) // binary OCAF format
{
  myDoc->SetUndoLimit(UNDO_LIMIT);
  myTransactionsAfterSave = 0;
  myNestedNum = -1;
  //myDoc->SetNestedTransactionMode();
  // to have something in the document and avoid empty doc open/save problem
  // in transaction for nesting correct working
  myDoc->NewCommand();
  TDataStd_Integer::Set(myDoc->Main().Father(), 0);
  myDoc->CommitCommand();
}

TDF_Label Model_Document::groupLabel(const std::string theGroup)
{
  // searching for existing
  TCollection_ExtendedString aGroup(theGroup.c_str());
  TDF_ChildIDIterator aGroupIter(myDoc->Main().FindChild(TAG_OBJECTS), TDataStd_Comment::GetID());
  for(; aGroupIter.More(); aGroupIter.Next()) {
    Handle(TDataStd_Comment) aName = Handle(TDataStd_Comment)::DownCast(aGroupIter.Value());
    if (aName->Get() == aGroup)
      return aGroupIter.Value()->Label();
  }
  // create a new
  TDF_Label aNew = myDoc->Main().FindChild(TAG_OBJECTS).NewChild();
  TDataStd_Comment::Set(aNew, aGroup);
  return aNew;
}

void Model_Document::setUniqueName(FeaturePtr theFeature)
{
  std::string aName; // result
  // first count all objects of such kind to start with index = count + 1
  int a, aNumObjects = 0;
  int aSize = myObjs.find(ModelAPI_Feature::group()) == myObjs.end() ? 
    0 : myObjs[ModelAPI_Feature::group()].size();
  for(a = 0; a < aSize; a++) {
    if (boost::dynamic_pointer_cast<ModelAPI_Feature>(myObjs[ModelAPI_Feature::group()][a])->
        getKind() == theFeature->getKind())
      aNumObjects++;
  }
  // generate candidate name
  std::stringstream aNameStream;
  aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
  aName = aNameStream.str();
  // check this is unique, if not, increase index by 1
  for(a = 0; a < aSize;) {
    FeaturePtr aFeature = 
      boost::dynamic_pointer_cast<ModelAPI_Feature>(myObjs[ModelAPI_Feature::group()][a]);
    bool isSameName = aFeature->isInHistory() && aFeature->data()->name() == aName;
    if (!isSameName) { // check also results to avoid same results names (actual for Parts)
      const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
      std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for(; aRIter != aResults.cend(); aRIter++) {
        isSameName = (*aRIter)->isInHistory() && (*aRIter)->data()->name() == aName;
      }
    }
    if (isSameName) {
      aNumObjects++;
      std::stringstream aNameStream;
      aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
      aName = aNameStream.str();
      // reinitialize iterator to make sure a new name is unique
      a = 0;
    } else a++;
  }
  theFeature->data()->setName(aName);
}

void Model_Document::initData(ObjectPtr theObj, TDF_Label& theLab, const int theTag) {
  boost::shared_ptr<ModelAPI_Document> aThis = 
    Model_Application::getApplication()->getDocument(myID);
  boost::shared_ptr<Model_Data> aData(new Model_Data);
  aData->setLabel(theLab.FindChild(theTag + 1));
  aData->setObject(theObj);
  theObj->setDoc(aThis);
  theObj->setData(aData);
  FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if (aFeature) aFeature->initAttributes();
}

void Model_Document::synchronizeFeatures(const bool theMarkUpdated)
{
  boost::shared_ptr<ModelAPI_Document> aThis = 
    Model_Application::getApplication()->getDocument(myID);
  // update all objects: iterate from the end: as they appeared in the list
  std::map<std::string, std::vector<ObjectPtr> >::reverse_iterator aGroupIter = myObjs.rbegin();
  for(; aGroupIter != myObjs.rend(); aGroupIter++) {
    std::vector<ObjectPtr>::iterator anObjIter = aGroupIter->second.begin();
    // and in parallel iterate labels of features
    const std::string& aGroupName = aGroupIter->first;
    TDF_ChildIDIterator aLabIter(groupLabel(aGroupName), TDataStd_Comment::GetID());
    while(anObjIter != aGroupIter->second.end() || aLabIter.More()) {
      static const int INFINITE_TAG = INT_MAX; // no label means that it exists somwhere in infinite
      int aFeatureTag = INFINITE_TAG; 
      if (anObjIter != aGroupIter->second.end()) { // existing tag for feature
        boost::shared_ptr<Model_Data> aData = 
          boost::dynamic_pointer_cast<Model_Data>((*anObjIter)->data());
        aFeatureTag = aData->label().Tag();
      }
      int aDSTag = INFINITE_TAG; 
      if (aLabIter.More()) { // next label in DS is existing
        aDSTag = aLabIter.Value()->Label().Tag();
      }
      if (aDSTag > aFeatureTag) { // feature is removed
        ObjectPtr anObj = *anObjIter;
        anObjIter = aGroupIter->second.erase(anObjIter);
        // event: model is updated
        if (anObj->isInHistory()) {
          ModelAPI_EventCreator::get()->sendDeleted(aThis, aGroupName);
        }
        ModelAPI_EventCreator::get()->sendDeleted(aThis, aGroupName);
        // results of this feature must be redisplayed (hided)
        static Events_ID EVENT_DISP = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
        const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = boost::dynamic_pointer_cast<ModelAPI_Feature>(anObj)->results();
        std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
        for(; aRIter != aResults.cend(); aRIter++) {
          boost::shared_ptr<ModelAPI_Result> aRes = *aRIter;
          aRes->setData(boost::shared_ptr<ModelAPI_Data>()); // deleted flag
          ModelAPI_EventCreator::get()->sendUpdated(aRes, EVENT_DISP);
        }
      } else if (aDSTag < aFeatureTag) { // a new feature is inserted
        // create a feature
        TDF_Label aLab = aLabIter.Value()->Label();
        ObjectPtr aNewObj = ModelAPI_PluginManager::get()->createFeature(
          TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(aLabIter.Value())->Get())
          .ToCString());
        // this must be before "setData" to redo the sketch line correctly
        if (anObjIter == aGroupIter->second.end()) {
          aGroupIter->second.push_back(aNewObj);
          anObjIter = aGroupIter->second.end();
        } else {
          anObjIter++;
          aGroupIter->second.insert(anObjIter, aNewObj);
        }
        initData(aNewObj, aLab, TAG_FEATURE_ARGUMENTS);

        // event: model is updated
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
        ModelAPI_EventCreator::get()->sendUpdated(aNewObj, anEvent);
        // feature for this label is added, so go to the next label
        aLabIter.Next();
      } else { // nothing is changed, both iterators are incremented
        if (theMarkUpdated) {
          static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
          ModelAPI_EventCreator::get()->sendUpdated(*anObjIter, anEvent);
        }
        anObjIter++;
        aLabIter.Next();
      }
    }
  }
  // after all updates, sends a message that groups of features were created or updated
  boost::static_pointer_cast<Model_PluginManager>(Model_PluginManager::get())->
    setCheckTransactions(false);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  if (theMarkUpdated)
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  boost::static_pointer_cast<Model_PluginManager>(Model_PluginManager::get())->
    setCheckTransactions(true);
}

void Model_Document::storeResult(boost::shared_ptr<ModelAPI_Data> theFeatureData,
  boost::shared_ptr<ModelAPI_Result> theResult, const int theResultIndex)
{
  boost::shared_ptr<ModelAPI_Document> aThis = 
    Model_Application::getApplication()->getDocument(myID);
  theResult->setDoc(aThis);
  initData(theResult, boost::dynamic_pointer_cast<Model_Data>(theFeatureData)->
    label().Father().FindChild(TAG_FEATURE_RESULTS), theResultIndex);
  if (theResult->data()->name().empty()) { // if was not initialized, generate event and set a name
    static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
    ModelAPI_EventCreator::get()->sendUpdated(theResult, anEvent);
    theResult->data()->setName(theFeatureData->name());
  }
}

boost::shared_ptr<ModelAPI_ResultConstruction> Model_Document::createConstruction(
  const boost::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  ObjectPtr anOldObject = object(boost::dynamic_pointer_cast<Model_Data>(theFeatureData)->
    label().Father().FindChild(TAG_FEATURE_RESULTS).FindChild(theIndex + 1));
  boost::shared_ptr<ModelAPI_ResultConstruction> aResult;
  if (anOldObject) {
    aResult = boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(anOldObject);
  }
  if (!aResult) {
    aResult = boost::shared_ptr<ModelAPI_ResultConstruction>(new Model_ResultConstruction);
    storeResult(theFeatureData, aResult);
  }
  return aResult;
}

boost::shared_ptr<ModelAPI_ResultBody> Model_Document::createBody(
  const boost::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  ObjectPtr anOldObject = object(boost::dynamic_pointer_cast<Model_Data>(theFeatureData)->
    label().Father().FindChild(TAG_FEATURE_RESULTS).FindChild(theIndex + 1));
  boost::shared_ptr<ModelAPI_ResultBody> aResult;
  if (anOldObject) {
    aResult = boost::dynamic_pointer_cast<ModelAPI_ResultBody>(anOldObject);
  }
  if (!aResult) {
    aResult = boost::shared_ptr<ModelAPI_ResultBody>(new Model_ResultBody);
    storeResult(theFeatureData, aResult);
  }
  return aResult;
}

boost::shared_ptr<ModelAPI_ResultPart> Model_Document::createPart(
  const boost::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  ObjectPtr anOldObject = object(boost::dynamic_pointer_cast<Model_Data>(theFeatureData)->
    label().Father().FindChild(TAG_FEATURE_RESULTS).FindChild(theIndex + 1));
  boost::shared_ptr<ModelAPI_ResultPart> aResult;
  if (anOldObject) {
    aResult = boost::dynamic_pointer_cast<ModelAPI_ResultPart>(anOldObject);
  }
  if (!aResult) {
    aResult = boost::shared_ptr<ModelAPI_ResultPart>(new Model_ResultPart);
    storeResult(theFeatureData, aResult);
  }
  return aResult;
}

boost::shared_ptr<ModelAPI_Feature> Model_Document::feature(
  const boost::shared_ptr<ModelAPI_Result>& theResult)
{
  // iterate all features in order to find the needed result
  std::map<std::string, std::vector<ObjectPtr> >::iterator aFind = 
    myObjs.find(ModelAPI_Feature::group());
  if (aFind != myObjs.end()) {
    std::vector<ObjectPtr>::iterator aFIter = aFind->second.begin();
    for(; aFIter != aFind->second.end(); aFIter++) {
      FeaturePtr aFeature = boost::dynamic_pointer_cast<ModelAPI_Feature>(*aFIter);
      const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
      std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for(; aRIter != aResults.cend(); aRIter++) {
        if (*aRIter == theResult) {
          return aFeature;
        }
      }
    }
  }
  return FeaturePtr();
}
