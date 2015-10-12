// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#include <Model_Document.h>
#include <Model_Data.h>
#include <Model_Objects.h>
#include <Model_Application.h>
#include <Model_Session.h>
#include <Model_Events.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_Validator.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_AttributeSelectionList.h>
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
#include <TDF_Delta.hxx>
#include <OSD_File.hxx>
#include <OSD_Path.hxx>
#include <TDF_AttributeDelta.hxx>
#include <TDF_AttributeDeltaList.hxx>
#include <TDF_ListIteratorOfAttributeDeltaList.hxx>
#include <TDF_ListIteratorOfLabelList.hxx>

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

// general sub-labels
static const int TAG_CURRENT_FEATURE = 1; ///< where the reference to the current feature label is located (or no attribute if null feature)
static const int TAG_CURRENT_TRANSACTION = 2; ///< integer, index of the cransaction
static const int TAG_SELECTION_FEATURE = 3; ///< integer, tag of the selection feature label

Model_Document::Model_Document(const std::string theID, const std::string theKind)
    : myID(theID), myKind(theKind), myIsActive(false),
      myDoc(new TDocStd_Document("BinOcaf"))  // binary OCAF format
{
  myObjs = new Model_Objects(myDoc->Main());
  myDoc->SetUndoLimit(UNDO_LIMIT);  
  myTransactionSave = 0;
  myExecuteFeatures = true;
  // to have something in the document and avoid empty doc open/save problem
  // in transaction for nesting correct working
  myDoc->NewCommand();
  TDataStd_Integer::Set(myDoc->Main().Father(), 0);
  // this to avoid creation of integer attribute outside the transaction after undo
  transactionID();
  myDoc->CommitCommand();
}

void Model_Document::setThis(DocumentPtr theDoc)
{
  myObjs->setOwner(theDoc);
}

/// Returns the file name of this document by the nameof directory and identifuer of a document
static TCollection_ExtendedString DocFileName(const char* theFileName, const std::string& theID)
{
  TCollection_ExtendedString aPath((const Standard_CString) theFileName);
  // remove end-separators
  while(aPath.Length() && 
        (aPath.Value(aPath.Length()) == '\\' || aPath.Value(aPath.Length()) == '/'))
    aPath.Remove(aPath.Length());
  aPath += _separator_;
  aPath += theID.c_str();
  aPath += ".cbf";  // standard binary file extension
  return aPath;
}

bool Model_Document::isRoot() const
{
  return this == Model_Session::get()->moduleDocument().get();
}

