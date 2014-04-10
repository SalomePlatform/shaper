// File:        Model_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#include <Model_Document.h>
#include <ModelAPI_Feature.h>
#include <Model_Data.h>
#include <Model_Application.h>
#include <Model_PluginManager.h>
#include <Model_Iterator.h>
#include <Model_Events.h>
#include <Event_Loop.h>

#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>
#include <TDF_ChildIDIterator.hxx>

static const int UNDO_LIMIT = 10; // number of possible undo operations

static const int TAG_GENERAL = 1; // general properties tag
static const int TAG_OBJECTS = 2; // tag of the objects sub-tree (Root for Model_DatasMgr)
static const int TAG_HISTORY = 3; // tag of the history sub-tree (Root for Model_History)

using namespace std;

bool Model_Document::load(const char* theFileName)
{
  bool myIsError = Standard_False;
  /*
   TCollection_ExtendedString aPath ((const Standard_CString)theFileName);
   PCDM_ReaderStatus aStatus = (PCDM_ReaderStatus) -1;
   try
   {
   Handle(TDocStd_Document) aDoc = this;
   aStatus = Model_Application::GetApplication()->Open(aPath, aDoc);
   }
   catch (Standard_Failure)
   {}
   myIsError = aStatus != PCDM_RS_OK;
   if (myIsError)
   {
   switch (aStatus)
   {
   case PCDM_RS_UnknownDocument: cout<<"OCAFApp_Appl_RUnknownDocument"<<endl; break;
   case PCDM_RS_AlreadyRetrieved: cout<<"OCAFApp_Appl_RAlreadyRetrieved"<<endl; break;
   case PCDM_RS_AlreadyRetrievedAndModified: cout<<"OCAFApp_Appl_RAlreadyRetrievedAndModified"<<endl; break;
   case PCDM_RS_NoDriver: cout<<"OCAFApp_Appl_RNoDriver"<<endl; break;
   case PCDM_RS_UnknownFileDriver: cout<<"OCAFApp_Appl_RNoDriver"<<endl; break;
   case PCDM_RS_OpenError: cout<<"OCAFApp_Appl_ROpenError"<<endl; break;
   case PCDM_RS_NoVersion: cout<<"OCAFApp_Appl_RNoVersion"<<endl; break;
   case PCDM_RS_NoModel: cout<<"OCAFApp_Appl_RNoModel"<<endl; break;
   case PCDM_RS_NoDocument: cout<<"OCAFApp_Appl_RNoDocument"<<endl; break;
   case PCDM_RS_FormatFailure: cout<<"OCAFApp_Appl_RFormatFailure"<<endl; break;
   case PCDM_RS_TypeNotFoundInSchema: cout<<"OCAFApp_Appl_RTypeNotFound"<<endl; break;
   case PCDM_RS_UnrecognizedFileFormat: cout<<"OCAFApp_Appl_RBadFileFormat"<<endl; break;
   case PCDM_RS_MakeFailure: cout<<"OCAFApp_Appl_RMakeFailure"<<endl; break;
   case PCDM_RS_PermissionDenied: cout<<"OCAFApp_Appl_RPermissionDenied"<<endl; break;
   case PCDM_RS_DriverFailure: cout<<"OCAFApp_Appl_RDriverFailure"<<endl; break;
   default: cout<<"OCAFApp_Appl_RUnknownFail"<<endl; break;
   }
   }
   SetUndoLimit(UNDO_LIMIT);
   */
  return !myIsError;
}

bool Model_Document::save(const char* theFileName)
{
  bool myIsError = true;
  /*
   TCollection_ExtendedString aPath ((const Standard_CString)theFileName);
   PCDM_StoreStatus aStatus;
   try {
   Handle(TDocStd_Document) aDoc = this;
   aStatus = Model_Application::GetApplication()->SaveAs (aDoc, aPath);
   }
   catch (Standard_Failure) {
   Handle(Standard_Failure) aFail = Standard_Failure::Caught();
   cout<<"OCAFApp_Engine:save Error: "<<aFail->GetMessageString()<<endl;
   return false;
   }
   myIsError = aStatus != PCDM_SS_OK;
   if (myIsError)
   {
   switch (aStatus)
   {
   case PCDM_SS_DriverFailure:
   cout<<"OCAFApp_Appl_SDriverFailure"<<endl;
   break;
   case PCDM_SS_WriteFailure:
   cout<<"OCAFApp_Appl_SWriteFailure"<<endl;
   break;
   case PCDM_SS_Failure:
   default:
   cout<<"OCAFApp_Appl_SUnknownFailure"<<endl;
   break;
   }
   }
   myTransactionsAfterSave = 0;
   Standard::Purge(); // Release free memory
   */
  return !myIsError;
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
  // new command for this
  myDoc->NewCommand();
  // new command for all subs
  set<string>::iterator aSubIter = mySubs.begin();
  for(; aSubIter != mySubs.end(); aSubIter++)
    subDocument(*aSubIter)->startOperation();
}

