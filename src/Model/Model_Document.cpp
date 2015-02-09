// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#include <Model_Document.h>
#include <Model_Data.h>
#include <Model_Application.h>
#include <Model_Session.h>
#include <Model_Events.h>
#include <Model_ResultPart.h>
#include <Model_ResultConstruction.h>
#include <Model_ResultBody.h>
#include <Model_ResultGroup.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_CompositeFeature.h>
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
#include <OSD_File.hxx>
#include <OSD_Path.hxx>

#include <climits>
#ifndef WIN32
#include <sys/stat.h>
#endif

#ifdef WIN32
# define _separator_ '\\'
#else
# define _separator_ '/'
#endif

static const int UNDO_LIMIT = 1000;  // number of possible undo operations (big for sketcher)

static const int TAG_GENERAL = 1;  // general properties tag
static const int TAG_OBJECTS = 2;  // tag of the objects sub-tree (features, results)
static const int TAG_HISTORY = 3;  // tag of the history sub-tree (python dump)

// feature sub-labels
static const int TAG_FEATURE_ARGUMENTS = 1;  ///< where the arguments are located
static const int TAG_FEATURE_RESULTS = 2;  ///< where the results are located

///
/// 0:1:2 - where features are located
/// 0:1:2:N:1 - data of the feature N
/// 0:1:2:N:2:K:1 - data of the K result of the feature N

Model_Document::Model_Document(const std::string theID, const std::string theKind)
    : myID(theID), myKind(theKind),
      myDoc(new TDocStd_Document("BinOcaf"))  // binary OCAF format
{
  myDoc->SetUndoLimit(UNDO_LIMIT);  
  myTransactionSave = 0;
  myExecuteFeatures = true;
  // to have something in the document and avoid empty doc open/save problem
  // in transaction for nesting correct working
  myDoc->NewCommand();
  TDataStd_Integer::Set(myDoc->Main().Father(), 0);
  myDoc->CommitCommand();
}

/// Returns the file name of this document by the nameof directory and identifuer of a document
static TCollection_ExtendedString DocFileName(const char* theFileName, const std::string& theID)
{
  TCollection_ExtendedString aPath((const Standard_CString) theFileName);
  // remove end-separators
  while(aPath.Length() && (aPath.Value(aPath.Length()) == '\\' || aPath.Value(aPath.Length()) == '/'))
    aPath.Remove(aPath.Length());
  aPath += _separator_;
  aPath += theID.c_str();
  aPath += ".cbf";  // standard binary file extension
  return aPath;
}

bool Model_Document::load(const char* theFileName)
{
  Handle(Model_Application) anApp = Model_Application::getApplication();
  if (this == Model_Session::get()->moduleDocument().get()) {
    anApp->setLoadPath(theFileName);
  }
  TCollection_ExtendedString aPath(DocFileName(theFileName, myID));
  PCDM_ReaderStatus aStatus = (PCDM_ReaderStatus) -1;
  try {
    aStatus = anApp->Open(aPath, myDoc);
  } catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    Events_Error::send(
        std::string("Exception in opening of document: ") + aFail->GetMessageString());
    return false;
  }
  bool isError = aStatus != PCDM_RS_OK;
  if (isError) {
    switch (aStatus) {
      case PCDM_RS_UnknownDocument:
        Events_Error::send(std::string("Can not open document"));
        break;
      case PCDM_RS_AlreadyRetrieved:
        Events_Error::send(std::string("Can not open document: already opened"));
        break;
      case PCDM_RS_AlreadyRetrievedAndModified:
        Events_Error::send(
            std::string("Can not open document: already opened and modified"));
        break;
      case PCDM_RS_NoDriver:
        Events_Error::send(std::string("Can not open document: driver library is not found"));
        break;
      case PCDM_RS_UnknownFileDriver:
        Events_Error::send(std::string("Can not open document: unknown driver for opening"));
        break;
      case PCDM_RS_OpenError:
        Events_Error::send(std::string("Can not open document: file open error"));
        break;
      case PCDM_RS_NoVersion:
        Events_Error::send(std::string("Can not open document: invalid version"));
        break;
      case PCDM_RS_NoModel:
        Events_Error::send(std::string("Can not open document: no data model"));
        break;
      case PCDM_RS_NoDocument:
        Events_Error::send(std::string("Can not open document: no document inside"));
        break;
      case PCDM_RS_FormatFailure:
        Events_Error::send(std::string("Can not open document: format failure"));
        break;
      case PCDM_RS_TypeNotFoundInSchema:
        Events_Error::send(std::string("Can not open document: invalid object"));
        break;
      case PCDM_RS_UnrecognizedFileFormat:
        Events_Error::send(std::string("Can not open document: unrecognized file format"));
        break;
      case PCDM_RS_MakeFailure:
        Events_Error::send(std::string("Can not open document: make failure"));
        break;
      case PCDM_RS_PermissionDenied:
        Events_Error::send(std::string("Can not open document: permission denied"));
        break;
      case PCDM_RS_DriverFailure:
        Events_Error::send(std::string("Can not open document: driver failure"));
        break;
      default:
        Events_Error::send(std::string("Can not open document: unknown error"));
        break;
    }
  }
  if (!isError) {
    myDoc->SetUndoLimit(UNDO_LIMIT);
    // to avoid the problem that feature is created in the current, not this, document
    std::shared_ptr<Model_Session> aSession = 
      std::dynamic_pointer_cast<Model_Session>(Model_Session::get());
    aSession->setActiveDocument(anApp->getDocument(myID), false);
    aSession->setCheckTransactions(false);
    synchronizeFeatures(false, true);
    aSession->setCheckTransactions(true);
    aSession->setActiveDocument(Model_Session::get()->moduleDocument(), false);
    aSession->setActiveDocument(anApp->getDocument(myID), true);
  }
  return !isError;
}