bool Model_Document::load(const char* theFileName, DocumentPtr theThis)
{
  Handle(Model_Application) anApp = Model_Application::getApplication();
  if (isRoot()) {
    anApp->setLoadPath(theFileName);
  }
  TCollection_ExtendedString aPath(DocFileName(theFileName, myID));
  PCDM_ReaderStatus aStatus = (PCDM_ReaderStatus) -1;
  Handle(TDocStd_Document) aLoaded;
  try {
    aStatus = anApp->Open(aPath, aLoaded);
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
  std::shared_ptr<Model_Session> aSession = 
    std::dynamic_pointer_cast<Model_Session>(Model_Session::get());
  if (!isError) {
    myDoc = aLoaded;
    myDoc->SetUndoLimit(UNDO_LIMIT);
    // to avoid the problem that feature is created in the current, not this, document
    aSession->setActiveDocument(anApp->getDocument(myID), false);
    aSession->setCheckTransactions(false);
    if (myObjs)
      delete myObjs;
    myObjs = new Model_Objects(myDoc->Main()); // synchronisation is inside
    myObjs->setOwner(theThis);
    // update the current features status
    setCurrentFeature(currentFeature(false), false);
    aSession->setCheckTransactions(true);
    aSession->setActiveDocument(Model_Session::get()->moduleDocument(), false);
    // this is done in Part result "activate", so no needed here. Causes not-blue active part.
    // aSession->setActiveDocument(anApp->getDocument(myID), true);
  } else { // open failed, but new documnet was created to work with it: inform the model
    aSession->setActiveDocument(Model_Session::get()->moduleDocument(), false);
  } 
  return !isError;
}

bool Model_Document::save(const char* theFileName, std::list<std::string>& theResults)
{
  // create a directory in the root document if it is not yet exist
  Handle(Model_Application) anApp = Model_Application::getApplication();
  if (isRoot()) {
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
  if (!isRoot() && this == aPM->activeDocument().get()) {
    aPM->setActiveDocument(aPM->moduleDocument());
  } else if (isRoot()) {
    // erase the active document if root is closed
    aPM->setActiveDocument(DocumentPtr());
  }
  // close all subs
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++) {
    std::shared_ptr<Model_Document> aSub = subDoc(*aSubIter);
    if (aSub->myObjs) // if it was not closed before
      aSub->close(theForever);
  }

  // close for thid document needs no transaction in this document
  std::static_pointer_cast<Model_Session>(Model_Session::get())->setCheckTransactions(false);

  // close all only if it is really asked, otherwise it can be undoed/redoed
  if (theForever) {
    // flush everything to avoid messages with bad objects
    delete myObjs;
    myObjs = 0;
    if (myDoc->CanClose() == CDM_CCS_OK)
      myDoc->Close();
    mySelectionFeature.reset();
  } else {
    setCurrentFeature(FeaturePtr(), false); // disables all features
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
  incrementTransactionID();
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
  // do it before flashes to enable and recompute nesting features correctly
  if (myNestedNum.empty() || (isNestedClosed && myNestedNum.size() == 1)) {
    // if all nested operations are closed, make current the higher level objects (to perform 
    // it in the python scripts correctly): sketch become current after creation ofsub-elements
    FeaturePtr aCurrent = currentFeature(false);
    CompositeFeaturePtr aMain, aNext = ModelAPI_Tools::compositeOwner(aCurrent);
    while(aNext.get()) {
      aMain = aNext;
      aNext = ModelAPI_Tools::compositeOwner(aMain);
    }
    if (aMain.get() && aMain != aCurrent)
      setCurrentFeature(aMain, false);
  }
  myObjs->synchronizeBackRefs();
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  aLoop->flush(Events_Loop::eventByName(EVENT_OBJECT_DELETED));
  // this must be here just after everything is finished but before real transaction stop
  // to avoid messages about modifications outside of the transaction
  // and to rebuild everything after all updates and creates
  if (isRoot()) { // once for root document
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

  if (!aResult && isRoot()) {
    // nothing inside in all documents, so remove this transaction from the transactions list
    undoInternal(true, false);
  }
  // on finish clear redos in any case (issue 446) and for all subs (issue 408)
  myDoc->ClearRedos();
  myRedos.clear();
  for (aSubIter = aSubs.begin(); aSubIter != aSubs.end(); aSubIter++) {
    subDoc(*aSubIter)->myDoc->ClearRedos();
    subDoc(*aSubIter)->myRedos.clear();
  }

  return aResult;
}

/// Returns in theDelta labels that has been modified in the latest transaction of theDoc
static void modifiedLabels(const Handle(TDocStd_Document)& theDoc, TDF_LabelList& theDelta,
  const bool isRedo = false) {
  Handle(TDF_Delta) aDelta;
  if (isRedo)
    aDelta = theDoc->GetRedos().First();
  else 
    aDelta = theDoc->GetUndos().Last();
  TDF_LabelList aDeltaList;
  aDelta->Labels(aDeltaList); // it clears list, so, use new one and then append to the result
  for(TDF_ListIteratorOfLabelList aListIter(aDeltaList); aListIter.More(); aListIter.Next()) {
    theDelta.Append(aListIter.Value());
  }
  // add also label of the modified attributes
  const TDF_AttributeDeltaList& anAttrs = aDelta->AttributeDeltas();
  for (TDF_ListIteratorOfAttributeDeltaList anAttr(anAttrs); anAttr.More(); anAttr.Next()) {
    theDelta.Append(anAttr.Value()->Label());
  }
}

void Model_Document::abortOperation()
{
  TDF_LabelList aDeltaLabels; // labels that are updated during "abort"
  if (!myNestedNum.empty() && !myDoc->HasOpenCommand()) {  // abort all what was done in nested
    compactNested();
    // store undo-delta here as undo actually does in the method later
    int a, aNumTransactions = myTransactions.rbegin()->myOCAFNum;
    for(a = 0; a < aNumTransactions; a++) {
      modifiedLabels(myDoc, aDeltaLabels);
      myDoc->Undo();
    }
    for(a = 0; a < aNumTransactions; a++) {
      myDoc->Redo();
    }

    undoInternal(false, false);
    myDoc->ClearRedos();
    myRedos.clear();
  } else { // abort the current
    int aNumTransactions = myTransactions.rbegin()->myOCAFNum;
    myTransactions.pop_back();
    if (!myNestedNum.empty())
      (*myNestedNum.rbegin())--;
    // roll back the needed number of transactions
    // make commit/undo to get the modification delta
    //myDoc->AbortCommand();
    if (myDoc->CommitCommand()) {
      modifiedLabels(myDoc, aDeltaLabels);
      myDoc->Undo();
    }
    for(int a = 0; a < aNumTransactions; a++) {
      modifiedLabels(myDoc, aDeltaLabels);
      myDoc->Undo();
    }
    myDoc->ClearRedos();
  }
  // abort for all subs, flushes will be later, in the end of root abort
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    subDoc(*aSubIter)->abortOperation();
  // references may be changed because they are set in attributes on the fly
  myObjs->synchronizeFeatures(aDeltaLabels, true, isRoot());
}

bool Model_Document::isOperation() const
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
  // issue 406 : if transaction is opened, but nothing to undo behind, can not undo
  int aCurrentNum = isOperation() ? 1 : 0;
  if (myDoc->GetAvailableUndos() > 0 && 
      (myNestedNum.empty() || *myNestedNum.rbegin() - aCurrentNum > 0) && // there is something to undo in nested
      myTransactions.size() - aCurrentNum > 0 /* for omitting the first useless transaction */)
    return true;
  // check other subs contains operation that can be undoed
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++) {
    std::shared_ptr<Model_Document> aSub = subDoc(*aSubIter);
    if (aSub->myObjs) {// if it was not closed before
      if (aSub->canUndo())
        return true;
    }
  }

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
  TDF_LabelList aDeltaLabels;
  for(int a = 0; a < aNumTransactions; a++) {
    if (theSynchronize)
      modifiedLabels(myDoc, aDeltaLabels);
    myDoc->Undo();
  }

  if (theWithSubs) {
    // undo for all subs
    const std::set<std::string> aSubs = subDocuments(true);
    std::set<std::string>::iterator aSubIter = aSubs.begin();
    for (; aSubIter != aSubs.end(); aSubIter++) {
      if (!subDoc(*aSubIter)->myObjs)
        continue;
      subDoc(*aSubIter)->undoInternal(theWithSubs, theSynchronize);
    }
  }
  // after undo of all sub-documents to avoid updates on not-modified data (issue 370)
  if (theSynchronize) {
    myObjs->synchronizeFeatures(aDeltaLabels, true, isRoot());
    // update the current features status
    setCurrentFeature(currentFeature(false), false);
  }
}

void Model_Document::undo()
{
  undoInternal(true, true);
}

bool Model_Document::canRedo()
{
  if (!myRedos.empty())
    return true;
  // check other subs contains operation that can be redoed
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++) {
    if (!subDoc(*aSubIter)->myObjs)
      continue;
    if (subDoc(*aSubIter)->canRedo())
      return true;
  }
  return false;
}

