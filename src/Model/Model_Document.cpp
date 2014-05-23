// File:        Model_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#include <Model_Document.h>
#include <ModelAPI_Feature.h>
#include <Model_Data.h>
#include <Model_Object.h>
#include <Model_Application.h>
#include <Model_PluginManager.h>
#include <Model_Events.h>
#include <Events_Loop.h>
#include <Events_Error.h>

#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDataStd_ReferenceArray.hxx>
#include <TDataStd_HLabelArray1.hxx>
#include <TDataStd_Name.hxx>

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
static const int TAG_OBJECTS = 2; // tag of the objects sub-tree (features)
static const int TAG_HISTORY = 3; // tag of the history sub-tree (python dump)

using namespace std;

/// Returns the file name of this document by the nameof directory and identifuer of a document
static TCollection_ExtendedString DocFileName(const char* theFileName, const string& theID)
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
    Events_Error::send(string("Exception in opening of document: ") + aFail->GetMessageString());
    return false;
  }
  bool isError = aStatus != PCDM_RS_OK;
  if (isError)
  {
    switch (aStatus)
    {
    case PCDM_RS_UnknownDocument: 
      Events_Error::send(string("Can not open document: PCDM_RS_UnknownDocument")); break;
    case PCDM_RS_AlreadyRetrieved: 
      Events_Error::send(string("Can not open document: PCDM_RS_AlreadyRetrieved")); break;
    case PCDM_RS_AlreadyRetrievedAndModified:
      Events_Error::send(string("Can not open document: PCDM_RS_AlreadyRetrievedAndModified")); break;
    case PCDM_RS_NoDriver:
      Events_Error::send(string("Can not open document: PCDM_RS_NoDriver")); break;
    case PCDM_RS_UnknownFileDriver:
      Events_Error::send(string("Can not open document: PCDM_RS_UnknownFileDriver")); break;
    case PCDM_RS_OpenError:
      Events_Error::send(string("Can not open document: PCDM_RS_OpenError")); break;
    case PCDM_RS_NoVersion:
      Events_Error::send(string("Can not open document: PCDM_RS_NoVersion")); break;
    case PCDM_RS_NoModel:
      Events_Error::send(string("Can not open document: PCDM_RS_NoModel")); break;
    case PCDM_RS_NoDocument:
      Events_Error::send(string("Can not open document: PCDM_RS_NoDocument")); break;
    case PCDM_RS_FormatFailure:
      Events_Error::send(string("Can not open document: PCDM_RS_FormatFailure")); break;
    case PCDM_RS_TypeNotFoundInSchema:
      Events_Error::send(string("Can not open document: PCDM_RS_TypeNotFoundInSchema")); break;
    case PCDM_RS_UnrecognizedFileFormat:
      Events_Error::send(string("Can not open document: PCDM_RS_UnrecognizedFileFormat")); break;
    case PCDM_RS_MakeFailure:
      Events_Error::send(string("Can not open document: PCDM_RS_MakeFailure")); break;
    case PCDM_RS_PermissionDenied:
      Events_Error::send(string("Can not open document: PCDM_RS_PermissionDenied")); break;
    case PCDM_RS_DriverFailure:
      Events_Error::send(string("Can not open document: PCDM_RS_DriverFailure")); break;
    default:
      Events_Error::send(string("Can not open document: unknown error")); break;
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
    Events_Error::send(string("Exception in saving of document: ") + aFail->GetMessageString());
    return false;
  }
  bool isDone = aStatus == PCDM_SS_OK || aStatus == PCDM_SS_No_Obj;
  if (!isDone)
  {
    switch (aStatus)
    {
    case PCDM_SS_DriverFailure:
      Events_Error::send(string("Can not save document: PCDM_SS_DriverFailure"));
      break;
    case PCDM_SS_WriteFailure:
      Events_Error::send(string("Can not save document: PCDM_SS_WriteFailure"));
      break;
    case PCDM_SS_Failure:
    default:
      Events_Error::send(string("Can not save document: PCDM_SS_Failure"));
      break;
    }
  }
  myTransactionsAfterSave = 0;
  if (isDone) { // save also sub-documents if any
    set<string>::iterator aSubIter = mySubs.begin();
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
  set<string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->close();
  mySubs.clear();
  // close this
  if (myDoc->CanClose() == CDM_CCS_OK)
    myDoc->Close();
  Model_Application::getApplication()->deleteDocument(myID);
}

void Model_Document::startOperation()
{
  if (myDoc->HasOpenCommand()) { // start of nested command
    if (myNestedNum == -1) {
      myNestedNum = 0;
      myDoc->InitDeltaCompaction();
    }
    myDoc->NewCommand();
  } else { // start of simple command
    myDoc->NewCommand();
  }
  // new command for all subs
  set<string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->startOperation();
}

void Model_Document::finishOperation()
{
  // just to be sure that everybody knows that changes were performed
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_CREATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_DELETED));

  if (myNestedNum != -1) // this nested transaction is owervritten
    myNestedNum++;
  if (!myDoc->HasOpenCommand()) {
    if (myNestedNum != -1) {
      myNestedNum -= 2; // one is just incremented before, one is left (and not empty!)
      while(myNestedNum != -1) {
        myIsEmptyTr.erase(myTransactionsAfterSave);
        myTransactionsAfterSave--;
        myNestedNum--;
      }
      myIsEmptyTr[myTransactionsAfterSave] = false;
      myTransactionsAfterSave++;
      myDoc->PerformDeltaCompaction();
    }
  } else {
    // returns false if delta is empty and no transaction was made
    myIsEmptyTr[myTransactionsAfterSave] = !myDoc->CommitCommand() && (myNestedNum == -1);
    myTransactionsAfterSave++;
  }

  // finish for all subs
  set<string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->finishOperation();
}