bool Model_Document::save(const char* theFileName, std::list<std::string>& theResults)
{
  // create a directory in the root document if it is not yet exist
  Handle(Model_Application) anApp = Model_Application::getApplication();
  if (this == Model_Session::get()->moduleDocument().get()) {
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
    aStatus = anApp->SaveAs(myDoc, aPath);
  } catch (Standard_Failure) {
    Handle(Standard_Failure) aFail = Standard_Failure::Caught();
    Events_Error::send(
        std::string("Exception in saving of document: ") + aFail->GetMessageString());
    return false;
  }
  bool isDone = aStatus == PCDM_SS_OK || aStatus == PCDM_SS_No_Obj;
  if (!isDone) {
    switch (aStatus) {
      case PCDM_SS_DriverFailure:
        Events_Error::send(std::string("Can not save document: save driver-library failure"));
        break;
      case PCDM_SS_WriteFailure:
        Events_Error::send(std::string("Can not save document: file writing failure"));
        break;
      case PCDM_SS_Failure:
      default:
        Events_Error::send(std::string("Can not save document"));
        break;
    }
  }
  myTransactionSave = myTransactions.size();
  if (isDone) {  // save also sub-documents if any
    theResults.push_back(TCollection_AsciiString(aPath).ToCString());
    const std::set<std::string> aSubs = subDocuments(false);
    std::set<std::string>::iterator aSubIter = aSubs.begin();
    for (; aSubIter != aSubs.end() && isDone; aSubIter++) {
      if (anApp->isLoadByDemand(*aSubIter)) { 
        // copy not-activated document that is not in the memory
        std::string aDocName = *aSubIter;
        if (!aDocName.empty()) {
          // just copy file
          TCollection_AsciiString aSubPath(DocFileName(anApp->loadPath().c_str(), aDocName));
          OSD_Path aPath(aSubPath);
          OSD_File aFile(aPath);
          if (aFile.Exists()) {
            TCollection_AsciiString aDestinationDir(DocFileName(theFileName, aDocName));
            OSD_Path aDestination(aDestinationDir);
            aFile.Copy(aDestination);
            theResults.push_back(aDestinationDir.ToCString());
          } else {
            Events_Error::send(
              std::string("Can not open file ") + aSubPath.ToCString() + " for saving");
          }
        }
      } else { // simply save opened document
        isDone = subDoc(*aSubIter)->save(theFileName, theResults);
      }
    }
  }
  return isDone;
}

void Model_Document::close(const bool theForever)
{
  std::shared_ptr<ModelAPI_Session> aPM = Model_Session::get();
  if (this != aPM->moduleDocument().get() && this == aPM->activeDocument().get()) {
    aPM->setActiveDocument(aPM->moduleDocument());
  } else if (this == aPM->moduleDocument().get()) {
    // erase the active document if root is closed
    aPM->setActiveDocument(DocumentPtr());
  }
  // close all subs
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    subDoc(*aSubIter)->close(theForever);

  // close for thid document needs no transaction in this document
  std::static_pointer_cast<Model_Session>(Model_Session::get())->setCheckTransactions(false);

  // delete all features of this document
  std::shared_ptr<ModelAPI_Document> aThis = 
    Model_Application::getApplication()->getDocument(myID);
  Events_Loop* aLoop = Events_Loop::loop();
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFeaturesIter(myObjs);
  for(; aFeaturesIter.More(); aFeaturesIter.Next()) {
    FeaturePtr aFeature = aFeaturesIter.Value();
    static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
    ModelAPI_EventCreator::get()->sendDeleted(aThis, ModelAPI_Feature::group());
    ModelAPI_EventCreator::get()->sendUpdated(aFeature, EVENT_DISP);
    aFeature->eraseResults();
    if (theForever) { // issue #294: do not delete content of the document until it can be redone
      aFeature->erase();
    } else {
      aFeature->data()->execState(ModelAPI_StateMustBeUpdated);
    }
  }
  if (theForever) {
    myObjs.Clear();
  }
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));

  // close all only if it is really asked, otherwise it can be undoed/redoed
  if (theForever) {
    if (myDoc->CanClose() == CDM_CCS_OK)
      myDoc->Close();
  }

  std::static_pointer_cast<Model_Session>(Model_Session::get())->setCheckTransactions(true);
}