void Model_Document::redo()
{
  if (!myNestedNum.empty())
    (*myNestedNum.rbegin())++;
  int aNumRedos = myRedos.rbegin()->myOCAFNum;
  myTransactions.push_back(*myRedos.rbegin());
  myRedos.pop_back();
  TDF_LabelList aDeltaLabels;
  for(int a = 0; a < aNumRedos; a++) {
    modifiedLabels(myDoc, aDeltaLabels, true);
    myDoc->Redo();
  }

  // redo for all subs
  const std::set<std::string> aSubs = subDocuments(true);
  std::set<std::string>::iterator aSubIter = aSubs.begin();
  for (; aSubIter != aSubs.end(); aSubIter++)
    subDoc(*aSubIter)->redo();

  // after redo of all sub-documents to avoid updates on not-modified data (issue 370)
  myObjs->synchronizeFeatures(aDeltaLabels, true, isRoot());
  // update the current features status
  setCurrentFeature(currentFeature(false), false);
}

std::list<std::string> Model_Document::undoList() const
{
  std::list<std::string> aResult;
  // the number of skipped current operations (on undo they will be aborted)
  int aSkipCurrent = isOperation() ? 1 : 0;
  std::list<Transaction>::const_reverse_iterator aTrIter = myTransactions.crbegin();
  int aNumUndo = myTransactions.size();
  if (!myNestedNum.empty())
    aNumUndo = *myNestedNum.rbegin();
  for( ; aNumUndo > 0; aTrIter++, aNumUndo--) {
    if (aSkipCurrent == 0) aResult.push_back(aTrIter->myId);
    else aSkipCurrent--;
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

FeaturePtr Model_Document::addFeature(std::string theID, const bool theMakeCurrent)
{
  std::shared_ptr<Model_Session> aSession = 
    std::dynamic_pointer_cast<Model_Session>(ModelAPI_Session::get());
  FeaturePtr aFeature = aSession->createFeature(theID, this);
  if (!aFeature)
    return aFeature;
  aFeature->init();
  Model_Document* aDocToAdd;
  if (!aFeature->documentToAdd().empty()) { // use the customized document to add
    if (aFeature->documentToAdd() != kind()) { // the root document by default
      aDocToAdd = std::dynamic_pointer_cast<Model_Document>(aSession->moduleDocument()).get();
    } else {
      aDocToAdd = this;
    }
  } else { // if customized is not presented, add to "this" document
    aDocToAdd = this;
  }
  if (aFeature) {
    // searching for feature after which must be added the next feature: this is the current feature
    // but also all sub-features of this feature
    FeaturePtr aCurrent = aDocToAdd->currentFeature(false);
    bool isModified = true;
    for(CompositeFeaturePtr aComp = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCurrent);
        aComp.get() && isModified; 
        aComp = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aCurrent)) {
      isModified =  false;
      int aSubs = aComp->numberOfSubs(false);
      for(int a = 0; a < aSubs; a++) {
        FeaturePtr aSub = aComp->subFeature(a, false);
        if (myObjs->isLater(aSub, aCurrent)) {
          isModified =  true;
          aCurrent = aSub;
        }
      }
    }
    aDocToAdd->myObjs->addFeature(aFeature, aCurrent);
    if (!aFeature->isAction()) {  // do not add action to the data model
      if (theMakeCurrent)  // after all this feature stays in the document, so make it current
        aDocToAdd->setCurrentFeature(aFeature, false);
    } else { // feature must be executed
       // no creation event => updater not working, problem with remove part
      aFeature->execute();
    }
  }
  return aFeature;
}


