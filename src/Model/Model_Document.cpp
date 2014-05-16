// File:        Model_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#include <Model_Document.h>
#include <ModelAPI_Feature.h>
#include <Model_Data.h>
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

#include <climits>

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
  // close all subs
  set<string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->close();
  mySubs.clear();
  // close this
  myDoc->Close();
  Model_Application::getApplication()->deleteDocument(myID);
}

void Model_Document::startOperation()
{
  // check is it nested or not
  if (myDoc->HasOpenCommand()) {
    myNestedStart = myTransactionsAfterSave;
  }
  // new command for this
  myDoc->NewCommand();
  // new command for all subs
  set<string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->startOperation();
}

void Model_Document::finishOperation()
{
  if (myNestedStart > myTransactionsAfterSave) // this nested transaction is owervritten
    myNestedStart = 0;
  // returns false if delta is empty and no transaction was made
  myIsEmptyTr[myTransactionsAfterSave] = !myDoc->CommitCommand();
  myTransactionsAfterSave++;
  // finish for all subs
  set<string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->finishOperation();
}

void Model_Document::abortOperation()
{
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
  return myDoc->HasOpenCommand() == Standard_True ;
}

bool Model_Document::isModified()
{
  // is modified if at least one operation was commited and not undoed
  return myTransactionsAfterSave > 0;
}

bool Model_Document::canUndo()
{
  if (myDoc->GetAvailableUndos() > 0 && myNestedStart != myTransactionsAfterSave)
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

void Model_Document::addFeature(const boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  const std::string& aGroup = theFeature->getGroup();
  TDF_Label aGroupLab = groupLabel(aGroup);
  TDF_Label anObjLab = aGroupLab.NewChild();
  boost::shared_ptr<Model_Data> aData(new Model_Data);
  aData->setFeature(theFeature);
  aData->setLabel(anObjLab);
  boost::shared_ptr<ModelAPI_Document> aThis = 
    Model_Application::getApplication()->getDocument(myID);
  theFeature->setDoc(aThis);
  theFeature->setData(aData);
  setUniqueName(theFeature);
  theFeature->initAttributes();
  // keep the feature ID to restore document later correctly
  TDataStd_Comment::Set(anObjLab, theFeature->getKind().c_str());
  // put index of the feature in the group in the tree
  TDataStd_Integer::Set(anObjLab, myFeatures[aGroup].size());
  myFeatures[aGroup].push_back(theFeature);
  // store feature in the history of features array
  if (theFeature->isInHistory()) {
    Handle(TDataStd_ReferenceArray) aRefs;
    if (!groupLabel(FEATURES_GROUP).FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
      aRefs = TDataStd_ReferenceArray::Set(groupLabel(FEATURES_GROUP), 0, 0);
      aRefs->SetValue(0, anObjLab);
    } else { // extend array by one more element
      Handle(TDataStd_HLabelArray1) aNewArray = 
        new TDataStd_HLabelArray1(aRefs->Lower(), aRefs->Upper() + 1);
      for(int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
        aNewArray->SetValue(a, aRefs->Value(a));
      }
      aNewArray->SetValue(aRefs->Upper() + 1, anObjLab);
      aRefs->SetInternalArray(aNewArray);
    }
  }

  // event: feature is added
  static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_CREATED);
  Model_FeatureUpdatedMessage aMsg(theFeature, anEvent);
  Events_Loop::loop()->send(aMsg);
}

boost::shared_ptr<ModelAPI_Feature> Model_Document::feature(TDF_Label& theLabel)
{
  Handle(TDataStd_Integer) aFeatureIndex;
  if (theLabel.FindAttribute(TDataStd_Integer::GetID(), aFeatureIndex)) {
    Handle(TDataStd_Comment) aGroupID;
    if (theLabel.Father().FindAttribute(TDataStd_Comment::GetID(), aGroupID)) {
      string aGroup = TCollection_AsciiString(aGroupID->Get()).ToCString();
      if (myFeatures[aGroup].size() > aFeatureIndex->Get())
        return myFeatures[aGroup][aFeatureIndex->Get()];
    }
  }
  return boost::shared_ptr<ModelAPI_Feature>(); // not found
}