void Model_Document::startOperation()
{
  if (myDoc->HasOpenCommand()) {  // start of nested command
    if (myDoc->CommitCommand()) { // commit the current: it will contain all nested after compactification
      myTransactions.rbegin()->myOCAFNum++; // if has open command, the list is not empty
    }
    myNestedNum.push_back(0); // start of nested operation with zero transactions inside yet
    myDoc->OpenCommand();
  } else {  // start the simple command
    myDoc->NewCommand();
  }
  // starts a new operation
  myTransactions.push_back(Transaction());
  if (!myNestedNum.empty())
    (*myNestedNum.rbegin())++;
  myRedos.clear();
  // new command for all subs
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    subDoc(*aSubIter)->startOperation();
}

void Model_Document::compactNested()
{
  if (!myNestedNum.empty()) {
    int aNumToCompact = *(myNestedNum.rbegin());
    int aSumOfTransaction = 0;
    for(int a = 0; a < aNumToCompact; a++) {
      aSumOfTransaction += myTransactions.rbegin()->myOCAFNum;
      myTransactions.pop_back();
    }
    // the latest transaction is the start of lower-level operation which startes the nested
    myTransactions.rbegin()->myOCAFNum += aSumOfTransaction;
    myNestedNum.pop_back();
  }
}

bool Model_Document::finishOperation()
{
  bool isNestedClosed = !myDoc->HasOpenCommand() && !myNestedNum.empty();
  static std::shared_ptr<Model_Session> aSession = 
    std::static_pointer_cast<Model_Session>(Model_Session::get());
  synchronizeBackRefs();
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_TOHIDE));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  // this must be here just after everything is finished but before real transaction stop
  // to avoid messages about modifications outside of the transaction
  // and to rebuild everything after all updates and creates
  if (Model_Session::get()->moduleDocument().get() == this) { // once for root document
    Events_Loop::loop()->autoFlush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
    static std::shared_ptr<Events_Message> aFinishMsg
      (new Events_Message(Events_Loop::eventByName("FinishOperation")));
    Events_Loop::loop()->send(aFinishMsg);
    Events_Loop::loop()->autoFlush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED), false);
  }
  // to avoid "updated" message appearance by updater
  //aLoop->clear(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));

  // finish for all subs first: to avoid nested finishing and "isOperation" calls problems inside
  bool aResult = false;
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    if (subDoc(*aSubIter)->finishOperation())
      aResult = true;

  // transaction may be empty if this document was created during this transaction (create part)
  if (!myTransactions.empty() && myDoc->CommitCommand()) { // if commit is successfull, just increment counters
    myTransactions.rbegin()->myOCAFNum++;
    aResult = true;
  }

  if (isNestedClosed) {
    compactNested();
  }
  if (!aResult && !myTransactions.empty() /* it can be for just created part document */)
    aResult = myTransactions.rbegin()->myOCAFNum != 0;

  if (!aResult && Model_Session::get()->moduleDocument().get() == this) {
    // nothing inside in all documents, so remove this transaction from the transactions list
    undoInternal(true, false);
    myDoc->ClearRedos();
    myRedos.clear();
  }
  return aResult;
}

void Model_Document::abortOperation()
{
  if (!myNestedNum.empty() && !myDoc->HasOpenCommand()) {  // abort all what was done in nested
    compactNested();
    undoInternal(false, false);
    myDoc->ClearRedos();
    myRedos.clear();
  } else { // abort the current
    int aNumTransactions = myTransactions.rbegin()->myOCAFNum;
    myTransactions.pop_back();
    if (!myNestedNum.empty())
      (*myNestedNum.rbegin())--;
    // roll back the needed number of transactions
    myDoc->AbortCommand();
    for(int a = 0; a < aNumTransactions; a++)
      myDoc->Undo();
    myDoc->ClearRedos();
  }
  synchronizeFeatures(true, false); // references were not changed since transaction start
  // abort for all subs
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    subDoc(*aSubIter)->abortOperation();
}

bool Model_Document::isOperation()
{
  // operation is opened for all documents: no need to check subs
  return myDoc->HasOpenCommand() == Standard_True ;
}

bool Model_Document::isModified()
{
  // is modified if at least one operation was commited and not undoed
  return myTransactions.size() != myTransactionSave || isOperation();
}

bool Model_Document::canUndo()
{
  if (myDoc->GetAvailableUndos() > 0 && (myNestedNum.empty() || *myNestedNum.rbegin() != 0) &&
      !myTransactions.empty() /* for omitting the first useless transaction */)
    return true;
  // check other subs contains operation that can be undoed
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    if (subDoc(*aSubIter)->canUndo())
      return true;
  return false;
}