void Model_Document::refsToFeature(FeaturePtr theFeature,
  std::set<std::shared_ptr<ModelAPI_Feature> >& theRefs, const bool isSendError)
{
  myObjs->refsToFeature(theFeature, theRefs, isSendError);
}

void Model_Document::removeFeature(FeaturePtr theFeature)
{
  myObjs->removeFeature(theFeature);
}

void Model_Document::moveFeature(FeaturePtr theMoved, FeaturePtr theAfterThis)
{
  myObjs->moveFeature(theMoved, theAfterThis);
  if (theAfterThis == currentFeature(true))
    setCurrentFeature(theMoved, true);
}

void Model_Document::updateHistory(const std::shared_ptr<ModelAPI_Object> theObject)
{
  myObjs->updateHistory(theObject);
}

void Model_Document::updateHistory(const std::string theGroup)
{
  myObjs->updateHistory(theGroup);
}

std::shared_ptr<ModelAPI_Document> Model_Document::subDocument(std::string theDocID)
{
  return Model_Application::getApplication()->getDocument(theDocID);
}

const std::set<std::string> Model_Document::subDocuments(const bool theActivatedOnly) const
{
  std::set<std::string> aResult;
  std::list<ResultPtr> aPartResults;
  myObjs->allResults(ModelAPI_ResultPart::group(), aPartResults);
  std::list<ResultPtr>::iterator aPartRes = aPartResults.begin();
  for(; aPartRes != aPartResults.end(); aPartRes++) {
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(*aPartRes);
    if (aPart && (!theActivatedOnly || aPart->isActivated())) {
      aResult.insert(aPart->original()->data()->name());
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

ObjectPtr Model_Document::object(const std::string& theGroupID, const int theIndex)
{
  return myObjs->object(theGroupID, theIndex);
}

std::shared_ptr<ModelAPI_Object> Model_Document::objectByName(
    const std::string& theGroupID, const std::string& theName)
{
  return myObjs->objectByName(theGroupID, theName);
}

const int Model_Document::index(std::shared_ptr<ModelAPI_Object> theObject)
{
  return myObjs->index(theObject);
}

int Model_Document::size(const std::string& theGroupID)
{
  return myObjs->size(theGroupID);
}

std::shared_ptr<ModelAPI_Feature> Model_Document::currentFeature(const bool theVisible)
{
  if (!myObjs) // on close document feature destruction it may call this method
    return std::shared_ptr<ModelAPI_Feature>();
  TDF_Label aRefLab = generalLabel().FindChild(TAG_CURRENT_FEATURE);
  Handle(TDF_Reference) aRef;
  if (aRefLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
    TDF_Label aLab = aRef->Get();
    FeaturePtr aResult = myObjs->feature(aLab);
    if (theVisible) { // get nearest visible (in history) going up
      while(aResult.get() &&  !aResult->isInHistory()) {
        aResult = myObjs->nextFeature(aResult, true);
      }
    }
    return aResult;
  }
  return std::shared_ptr<ModelAPI_Feature>(); // null feature means the higher than first
}

void Model_Document::setCurrentFeature(
  std::shared_ptr<ModelAPI_Feature> theCurrent, const bool theVisible)
{
  // blocks the flush signals to avoid each objects visualization in the viewer
  // they should not be shown once after all modifications are performed
  Events_Loop* aLoop = Events_Loop::loop();
  bool isActive = aLoop->activateFlushes(false);

  TDF_Label aRefLab = generalLabel().FindChild(TAG_CURRENT_FEATURE);
  CompositeFeaturePtr aMain; // main feature that may nest the new current
  std::set<FeaturePtr> anOwners; // composites that contain theCurrent (with any level of nesting)
  if (theCurrent.get()) {
    aMain = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(theCurrent);
    CompositeFeaturePtr anOwner = ModelAPI_Tools::compositeOwner(theCurrent);
    while(anOwner.get()) {
      if (!aMain.get()) {
        aMain = anOwner;
      }
      anOwners.insert(anOwner);
      anOwner = ModelAPI_Tools::compositeOwner(anOwner);
    }
  }

  if (theVisible) { // make features below which are not in history also enabled: sketch subs
    FeaturePtr aNext = 
      theCurrent.get() ? myObjs->nextFeature(theCurrent) : myObjs->firstFeature();
    for (; aNext.get(); aNext = myObjs->nextFeature(theCurrent)) {
      if (aNext->isInHistory()) {
        break; // next in history is not needed
      } else { // next not in history is good for making current
        theCurrent = aNext;
      }
    }
  }
  if (theCurrent.get()) {
    std::shared_ptr<Model_Data> aData = std::static_pointer_cast<Model_Data>(theCurrent->data());
    if (!aData.get() || !aData->isValid()) {
      aLoop->activateFlushes(isActive);
      return;
    }
    TDF_Label aFeatureLabel = aData->label().Father();

    Handle(TDF_Reference) aRef;
    if (aRefLab.FindAttribute(TDF_Reference::GetID(), aRef)) {
      aRef->Set(aFeatureLabel);
    } else {
      aRef = TDF_Reference::Set(aRefLab, aFeatureLabel);
    }
  } else { // remove reference for the null feature
    aRefLab.ForgetAttribute(TDF_Reference::GetID());
  }
  // make all features after this feature disabled in reversed order (to remove results without deps)
  static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);
  static Events_ID aCreateEvent = Events_Loop::eventByName(EVENT_OBJECT_CREATED);
  static Events_ID aDeleteEvent = aLoop->eventByName(EVENT_OBJECT_DELETED);

  bool aPassed = false; // flag that the current object is already passed in cycle
  FeaturePtr anIter = myObjs->lastFeature();
  bool aWasChanged = false;
  for(; anIter.get(); anIter = myObjs->nextFeature(anIter, true)) {
    // check this before passed become enabled: the current feature is enabled!
    if (anIter == theCurrent) aPassed = true;

    bool aDisabledFlag = !aPassed;
    if (aMain.get()) {
      if (aMain->isSub(anIter)) // sub-elements of not-disabled feature are not disabled
        aDisabledFlag = false;
      else if (anOwners.find(anIter) != anOwners.end()) // disable the higher-level feature is the nested is the current
        aDisabledFlag = true;
    }

    if (anIter->getKind() == "Parameter") {// parameters are always out of the history of features, but not parameters
      if (theCurrent.get() && theCurrent->getKind() != "Parameter")
        aDisabledFlag = false;
    }
    if (anIter->setDisabled(aDisabledFlag)) {
      // state of feature is changed => so feature become updated
      static Events_ID anUpdateEvent = aLoop->eventByName(EVENT_OBJECT_UPDATED);
      ModelAPI_EventCreator::get()->sendUpdated(anIter, anUpdateEvent);
      // flush is in the end of this method
      ModelAPI_EventCreator::get()->sendUpdated(anIter, aRedispEvent /*, false*/);
      aWasChanged = true;
    }
    // update for everyone the concealment flag immideately: on edit feature in the midle of history
    if (aWasChanged) {
      const std::list<std::shared_ptr<ModelAPI_Result> >& aResList = anIter->results();
      std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResList.begin();
      for(; aRes != aResList.end(); aRes++) {
        if ((*aRes).get() && (*aRes)->data()->isValid() && !(*aRes)->isDisabled())
          std::dynamic_pointer_cast<Model_Data>((*aRes)->data())->updateConcealmentFlag();
      }

    }
  }
  // unblock  the flush signals and up them after this
  aLoop->activateFlushes(isActive);
}

void Model_Document::setCurrentFeatureUp()
{
  // on remove just go up for minimum step: highlight external objects in sketch causes 
  // problems if it is true: here and in "setCurrentFeature"
  FeaturePtr aCurrent = currentFeature(false);
  if (aCurrent.get()) { // if not, do nothing because null is the upper
    FeaturePtr aPrev = myObjs->nextFeature(aCurrent, true);
    // do not flush: it is called only on remove, it will be flushed in the end of transaction
    setCurrentFeature(aPrev, false);
  }
}

TDF_Label Model_Document::generalLabel() const
{
  return myDoc->Main().FindChild(TAG_GENERAL);
}

std::shared_ptr<ModelAPI_ResultConstruction> Model_Document::createConstruction(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createConstruction(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultBody> Model_Document::createBody(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createBody(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultPart> Model_Document::createPart(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createPart(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultPart> Model_Document::copyPart(
      const std::shared_ptr<ModelAPI_ResultPart>& theOrigin,
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->copyPart(theOrigin, theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultGroup> Model_Document::createGroup(
    const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createGroup(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_ResultParameter> Model_Document::createParameter(
      const std::shared_ptr<ModelAPI_Data>& theFeatureData, const int theIndex)
{
  return myObjs->createParameter(theFeatureData, theIndex);
}

std::shared_ptr<ModelAPI_Feature> Model_Document::feature(
    const std::shared_ptr<ModelAPI_Result>& theResult)
{
  return myObjs->feature(theResult);
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
  return myObjs->findByName(theName);
}

std::list<std::shared_ptr<ModelAPI_Feature> > Model_Document::allFeatures()
{
  return myObjs->allFeatures();
}

void Model_Document::setActive(const bool theFlag)
{
  if (theFlag != myIsActive) {
    myIsActive = theFlag;
    // redisplay all the objects of this part
    static Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID aRedispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);

    for(int a = size(ModelAPI_Feature::group()) - 1; a >= 0; a--) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(
        object(ModelAPI_Feature::group(), a));
      if (aFeature.get() && aFeature->data()->isValid()) {
        const std::list<std::shared_ptr<ModelAPI_Result> >& aResList = aFeature->results();
        std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResList.begin();
        for(; aRes != aResList.end(); aRes++) {
          ModelAPI_EventCreator::get()->sendUpdated(*aRes, aRedispEvent);
          // #issue 1048: sub-compsolids also
          ResultCompSolidPtr aCompRes = std::dynamic_pointer_cast<ModelAPI_ResultCompSolid>(*aRes);
          if (aCompRes.get()) {
            int aNumSubs = aCompRes->numberOfSubs();
            for(int a = 0; a < aNumSubs; a++) {
              ResultPtr aSub = aCompRes->subResult(a);
              if (aSub.get()) {
                ModelAPI_EventCreator::get()->sendUpdated(aSub, aRedispEvent);
              }
            }
          }
        }
      }
    }
  }
}

bool Model_Document::isActive() const
{
  return myIsActive;
}

int Model_Document::transactionID()
{
  Handle(TDataStd_Integer) anIndex;
  if (!generalLabel().FindChild(TAG_CURRENT_TRANSACTION).
      FindAttribute(TDataStd_Integer::GetID(), anIndex)) {
    anIndex = TDataStd_Integer::Set(generalLabel().FindChild(TAG_CURRENT_TRANSACTION), 1);
  }
  return anIndex->Get();
}

void Model_Document::incrementTransactionID()
{
  int aNewVal = transactionID() + 1;
  TDataStd_Integer::Set(generalLabel().FindChild(TAG_CURRENT_TRANSACTION), aNewVal);
}
void Model_Document::decrementTransactionID()
{
  int aNewVal = transactionID() - 1;
  TDataStd_Integer::Set(generalLabel().FindChild(TAG_CURRENT_TRANSACTION), aNewVal);
}

bool Model_Document::isOpened()
{
  return myObjs && !myDoc.IsNull();
}

int Model_Document::numInternalFeatures()
{
  return myObjs->numInternalFeatures();
}

std::shared_ptr<ModelAPI_Feature> Model_Document::internalFeature(const int theIndex)
{
  return myObjs->internalFeature(theIndex);
}

void Model_Document::synchronizeTransactions()
{
  Model_Document* aRoot = 
    std::dynamic_pointer_cast<Model_Document>(ModelAPI_Session::get()->moduleDocument()).get();
  if (aRoot == this)
    return; // don't need to synchronise root with root

  std::shared_ptr<Model_Session> aSession = 
    std::dynamic_pointer_cast<Model_Session>(Model_Session::get());
  while(myRedos.size() > aRoot->myRedos.size()) { // remove redos in this
    aSession->setCheckTransactions(false);
    redo();
    aSession->setCheckTransactions(true);
  }
  /* this case can not be reproduced in any known case for the current moment, so, just comment
  while(myRedos.size() < aRoot->myRedos.size()) { // add more redos in this
    undoInternal(false, true);
  }*/
}

/// Feature that is used for selection in the Part document by the external request
class Model_SelectionInPartFeature : public ModelAPI_Feature {
public:
  /// Nothing to do in constructor
  Model_SelectionInPartFeature() : ModelAPI_Feature() {}

  /// Returns the unique kind of a feature
  virtual const std::string& getKind() {
    static std::string MY_KIND("InternalSelectionInPartFeature");
    return MY_KIND;
  }
  /// Request for initialization of data model of the object: adding all attributes
  virtual void initAttributes() {
    data()->addAttribute("selection", ModelAPI_AttributeSelectionList::typeId());
  }
  /// Nothing to do in the execution function
  virtual void execute() {}

};

//! Returns the feature that is used for calculation of selection externally from the document
AttributeSelectionListPtr Model_Document::selectionInPartFeature()
{
  // return already created, otherwise create
  if (!mySelectionFeature.get() || !mySelectionFeature->data()->isValid()) {
    // create a new one
    mySelectionFeature = FeaturePtr(new Model_SelectionInPartFeature);
  
    TDF_Label aFeatureLab = generalLabel().FindChild(TAG_SELECTION_FEATURE);
    std::shared_ptr<Model_Data> aData(new Model_Data);
    aData->setLabel(aFeatureLab.FindChild(1));
    aData->setObject(mySelectionFeature);
    mySelectionFeature->setDoc(myObjs->owner());
    mySelectionFeature->setData(aData);
    std::string aName = id() + "_Part";
    mySelectionFeature->data()->setName(aName);
    mySelectionFeature->setDoc(myObjs->owner());
    mySelectionFeature->initAttributes();
  }
  return mySelectionFeature->selectionList("selection");
}

FeaturePtr Model_Document::lastFeature()
{
  if (myObjs)
    return myObjs->lastFeature();
  return FeaturePtr();
}