int Model_Document::featureIndex(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (theFeature->document().get() != this) {
    return theFeature->document()->featureIndex(theFeature);
  }
  boost::shared_ptr<Model_Data> aData =
    boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
  Handle(TDataStd_Integer) aFeatureIndex;
  if (aData->label().FindAttribute(TDataStd_Integer::GetID(), aFeatureIndex)) {
    return aFeatureIndex->Get();
  }
  return -1; // not found
}

boost::shared_ptr<ModelAPI_Document> Model_Document::subDocument(string theDocID)
{
  // just store sub-document identifier here to manage it later
  if (mySubs.find(theDocID) == mySubs.end())
    mySubs.insert(theDocID);
  return Model_Application::getApplication()->getDocument(theDocID);
}

boost::shared_ptr<ModelAPI_Feature> Model_Document::feature(
  const string& theGroupID, const int theIndex)
{
  if (theGroupID == FEATURES_GROUP) { // history is just a references array
    Handle(TDataStd_ReferenceArray) aRefs;
    if (groupLabel(FEATURES_GROUP).FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
      if (aRefs->Lower() <= theIndex && aRefs->Upper() >= theIndex) {
        TDF_Label aFeatureLab = aRefs->Value(theIndex);
        return feature(aFeatureLab);
      }
    }
  } else { // one of the group
    map<string, vector<boost::shared_ptr<ModelAPI_Feature> > >::iterator aGroup = 
      myFeatures.find(theGroupID);
    if (aGroup != myFeatures.end() && (int)(aGroup->second.size()) > theIndex) {
      return aGroup->second[theIndex];
    }
  }
  // not found
  return boost::shared_ptr<ModelAPI_Feature>();
}