void Model_Document::undoInternal(const bool theWithSubs, const bool theSynchronize)
{
  int aNumTransactions = myTransactions.rbegin()->myOCAFNum;
  myRedos.push_back(*myTransactions.rbegin());
  myTransactions.pop_back();
  if (!myNestedNum.empty())
    (*myNestedNum.rbegin())--;
  // roll back the needed number of transactions
  for(int a = 0; a < aNumTransactions; a++)
    myDoc->Undo();

  if (theSynchronize)
    synchronizeFeatures(true, true);
  if (theWithSubs) {
    // undo for all subs
    const std::set<std::string> aSubs = subDocuments(true);
    std::set<std::string>::iterator aSubIter = aSubs.begin();
    for (; aSubIter != aSubs.end(); aSubIter++)
      subDoc(*aSubIter)->undoInternal(theWithSubs, theSynchronize);
  }
}

void Model_Document::undo()
{
  undoInternal(true, true);
}

bool Model_Document::canRedo()
{
  if (myDoc->GetAvailableRedos() > 0)
    return true;
  // check other subs contains operation that can be redoed
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    if (subDoc(*aSubIter)->canRedo())
      return true;
  return false;
}

void Model_Document::redo()
{
  if (!myNestedNum.empty())
    (*myNestedNum.rbegin())++;
  int aNumRedos = myRedos.rbegin()->myOCAFNum;
  myTransactions.push_back(*myRedos.rbegin());
  myRedos.pop_back();
  for(int a = 0; a < aNumRedos; a++)
    myDoc->Redo();

  synchronizeFeatures(true, true);
  // redo for all subs
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    subDoc(*aSubIter)->redo();
}

std::list<std::string> Model_Document::undoList() const
{
  std::list<std::string> aResult;
  std::list<Transaction>::const_reverse_iterator aTrIter = myTransactions.crbegin();
  int aNumUndo = myTransactions.size();
  if (!myNestedNum.empty())
    aNumUndo = *myNestedNum.rbegin();
  for( ; aNumUndo > 0; aTrIter++, aNumUndo--) {
    aResult.push_back(aTrIter->myId);
  }
  return aResult;
}

std::list<std::string> Model_Document::redoList() const
{
  std::list<std::string> aResult;
  std::list<Transaction>::const_reverse_iterator aTrIter = myRedos.crbegin();
  for( ; aTrIter != myRedos.crend(); aTrIter++) {
    aResult.push_back(aTrIter->myId);
  }
  return aResult;
}

void Model_Document::operationId(const std::string& theId)
{
  myTransactions.rbegin()->myId = theId;
}