void Model_Document::abortOperation()
{
  if (myNestedNum == 0)
    myNestedNum = -1;
  myDoc->AbortCommand();
  synchronizeFeatures();
  // abort for all subs
  set<string>::iterator aSubIter = mySubs.begin();
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
  set<string>::iterator aSubIter = mySubs.begin();
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
  synchronizeFeatures();
  // undo for all subs
  set<string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->undo();
}

bool Model_Document::canRedo()
{
  if (myDoc->GetAvailableRedos() > 0)
    return true;
  // check other subs contains operation that can be redoed
  set<string>::iterator aSubIter = mySubs.begin();
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
  synchronizeFeatures();
  // redo for all subs
  set<string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->redo();
}

boost::shared_ptr<ModelAPI_Feature> Model_Document::addFeature(string theID)
{
  boost::shared_ptr<ModelAPI_Feature> aFeature = 
    ModelAPI_PluginManager::get()->createFeature(theID);
  if (aFeature) {
    boost::dynamic_pointer_cast<Model_Document>(aFeature->documentToAdd())->addFeature(aFeature);
  } else {
    // TODO: generate error that feature is not created
  }
  return aFeature;
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

void Model_Document::addFeature(const boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (theFeature->isAction()) return; // do not add action to the data model

  boost::shared_ptr<ModelAPI_Document> aThis = 
    Model_Application::getApplication()->getDocument(myID);
  TDF_Label aFeaturesLab = groupLabel(FEATURES_GROUP);
  TDF_Label aFeatureLab = aFeaturesLab.NewChild();

  // organize feature and data objects
  boost::shared_ptr<Model_Data> aData(new Model_Data);
  aData->setFeature(theFeature);
  aData->setLabel(aFeatureLab);
  theFeature->setDoc(aThis);
  theFeature->setData(aData);
  setUniqueName(theFeature);
  theFeature->initAttributes();

  // keep the feature ID to restore document later correctly
  TDataStd_Comment::Set(aFeatureLab, theFeature->getKind().c_str());
  myFeatures.push_back(theFeature);
  // store feature in the history of features array
  if (theFeature->isInHistory()) {
    AddToRefArray(aFeaturesLab, aFeatureLab);
  }
  // add featue to the group
  const std::string& aGroup = theFeature->getGroup();
  TDF_Label aGroupLab = groupLabel(aGroup);
  AddToRefArray(aGroupLab, aFeatureLab);
  // new name of this feature object by default equal to name of feature
  TDF_Label anObjLab = aGroupLab.NewChild();
  TCollection_ExtendedString aName(theFeature->data()->getName().c_str());
  TDataStd_Name::Set(anObjLab, aName);
  TDF_Label aGrLabChild = aGroupLab.FindChild(1);
  AddToRefArray(aGrLabChild, anObjLab); // reference to names is on the first sub

  // event: feature is added
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_CREATED);
  Model_FeatureUpdatedMessage aMsg(theFeature, anEvent);
  Events_Loop::loop()->send(aMsg);
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

void Model_Document::removeFeature(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  boost::shared_ptr<Model_Data> aData = boost::static_pointer_cast<Model_Data>(theFeature->data());
  TDF_Label aFeatureLabel = aData->label();
  // remove the object
  TDF_Label aGroupLabel = groupLabel(theFeature->getGroup());
  int aRemovedIndex = RemoveFromRefArray(aGroupLabel, aFeatureLabel);
  RemoveFromRefArray(aGroupLabel.FindChild(1), TDF_Label(), aRemovedIndex);
  // remove feature from the myFeatures list
  std::vector<boost::shared_ptr<ModelAPI_Feature> >::iterator aFIter = myFeatures.begin();
  while(aFIter != myFeatures.end()) {
    if (*aFIter == theFeature) {
      aFIter = myFeatures.erase(aFIter);
    } else {
      aFIter++;
    }
  }
  // erase all attributes under the label of feature
  aFeatureLabel.ForgetAllAttributes();
  // remove it from the references array
  RemoveFromRefArray(groupLabel(FEATURES_GROUP), aData->label());

  // event: feature is added
  Model_FeatureDeletedMessage aMsg(theFeature->document(), theFeature->getGroup());
  Events_Loop::loop()->send(aMsg);
}

boost::shared_ptr<ModelAPI_Feature> Model_Document::feature(TDF_Label& theLabel)
{
  // iterate all features, may be optimized later by keeping labels-map
  vector<boost::shared_ptr<ModelAPI_Feature> >::iterator aFIter = myFeatures.begin();
  for(; aFIter != myFeatures.end(); aFIter++) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>((*aFIter)->data());
    if (aData->label().IsEqual(theLabel))
      return *aFIter;
  }
  return boost::shared_ptr<ModelAPI_Feature>(); // not found
}