int Model_Document::size(const string& theGroupID) 
{
  if (theGroupID == FEATURES_GROUP) { // history is just a references array
    Handle(TDataStd_ReferenceArray) aRefs;
    if (groupLabel(FEATURES_GROUP).FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
      return aRefs->Length();
  } else { // one of the group
    map<string, vector<boost::shared_ptr<ModelAPI_Feature> > >::iterator aGroup = 
      myFeatures.find(theGroupID);
    if (aGroup != myFeatures.end())
      return aGroup->second.size();
  }
  // group is not found
  return 0;
}

const vector<string>& Model_Document::getGroups() const
{
  return myGroupsNames;
}

Model_Document::Model_Document(const std::string theID)
    : myID(theID), myDoc(new TDocStd_Document("BinOcaf")) // binary OCAF format
{
  myDoc->SetUndoLimit(UNDO_LIMIT);
  myTransactionsAfterSave = 0;
  myNestedStart = 0;
  myDoc->SetNestedTransactionMode();
  // to have something in the document and avoid empty doc open/save problem
  TDataStd_Integer::Set(myDoc->Main().Father(), 0);
}

TDF_Label Model_Document::groupLabel(const string theGroup)
{
  if (myGroups.find(theGroup) == myGroups.end()) {
    myGroups[theGroup] = myDoc->Main().FindChild(TAG_OBJECTS).NewChild();
    myGroupsNames.push_back(theGroup);
    // set to the group label the group idntifier to restore on "open"
    TDataStd_Comment::Set(myGroups[theGroup], theGroup.c_str());
    myFeatures[theGroup] = vector<boost::shared_ptr<ModelAPI_Feature> >();
  }
  return myGroups[theGroup];
}

void Model_Document::setUniqueName(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  // first count all objects of such kind to start with index = count + 1
  int a, aNumObjects = 0;
  int aSize = size(theFeature->getGroup());
  for(a = 0; a < aSize; a++) {
    if (feature(theFeature->getGroup(), a)->getKind() == theFeature->getKind())
      aNumObjects++;
  }
  // generate candidate name
  stringstream aNameStream;
  aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
  string aName = aNameStream.str();
  // check this is unique, if not, increase index by 1
  for(a = 0; a < aSize;) {
    if (feature(theFeature->getGroup(), a)->data()->getName() == aName) {
      aNumObjects++;
      stringstream aNameStream;
      aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
      // reinitialize iterator to make sure a new name is unique
      a = 0;
    } else a++;
  }

  theFeature->data()->setName(aName);
}

void Model_Document::synchronizeFeatures()
{
  boost::shared_ptr<ModelAPI_Document> aThis = Model_Application::getApplication()->getDocument(myID);
  // iterate groups labels
  TDF_ChildIDIterator aGroupsIter(myDoc->Main().FindChild(TAG_OBJECTS),
    TDataStd_Comment::GetID(), Standard_False);
  vector<string>::iterator aGroupNamesIter = myGroupsNames.begin();
  for(; aGroupsIter.More() && aGroupNamesIter != myGroupsNames.end();
        aGroupsIter.Next(), aGroupNamesIter++) {
    string aGroupName = TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(
      aGroupsIter.Value())->Get()).ToCString();
    if (*aGroupNamesIter != aGroupName) 
      break; // all since there is a change this must be recreated from scratch
  }
  // delete all groups left after the data model groups iteration
  while(aGroupNamesIter != myGroupsNames.end()) {
    string aGroupName = *aGroupNamesIter;
    myFeatures.erase(aGroupName);
    myGroups.erase(aGroupName);
    aGroupNamesIter = myGroupsNames.erase(aGroupNamesIter);
    // say that features were deleted from group
    Model_FeatureDeletedMessage aMsg(aThis, aGroupName);
    Events_Loop::loop()->send(aMsg);
  }
  // create new groups basing on the following data model update
  for(; aGroupsIter.More(); aGroupsIter.Next()) {
    string aGroupName = TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(
      aGroupsIter.Value())->Get()).ToCString();
    myGroupsNames.push_back(aGroupName);
    myGroups[aGroupName] = aGroupsIter.Value()->Label();
    myFeatures[aGroupName] = vector<boost::shared_ptr<ModelAPI_Feature> >();
  }
  // update features group by group
  aGroupsIter.Initialize(myDoc->Main().FindChild(TAG_OBJECTS),
    TDataStd_Comment::GetID(), Standard_False);
  for(; aGroupsIter.More(); aGroupsIter.Next()) {
    string aGroupName = TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(
      aGroupsIter.Value())->Get()).ToCString();
    // iterate features in internal container
    vector<boost::shared_ptr<ModelAPI_Feature> >& aFeatures = myFeatures[aGroupName];
    vector<boost::shared_ptr<ModelAPI_Feature> >::iterator aFIter = aFeatures.begin();
    // and in parallel iterate labels of features
    TDF_ChildIDIterator aFLabIter(
      aGroupsIter.Value()->Label(), TDataStd_Comment::GetID(), Standard_False);
    while(aFIter != aFeatures.end() || aFLabIter.More()) {
      static const int INFINITE_TAG = INT_MAX; // no label means that it exists somwhere in infinite
      int aFeatureTag = INFINITE_TAG; 
      if (aFIter != aFeatures.end()) { // existing tag for feature
        boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>((*aFIter)->data());
        aFeatureTag = aData->label().Tag();
      }
      int aDSTag = INFINITE_TAG; 
      if (aFLabIter.More()) { // next label in DS is existing
        aDSTag = aFLabIter.Value()->Label().Tag();
      }
      if (aDSTag > aFeatureTag) { // feature is removed
        aFIter = aFeatures.erase(aFIter);
        // event: model is updated
        Model_FeatureDeletedMessage aMsg(aThis, aGroupName);
        Events_Loop::loop()->send(aMsg);
      } else if (aDSTag < aFeatureTag) { // a new feature is inserted
        // create a feature
        boost::shared_ptr<ModelAPI_Feature> aFeature = ModelAPI_PluginManager::get()->createFeature(
          TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(
          aFLabIter.Value())->Get()).ToCString());

        if (aFIter == aFeatures.end()) { // must be before "setData" to redo the sketch line correctly
          aFeatures.push_back(aFeature);
          aFIter = aFeatures.end();
        } else {
          aFIter++;
          aFeatures.insert(aFIter, aFeature);
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
        Model_FeatureUpdatedMessage aMsg(aFeature, anEvent);
        Events_Loop::loop()->send(aMsg);

        // feature for this label is added, so go to the next label
        aFLabIter.Next();
      } else { // nothing is changed, both iterators are incremented
        aFIter++;
        aFLabIter.Next();
      }
    }
  }
}