/// Append to the array of references a new referenced label
static void AddToRefArray(TDF_Label& theArrayLab, TDF_Label& theReferenced)
{
  Handle(TDataStd_ReferenceArray) aRefs;
  if (!theArrayLab.FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs)) {
    aRefs = TDataStd_ReferenceArray::Set(theArrayLab, 0, 0);
    aRefs->SetValue(0, theReferenced);
  } else {  // extend array by one more element
    Handle(TDataStd_HLabelArray1) aNewArray = new TDataStd_HLabelArray1(aRefs->Lower(),
                                                                        aRefs->Upper() + 1);
    for (int a = aRefs->Lower(); a <= aRefs->Upper(); a++) {
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
  FeaturePtr aFeature = ModelAPI_Session::get()->createFeature(theID);
  if (!aFeature)
    return aFeature;
  std::shared_ptr<Model_Document> aDocToAdd = std::dynamic_pointer_cast<Model_Document>(
      aFeature->documentToAdd());
  if (aFeature) {
    TDF_Label aFeatureLab;
    if (!aFeature->isAction()) {  // do not add action to the data model
      TDF_Label aFeaturesLab = aDocToAdd->featuresLabel();
      aFeatureLab = aFeaturesLab.NewChild();
      aDocToAdd->initData(aFeature, aFeatureLab, TAG_FEATURE_ARGUMENTS);
      // keep the feature ID to restore document later correctly
      TDataStd_Comment::Set(aFeatureLab, aFeature->getKind().c_str());
      aDocToAdd->myObjs.Bind(aFeatureLab, aFeature);
      // store feature in the history of features array
      if (aFeature->isInHistory()) {
        AddToRefArray(aFeaturesLab, aFeatureLab);
      }
    }
    if (!aFeature->isAction()) {  // do not add action to the data model
      // event: feature is added
      static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
      ModelAPI_EventCreator::get()->sendUpdated(aFeature, anEvent);
    } else { // feature must be executed
       // no creation event => updater not working, problem with remove part
      aFeature->execute();
    }
  }
  return aFeature;
}

/// Appenad to the array of references a new referenced label.
/// If theIndex is not -1, removes element at thisindex, not theReferenced.
/// \returns the index of removed element
static int RemoveFromRefArray(TDF_Label theArrayLab, TDF_Label theReferenced, const int theIndex =
                                  -1)
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

void Model_Document::refsToFeature(FeaturePtr theFeature,
                                   std::set<std::shared_ptr<ModelAPI_Feature> >& theRefs,
                                   const bool isSendError)
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

void Model_Document::removeFeature(FeaturePtr theFeature/*, const bool theCheck*/)
{
  std::shared_ptr<Model_Data> aData = std::static_pointer_cast<Model_Data>(theFeature->data());
  if (aData) {
    TDF_Label aFeatureLabel = aData->label().Father();
    if (myObjs.IsBound(aFeatureLabel))
      myObjs.UnBind(aFeatureLabel);
    else
      return;  // not found feature => do not remove
    // erase fields
    theFeature->erase();
    // erase all attributes under the label of feature
    aFeatureLabel.ForgetAllAttributes();
    // remove it from the references array
    if (theFeature->isInHistory()) {
      RemoveFromRefArray(featuresLabel(), aFeatureLabel);
    }
  }
  // event: feature is deleted
  ModelAPI_EventCreator::get()->sendDeleted(theFeature->document(), ModelAPI_Feature::group());
}

FeaturePtr Model_Document::feature(TDF_Label& theLabel) const
{
  if (myObjs.IsBound(theLabel))
    return myObjs.Find(theLabel);
  return FeaturePtr();  // not found
}

ObjectPtr Model_Document::object(TDF_Label theLabel)
{
  // try feature by label
  FeaturePtr aFeature = feature(theLabel);
  if (aFeature)
    return feature(theLabel);
  TDF_Label aFeatureLabel = theLabel.Father().Father();  // let's suppose it is result
  aFeature = feature(aFeatureLabel);
  if (aFeature) {
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.cbegin();
    for (; aRIter != aResults.cend(); aRIter++) {
      std::shared_ptr<Model_Data> aResData = std::dynamic_pointer_cast<Model_Data>(
          (*aRIter)->data());
      if (aResData->label().Father().IsEqual(theLabel))
        return *aRIter;
    }
  }
  return FeaturePtr();  // not found
}

std::shared_ptr<ModelAPI_Document> Model_Document::subDocument(std::string theDocID)
{
  return Model_Application::getApplication()->getDocument(theDocID);
}

const std::set<std::string> Model_Document::subDocuments(const bool theActivatedOnly) const
{
  std::set<std::string> aResult;
  // comment must be in any feature: it is kind
  int anIndex = 0;
  TDF_ChildIDIterator aLabIter(featuresLabel(), TDataStd_Comment::GetID());
  for (; aLabIter.More(); aLabIter.Next()) {
    TDF_Label aFLabel = aLabIter.Value()->Label();
    FeaturePtr aFeature = feature(aFLabel);
    if (aFeature.get()) { // if document is closed the feature may be not in myObjs map
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        if ((*aRIter)->groupName() != ModelAPI_ResultPart::group()) continue;
        if ((*aRIter)->isInHistory()) {
          ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aRIter);
          if (aPart && (!theActivatedOnly || aPart->isActivated()))
            aResult.insert(aPart->data()->name());
        }
      }
    }
  }
  return aResult;
}

std::shared_ptr<Model_Document> Model_Document::subDoc(std::string theDocID)
{
  // just store sub-document identifier here to manage it later
  return std::dynamic_pointer_cast<Model_Document>(
    Model_Application::getApplication()->getDocument(theDocID));
}

ObjectPtr Model_Document::object(const std::string& theGroupID, const int theIndex,
                                 const bool theHidden)
{
  if (theGroupID == ModelAPI_Feature::group()) {
    if (theHidden) {
      int anIndex = 0;
      TDF_ChildIDIterator aLabIter(featuresLabel(), TDataStd_Comment::GetID());
      for (; aLabIter.More(); aLabIter.Next()) {
        if (theIndex == anIndex) {
          TDF_Label aFLabel = aLabIter.Value()->Label();
          return feature(aFLabel);
        }
        anIndex++;
      }
    } else {
      Handle(TDataStd_ReferenceArray) aRefs;
      if (!featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
        return ObjectPtr();
      if (aRefs->Lower() > theIndex || aRefs->Upper() < theIndex)
        return ObjectPtr();
      TDF_Label aFeatureLabel = aRefs->Value(theIndex);
      return feature(aFeatureLabel);
    }
  } else {
    // comment must be in any feature: it is kind
    int anIndex = 0;
    TDF_ChildIDIterator aLabIter(featuresLabel(), TDataStd_Comment::GetID());
    for (; aLabIter.More(); aLabIter.Next()) {
      TDF_Label aFLabel = aLabIter.Value()->Label();
      FeaturePtr aFeature = feature(aFLabel);
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        if ((*aRIter)->groupName() != theGroupID) continue;
        bool isIn = theHidden && (*aRIter)->isInHistory();
        if (!isIn && (*aRIter)->isInHistory()) { // check that there is nobody references this result
          isIn = !(*aRIter)->isConcealed();
        }
        if (isIn) {
          if (anIndex == theIndex)
            return *aRIter;
          anIndex++;
        }
      }
    }
  }
  // not found
  return ObjectPtr();
}