boost::shared_ptr<ModelAPI_Document> Model_Document::subDocument(string theDocID)
{
  // just store sub-document identifier here to manage it later
  if (mySubs.find(theDocID) == mySubs.end())
    mySubs.insert(theDocID);
  return Model_Application::getApplication()->getDocument(theDocID);
}

boost::shared_ptr<ModelAPI_Feature> Model_Document::feature(
  const string& theGroupID, const int theIndex, const bool isOperation)
{
  TDF_Label aGroupLab = groupLabel(theGroupID);
  Handle(TDataStd_ReferenceArray) aRefs;
  if (aGroupLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    if (aRefs->Lower() <= theIndex && aRefs->Upper() >= theIndex) {
      TDF_Label aFeatureLab = aRefs->Value(theIndex);
      boost::shared_ptr<ModelAPI_Feature> aFeature = feature(aFeatureLab);

      if (theGroupID == FEATURES_GROUP || isOperation) { // just returns the feature from the history
        return aFeature;
      } else { // create a new object from the group to return it
        Handle(TDataStd_Name) aName; // name of the object
        if (aGroupLab.FindChild(1).FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
          aRefs->Value(theIndex).FindAttribute(TDataStd_Name::GetID(), aName);
        boost::shared_ptr<Model_Object> anObj(new Model_Object(aFeature, aName));
        return anObj;
      }
    }
  }

  // not found
  return boost::shared_ptr<ModelAPI_Feature>();
}

int Model_Document::size(const string& theGroupID) 
{
  Handle(TDataStd_ReferenceArray) aRefs;
  if (groupLabel(theGroupID).FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
    return aRefs->Length();
  // group is not found
  return 0;
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

TDF_Label Model_Document::groupLabel(const string theGroup)
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

void Model_Document::setUniqueName(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  string aName; // result
  // iterate all features but also iterate group of this feature if object is not in history
  list<string> aGroups;
  aGroups.push_back(FEATURES_GROUP);
  if (!theFeature->isInHistory()) {
    aGroups.push_back(theFeature->getGroup());
  }
  for(list<string>::iterator aGIter = aGroups.begin(); aGIter != aGroups.end(); aGIter++) {
    // first count all objects of such kind to start with index = count + 1
    int a, aNumObjects = 0;
    int aSize = size(*aGIter);
    for(a = 0; a < aSize; a++) {
      if (feature(*aGIter, a)->getKind() == theFeature->getKind())
        aNumObjects++;
    }
    // generate candidate name
    stringstream aNameStream;
    aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
    aName = aNameStream.str();
    // check this is unique, if not, increase index by 1
    for(a = 0; a < aSize;) {
      if (feature(*aGIter, a, true)->data()->getName() == aName) {
        aNumObjects++;
        stringstream aNameStream;
        aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
        aName = aNameStream.str();
        // reinitialize iterator to make sure a new name is unique
        a = 0;
      } else a++;
    }
  }
  theFeature->data()->setName(aName);
}

//! Returns the object by the feature
boost::shared_ptr<ModelAPI_Feature> Model_Document::objectByFeature(
  const boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  for(int a = 0; a < size(theFeature->getGroup()); a++) {
    boost::shared_ptr<Model_Object> anObj = 
      boost::dynamic_pointer_cast<Model_Object>(feature(theFeature->getGroup(), a));
    if (anObj) {
      return anObj;
    }
  }
  return boost::shared_ptr<ModelAPI_Feature>(); // not found
}

void Model_Document::synchronizeFeatures()
{
  boost::shared_ptr<ModelAPI_Document> aThis = Model_Application::getApplication()->getDocument(myID);
  // update features
  vector<boost::shared_ptr<ModelAPI_Feature> >::iterator aFIter = myFeatures.begin();
  // and in parallel iterate labels of features
  TDF_ChildIDIterator aFLabIter(groupLabel(FEATURES_GROUP), TDataStd_Comment::GetID());
  while(aFIter != myFeatures.end() || aFLabIter.More()) {
    static const int INFINITE_TAG = INT_MAX; // no label means that it exists somwhere in infinite
    int aFeatureTag = INFINITE_TAG; 
    if (aFIter != myFeatures.end()) { // existing tag for feature
      boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>((*aFIter)->data());
      aFeatureTag = aData->label().Tag();
    }
    int aDSTag = INFINITE_TAG; 
    if (aFLabIter.More()) { // next label in DS is existing
      aDSTag = aFLabIter.Value()->Label().Tag();
    }
    if (aDSTag > aFeatureTag) { // feature is removed
      Model_FeatureDeletedMessage aMsg1(aThis, FEATURES_GROUP);
      Model_FeatureDeletedMessage aMsg2(aThis, (*aFIter)->getGroup());
      aFIter = myFeatures.erase(aFIter);
      // event: model is updated
      Events_Loop::loop()->send(aMsg1);
      Events_Loop::loop()->send(aMsg2);
    } else if (aDSTag < aFeatureTag) { // a new feature is inserted
      // create a feature
      boost::shared_ptr<ModelAPI_Feature> aFeature = ModelAPI_PluginManager::get()->createFeature(
        TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(
        aFLabIter.Value())->Get()).ToCString());

      if (aFIter == myFeatures.end()) { // must be before "setData" to redo the sketch line correctly
        myFeatures.push_back(aFeature);
        aFIter = myFeatures.end();
      } else {
        aFIter++;
        myFeatures.insert(aFIter, aFeature);
      }
      boost::shared_ptr<Model_Data> aData(new Model_Data);
      TDF_Label aLab = aFLabIter.Value()->Label();
      aData->setLabel(aLab);
      aData->setFeature(aFeature);
      aFeature->setDoc(aThis);
      aFeature->setData(aData);
      aFeature->initAttributes();

      // event: model is updated
      static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_CREATED);
      Model_FeatureUpdatedMessage aMsg1(aFeature, anEvent);
      Events_Loop::loop()->send(aMsg1);
      boost::shared_ptr<ModelAPI_Feature> anObj = objectByFeature(aFeature);
      if (anObj) {
        Model_FeatureUpdatedMessage aMsg2(anObj, anEvent);
        Events_Loop::loop()->send(aMsg2);
      }

      // feature for this label is added, so go to the next label
      aFLabIter.Next();
    } else { // nothing is changed, both iterators are incremented
      aFIter++;
      aFLabIter.Next();
    }
  }
  // after all updates, sends a message that groups of features were created or updated
  boost::static_pointer_cast<Model_PluginManager>(Model_PluginManager::get())->
    setCheckTransactions(false);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_CREATED));
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_DELETED));
  boost::static_pointer_cast<Model_PluginManager>(Model_PluginManager::get())->
    setCheckTransactions(true);
}