void Model_Document::finishOperation()
{
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
  if (myDoc->GetAvailableUndos() > 0)
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

shared_ptr<ModelAPI_Feature> Model_Document::addFeature(string theID)
{
  shared_ptr<ModelAPI_Feature> aFeature = ModelAPI_PluginManager::get()->createFeature(theID);
  if (aFeature) {
    dynamic_pointer_cast<Model_Document>(aFeature->documentToAdd())->addFeature(aFeature);
  } else {
    // TODO: generate error that feature is not created
  }
  return aFeature;
}

void Model_Document::addFeature(const std::shared_ptr<ModelAPI_Feature> theFeature)
{
  const std::string& aGroup = theFeature->getGroup();
  TDF_Label aGroupLab = groupLabel(aGroup);
  TDF_Label anObjLab = aGroupLab.NewChild();
  std::shared_ptr<Model_Data> aData(new Model_Data);
  aData->setLabel(anObjLab);
  aData->setDocument(Model_Application::getApplication()->getDocument(myID));
  theFeature->setData(aData);
  setUniqueName(theFeature);
  theFeature->initAttributes();
  // keep the feature ID to restore document later correctly
  TDataStd_Comment::Set(anObjLab, theFeature->getKind().c_str());
  // put index of the feature in the group in the tree
  TDataStd_Integer::Set(anObjLab, myFeatures[aGroup].size());
  myFeatures[aGroup].push_back(theFeature);

  // event: feature is added
  static Event_ID anEvent = Event_Loop::eventByName(EVENT_FEATURE_CREATED);
  ModelAPI_FeatureUpdatedMessage aMsg(theFeature, anEvent);
  Event_Loop::loop()->send(aMsg);
}

shared_ptr<ModelAPI_Feature> Model_Document::feature(TDF_Label& theLabel)
{
  Handle(TDataStd_Integer) aFeatureIndex;
  if (theLabel.FindAttribute(TDataStd_Integer::GetID(), aFeatureIndex)) {
    Handle(TDataStd_Comment) aGroupID;
    if (theLabel.Father().FindAttribute(TDataStd_Comment::GetID(), aGroupID)) {
      string aGroup = TCollection_AsciiString(aGroupID->Get()).ToCString();
      return myFeatures[aGroup][aFeatureIndex->Get()];
    }
  }
  return std::shared_ptr<ModelAPI_Feature>(); // not found
}

int Model_Document::featureIndex(shared_ptr<ModelAPI_Feature> theFeature)
{
  if (theFeature->data()->document().get() != this) {
    return theFeature->data()->document()->featureIndex(theFeature);
  }
  shared_ptr<Model_Data> aData = dynamic_pointer_cast<Model_Data>(theFeature->data());
  Handle(TDataStd_Integer) aFeatureIndex;
  if (aData->label().FindAttribute(TDataStd_Integer::GetID(), aFeatureIndex)) {
    return aFeatureIndex->Get();
  }
  return -1; // not found
}

shared_ptr<ModelAPI_Document> Model_Document::subDocument(string theDocID)
{
  // just store sub-document identifier here to manage it later
  if (mySubs.find(theDocID) == mySubs.end())
    mySubs.insert(theDocID);
  return Model_Application::getApplication()->getDocument(theDocID);
}

shared_ptr<ModelAPI_Iterator> Model_Document::featuresIterator(const string theGroup)
{
  shared_ptr<Model_Document> aThis(Model_Application::getApplication()->getDocument(myID));
  // create an empty iterator for not existing group 
  // (to avoidance of attributes management outside the transaction)
  if (myGroups.find(theGroup) == myGroups.end())
    return shared_ptr<ModelAPI_Iterator>(new Model_Iterator());
  return shared_ptr<ModelAPI_Iterator>(new Model_Iterator(aThis, groupLabel(theGroup)));
}

shared_ptr<ModelAPI_Feature> Model_Document::feature(const string& theGroupID, const int theIndex)
{
  // TODO: optimize this method
  shared_ptr<ModelAPI_Iterator>  anIter = featuresIterator(theGroupID);
  for(int a = 0; a != theIndex && anIter->more(); anIter->next()) a++;
  return anIter->more() ? anIter->current() : shared_ptr<ModelAPI_Feature>();
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
  // to avoid creation of tag outside of the transaction (by iterator, for example)
  /*
  if (!myDoc->Main().FindChild(TAG_OBJECTS).IsAttribute(TDF_TagSource::GetID()))
    TDataStd_Comment::Set(myDoc->Main().FindChild(TAG_OBJECTS).NewChild(), "");
    */
}

TDF_Label Model_Document::groupLabel(const string theGroup)
{
  if (myGroups.find(theGroup) == myGroups.end()) {
    myGroups[theGroup] = myDoc->Main().FindChild(TAG_OBJECTS).NewChild();
    myGroupsNames.push_back(theGroup);
    // set to the group label the group idntifier to restore on "open"
    TDataStd_Comment::Set(myGroups[theGroup], theGroup.c_str());
    myFeatures[theGroup] = vector<shared_ptr<ModelAPI_Feature> >();
  }
  return myGroups[theGroup];
}

void Model_Document::setUniqueName(
  shared_ptr<ModelAPI_Feature> theFeature)
{
  // first count all objects of such kind to start with index = count + 1
  int aNumObjects = 0;
  shared_ptr<ModelAPI_Iterator> anIter = featuresIterator(theFeature->getGroup());
  for(; anIter->more(); anIter->next()) {
    if (anIter->currentKind() == theFeature->getKind())
      aNumObjects++;
  }
  // generate candidate name
  stringstream aNameStream;
  aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
  string aName = aNameStream.str();
  // check this is unique, if not, increase index by 1
  for(anIter = featuresIterator(theFeature->getGroup()); anIter->more();) {
    if (anIter->currentName() == aName) {
      aNumObjects++;
      stringstream aNameStream;
      aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
      // reinitialize iterator to make sure a new name is unique
      anIter = featuresIterator(theFeature->getGroup());
    } else anIter->next();
  }

  theFeature->data()->setName(aName);
}

void Model_Document::synchronizeFeatures()
{
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
    myFeatures.erase(*aGroupNamesIter);
    myGroups.erase(*aGroupNamesIter);
    aGroupNamesIter = myGroupsNames.erase(aGroupNamesIter);
  }
  // create new groups basing on the following data model update
  for(; aGroupsIter.More(); aGroupsIter.Next()) {
    string aGroupName = TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(
      aGroupsIter.Value())->Get()).ToCString();
    myGroupsNames.push_back(aGroupName);
    myGroups[aGroupName] = aGroupsIter.Value()->Label();
    myFeatures[aGroupName] = vector<shared_ptr<ModelAPI_Feature> >();
  }
  // update features group by group
  aGroupsIter.Initialize(myDoc->Main().FindChild(TAG_OBJECTS),
    TDataStd_Comment::GetID(), Standard_False);
  for(; aGroupsIter.More(); aGroupsIter.Next()) {
    string aGroupName = TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(
      aGroupsIter.Value())->Get()).ToCString();
    // iterate features in internal container
    vector<shared_ptr<ModelAPI_Feature> >& aFeatures = myFeatures[aGroupName];
    vector<shared_ptr<ModelAPI_Feature> >::iterator aFIter = aFeatures.begin();
    // and in parallel iterate labels of features
    TDF_ChildIDIterator aFLabIter(
      aGroupsIter.Value()->Label(), TDataStd_Comment::GetID(), Standard_False);
    while(aFIter != aFeatures.end() || aFLabIter.More()) {
      static const int INFINITE_TAG = MAXINT; // no label means that it exists somwhere in infinite
      int aFeatureTag = INFINITE_TAG; 
      if (aFIter != aFeatures.end()) { // existing tag for feature
        shared_ptr<Model_Data> aData = dynamic_pointer_cast<Model_Data>((*aFIter)->data());
        aFeatureTag = aData->label().Tag();
      }
      int aDSTag = INFINITE_TAG; 
      if (aFLabIter.More()) { // next label in DS is existing
        aDSTag = aFLabIter.Value()->Label().Tag();
      }
      if (aDSTag > aFeatureTag) { // feature is removed
        aFIter = aFeatures.erase(aFIter);
        // event: model is updated
        ModelAPI_FeatureDeletedMessage aMsg(
          Model_Application::getApplication()->getDocument(myID), aGroupName);
        Event_Loop::loop()->send(aMsg);
      } else if (aDSTag < aFeatureTag) { // a new feature is inserted
        // create a feature
        shared_ptr<ModelAPI_Feature> aFeature = ModelAPI_PluginManager::get()->createFeature(
          TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(
          aFLabIter.Value())->Get()).ToCString());

        std::shared_ptr<Model_Data> aData(new Model_Data);
        TDF_Label aLab = aFLabIter.Value()->Label();
        aData->setLabel(aLab);
        aData->setDocument(Model_Application::getApplication()->getDocument(myID));
        aFeature->setData(aData);
        aFeature->initAttributes();
        // event: model is updated
        static Event_ID anEvent = Event_Loop::eventByName(EVENT_FEATURE_CREATED);
        ModelAPI_FeatureUpdatedMessage aMsg(aFeature, anEvent);
        Event_Loop::loop()->send(aMsg);

        if (aFIter == aFeatures.end()) {
          aFeatures.push_back(aFeature);
          aFIter = aFeatures.end();
        } else {
          aFIter++;
          aFeatures.insert(aFIter, aFeature);
        }
        // feature for this label is added, so go to the next label
        aFLabIter.Next();
      } else { // nothing is changed, both iterators are incremented
        aFIter++;
        aFLabIter.Next();
      }
    }
  }
}