int Model_Document::size(const std::string& theGroupID, const bool theHidden)
{
  int aResult = 0;
  if (theGroupID == ModelAPI_Feature::group()) {
    if (theHidden) {
      return myObjs.Size();
    } else {
      Handle(TDataStd_ReferenceArray) aRefs;
      if (featuresLabel().FindAttribute(TDataStd_ReferenceArray::GetID(), aRefs))
        return aRefs->Length();
    }
  } else {
    // comment must be in any feature: it is kind
    TDF_ChildIDIterator aLabIter(featuresLabel(), TDataStd_Comment::GetID());
    for (; aLabIter.More(); aLabIter.Next()) {
      TDF_Label aFLabel = aLabIter.Value()->Label();
      FeaturePtr aFeature = feature(aFLabel);
      if (!aFeature) // may be on close
        continue;
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      for (; aRIter != aResults.cend(); aRIter++) {
        if ((*aRIter)->groupName() != theGroupID) continue;
        bool isIn = theHidden;
        if (!isIn && (*aRIter)->isInHistory()) { // check that there is nobody references this result
          isIn = !(*aRIter)->isConcealed();
        }
        if (isIn)
          aResult++;
      }
    }
  }
  // group is not found
  return aResult;
}

TDF_Label Model_Document::featuresLabel() const
{
  return myDoc->Main().FindChild(TAG_OBJECTS);
}

void Model_Document::setUniqueName(FeaturePtr theFeature)
{
  if (!theFeature->data()->name().empty())
    return;  // not needed, name is already defined
  std::string aName;  // result
  // first count all objects of such kind to start with index = count + 1
  int aNumObjects = 0;
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFIter(myObjs);
  for (; aFIter.More(); aFIter.Next()) {
    if (aFIter.Value()->getKind() == theFeature->getKind())
      aNumObjects++;
  }
  // generate candidate name
  std::stringstream aNameStream;
  aNameStream << theFeature->getKind() << "_" << aNumObjects + 1;
  aName = aNameStream.str();
  // check this is unique, if not, increase index by 1
  for (aFIter.Initialize(myObjs); aFIter.More();) {
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
      aFIter.Initialize(myObjs);
    } else
      aFIter.Next();
  }
  theFeature->data()->setName(aName);
}

void Model_Document::initData(ObjectPtr theObj, TDF_Label theLab, const int theTag)
{
  std::shared_ptr<ModelAPI_Document> aThis = Model_Application::getApplication()->getDocument(
      myID);
  std::shared_ptr<Model_Data> aData(new Model_Data);
  aData->setLabel(theLab.FindChild(theTag));
  aData->setObject(theObj);
  theObj->setDoc(aThis);
  theObj->setData(aData);
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theObj);
  if (aFeature) {
    setUniqueName(aFeature);  // must be before "initAttributes" because duplicate part uses name
    aFeature->initAttributes();
  }
}

void Model_Document::synchronizeFeatures(const bool theMarkUpdated, const bool theUpdateReferences)
{
  std::shared_ptr<ModelAPI_Document> aThis = 
    Model_Application::getApplication()->getDocument(myID);
  // after all updates, sends a message that groups of features were created or updated
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->activateFlushes(false);

  // update all objects by checking are they of labels or not
  std::set<FeaturePtr> aNewFeatures, aKeptFeatures;
  TDF_ChildIDIterator aLabIter(featuresLabel(), TDataStd_Comment::GetID());
  for (; aLabIter.More(); aLabIter.Next()) {
    TDF_Label aFeatureLabel = aLabIter.Value()->Label();
    FeaturePtr aFeature;
    if (!myObjs.IsBound(aFeatureLabel)) {  // a new feature is inserted
      // create a feature
      aFeature = ModelAPI_Session::get()->createFeature(
          TCollection_AsciiString(Handle(TDataStd_Comment)::DownCast(aLabIter.Value())->Get())
              .ToCString());
      if (!aFeature) {  // somethig is wrong, most probably, the opened document has invalid structure
        Events_Error::send("Invalid type of object in the document");
        aLabIter.Value()->Label().ForgetAllAttributes();
        continue;
      }
      // this must be before "setData" to redo the sketch line correctly
      myObjs.Bind(aFeatureLabel, aFeature);
      aNewFeatures.insert(aFeature);
      initData(aFeature, aFeatureLabel, TAG_FEATURE_ARGUMENTS);

      // event: model is updated
      static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
      ModelAPI_EventCreator::get()->sendUpdated(aFeature, anEvent);
    } else {  // nothing is changed, both iterators are incremented
      aFeature = myObjs.Find(aFeatureLabel);
      aKeptFeatures.insert(aFeature);
      if (theMarkUpdated) {
        static Events_ID anEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
        ModelAPI_EventCreator::get()->sendUpdated(aFeature, anEvent);
      }
    }
  }
  // update results of thefeatures (after features created because they may be connected, like sketch and sub elements)
  std::list<FeaturePtr> aComposites; // composites must be updated after their subs (issue 360)
  TDF_ChildIDIterator aLabIter2(featuresLabel(), TDataStd_Comment::GetID());
  for (; aLabIter2.More(); aLabIter2.Next()) {
    TDF_Label aFeatureLabel = aLabIter2.Value()->Label();
    if (myObjs.IsBound(aFeatureLabel)) {  // a new feature is inserted
      FeaturePtr aFeature = myObjs.Find(aFeatureLabel);
      if (std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature).get())
        aComposites.push_back(aFeature);
      updateResults(aFeature);
    }
  }
  std::list<FeaturePtr>::iterator aComposite = aComposites.begin();
  for(; aComposite != aComposites.end(); aComposite++) {
    updateResults(*aComposite);
  }

  // check all features are checked: if not => it was removed
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFIter(myObjs);
  while (aFIter.More()) {
    if (aKeptFeatures.find(aFIter.Value()) == aKeptFeatures.end()
        && aNewFeatures.find(aFIter.Value()) == aNewFeatures.end()) {
      FeaturePtr aFeature = aFIter.Value();
      // event: model is updated
      //if (aFeature->isInHistory()) {
        ModelAPI_EventCreator::get()->sendDeleted(aThis, ModelAPI_Feature::group());
      //}
      // results of this feature must be redisplayed (hided)
      static Events_ID EVENT_DISP = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
      // redisplay also removed feature (used for sketch and AISObject)
      ModelAPI_EventCreator::get()->sendUpdated(aFeature, EVENT_DISP);
      aFeature->erase();
      // unbind after the "erase" call: on abort sketch is removes sub-objects that corrupts aFIter
      myObjs.UnBind(aFIter.Key());
      // reinitialize iterator because unbind may corrupt the previous order in the map
      aFIter.Initialize(myObjs);
    } else
      aFIter.Next();
  }

  if (theUpdateReferences) {
    synchronizeBackRefs();
  }

  myExecuteFeatures = false;
  aLoop->activateFlushes(true);

  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_TOHIDE));
  myExecuteFeatures = true;
}

void Model_Document::synchronizeBackRefs()
{
  std::shared_ptr<ModelAPI_Document> aThis = 
    Model_Application::getApplication()->getDocument(myID);
  // keeps the concealed flags of result to catch the change and create created/deleted events
  std::list<std::pair<ResultPtr, bool> > aConcealed;
  // first cycle: erase all data about back-references
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator aFeatures(myObjs);
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
      if (aResData) {
        aConcealed.push_back(std::pair<ResultPtr, bool>(*aRIter, (*aRIter)->isConcealed()));
        aResData->eraseBackReferences();
      }
    }
  }

  // second cycle: set new back-references: only features may have reference, iterate only them
  ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();
  for(aFeatures.Initialize(myObjs); aFeatures.More(); aFeatures.Next()) {
    FeaturePtr aFeature = aFeatures.Value();
    std::shared_ptr<Model_Data> aFData = 
      std::dynamic_pointer_cast<Model_Data>(aFeature->data());
    if (aFData) {
      std::list<std::pair<std::string, std::list<ObjectPtr> > > aRefs;
      aFData->referencesToObjects(aRefs);
      std::list<std::pair<std::string, std::list<ObjectPtr> > >::iterator aRefsIter = aRefs.begin();
      for(; aRefsIter != aRefs.end(); aRefsIter++) {
        std::list<ObjectPtr>::iterator aRefTo = aRefsIter->second.begin();
        for(; aRefTo != aRefsIter->second.end(); aRefTo++) {
          if (*aRefTo) {
            std::shared_ptr<Model_Data> aRefData = 
              std::dynamic_pointer_cast<Model_Data>((*aRefTo)->data());
            aRefData->addBackReference(aFeature, aRefsIter->first); // here the Concealed flag is updated
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
        ModelAPI_EventCreator::get()->sendDeleted(aThis, aCIter->first->groupName());
        // redisplay for the viewer (it must be disappeared also)
        static Events_ID EVENT_DISP = 
          Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
        ModelAPI_EventCreator::get()->sendUpdated(aCIter->first, EVENT_DISP);
      }
    }
  }
}

TDF_Label Model_Document::resultLabel(
  const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theResultIndex) 
{
  const std::shared_ptr<Model_Data>& aData = 
    std::dynamic_pointer_cast<Model_Data>(theFeatureData);
  return aData->label().Father().FindChild(TAG_FEATURE_RESULTS).FindChild(theResultIndex + 1);
}

void Model_Document::storeResult(std::shared_ptr<ModelAPI_Data> theFeatureData,
                                 std::shared_ptr<ModelAPI_Result> theResult,
                                 const int theResultIndex)
{
  std::shared_ptr<ModelAPI_Document> aThis = 
    Model_Application::getApplication()->getDocument(myID);
  theResult->setDoc(aThis);
  initData(theResult, resultLabel(theFeatureData, theResultIndex), TAG_FEATURE_ARGUMENTS);
  if (theResult->data()->name().empty()) {  // if was not initialized, generate event and set a name
    theResult->data()->setName(theFeatureData->name());
  }
}

std::shared_ptr<ModelAPI_ResultConstruction> Model_Document::createConstruction(
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

std::shared_ptr<ModelAPI_ResultBody> Model_Document::createBody(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  TDF_Label aLab = resultLabel(theFeatureData, theIndex);
  TDataStd_Comment::Set(aLab, ModelAPI_ResultBody::group().c_str());
  ObjectPtr anOldObject = object(aLab);
  std::shared_ptr<ModelAPI_ResultBody> aResult;
  if (anOldObject) {
    aResult = std::dynamic_pointer_cast<ModelAPI_ResultBody>(anOldObject);
  }
  if (!aResult) {
    aResult = std::shared_ptr<ModelAPI_ResultBody>(new Model_ResultBody);
    storeResult(theFeatureData, aResult, theIndex);
  }
  return aResult;
}

std::shared_ptr<ModelAPI_ResultPart> Model_Document::createPart(
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

std::shared_ptr<ModelAPI_ResultGroup> Model_Document::createGroup(
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

std::shared_ptr<ModelAPI_Feature> Model_Document::feature(
    const std::shared_ptr<ModelAPI_Result>& theResult)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theResult->data());
  if (aData) {
    TDF_Label aFeatureLab = aData->label().Father().Father().Father();
    return feature(aFeatureLab);
  }
  return FeaturePtr();
}

void Model_Document::updateResults(FeaturePtr theFeature)
{
  // for not persistent is will be done by parametric updater automatically
  //if (!theFeature->isPersistentResult()) return;
  // check the existing results and remove them if there is nothing on the label
  std::list<ResultPtr>::const_iterator aResIter = theFeature->results().cbegin();
  while(aResIter != theFeature->results().cend()) {
    ResultPtr aBody = std::dynamic_pointer_cast<ModelAPI_Result>(*aResIter);
    if (aBody) {
      if (!aBody->data()->isValid()) { 
        // found a disappeared result => remove it
        theFeature->removeResult(aBody);
        // start iterate from beginning because iterator is corrupted by removing
        aResIter = theFeature->results().cbegin();
        continue;
      }
    }
    aResIter++;
  }
  // it may be on undo
  if (!theFeature->data() || !theFeature->data()->isValid())
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
        if (aGroup->Get() == ModelAPI_ResultBody::group().c_str()) {
          aNewBody = createBody(theFeature->data(), aResIndex);
        } else if (aGroup->Get() == ModelAPI_ResultPart::group().c_str()) {
          aNewBody = createPart(theFeature->data(), aResIndex);
        } else if (aGroup->Get() == ModelAPI_ResultConstruction::group().c_str()) {
          theFeature->execute(); // construction shapes are needed for sketch solver
          break;
        } else if (aGroup->Get() == ModelAPI_ResultGroup::group().c_str()) {
          aNewBody = createGroup(theFeature->data(), aResIndex);
        } else {
          Events_Error::send(std::string("Unknown type of result is found in the document:") +
            TCollection_AsciiString(aGroup->Get()).ToCString());
        }
      }
      if (aNewBody) {
        theFeature->setResult(aNewBody, aResIndex);
      }
    }
  }
}

Standard_Integer HashCode(const TDF_Label& theLab, const Standard_Integer theUpper)
{
  return TDF_LabelMapHasher::HashCode(theLab, theUpper);

}
Standard_Boolean IsEqual(const TDF_Label& theLab1, const TDF_Label& theLab2)
{
  return TDF_LabelMapHasher::IsEqual(theLab1, theLab2);
}

void Model_Document::addNamingName(const TDF_Label theLabel, std::string theName)
{
  myNamingNames[theName] = theLabel;
}

TDF_Label Model_Document::findNamingName(std::string theName)
{
  std::map<std::string, TDF_Label>::iterator aFind = myNamingNames.find(theName);
  if (aFind == myNamingNames.end())
    return TDF_Label(); // not found
  return aFind->second;
}

ResultPtr Model_Document::findByName(const std::string theName)
{
  NCollection_DataMap<TDF_Label, FeaturePtr>::Iterator anObjIter(myObjs);
  for(; anObjIter.More(); anObjIter.Next()) {
    FeaturePtr& aFeature = anObjIter.ChangeValue();
    if (!aFeature) // may be on close
      continue;
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aFeature->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRIter = aResults.begin();
    for (; aRIter != aResults.cend(); aRIter++) {
      if (aRIter->get() && (*aRIter)->data() && (*aRIter)->data()->isValid() &&
          (*aRIter)->data()->name() == theName) {
        return *aRIter;
      }
    }
  }
  // not found
  return ResultPtr();
}
