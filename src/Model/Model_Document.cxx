// File:        Model_Document.cxx
// Created:     28 Feb 2014
// Author:      Mikhail PONIKAROV

#include <Model_Document.h>
#include <ModelAPI_Feature.h>
#include <Model_Object.h>
#include <Model_Application.h>
#include <Model_PluginManager.h>
#include <Model_Iterator.h>
#include <Event_Loop.h>

#include <TDataStd_Integer.hxx>
#include <TDataStd_Comment.hxx>

static const int UNDO_LIMIT = 10; // number of possible undo operations

static const int TAG_GENERAL = 1; // general properties tag
static const int TAG_OBJECTS = 2; // tag of the objects sub-tree (Root for Model_ObjectsMgr)
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
  myDoc->Close();
}

void Model_Document::startOperation()
{
  myDoc->NewCommand();
}

void Model_Document::finishOperation()
{
  myDoc->CommitCommand();
  myTransactionsAfterSave++;
}

void Model_Document::abortOperation()
{
  myDoc->AbortCommand();
}

bool Model_Document::isOperation()
{
  return myDoc->HasOpenCommand() == Standard_True ;
}

bool Model_Document::isModified()
{
  return myTransactionsAfterSave != 0;
}

bool Model_Document::canUndo()
{
  return myDoc->GetAvailableUndos() > 0;
}

void Model_Document::undo()
{
  myDoc->Undo();
  myTransactionsAfterSave--;
}

bool Model_Document::canRedo()
{
  return myDoc->GetAvailableRedos() > 0;
}

void Model_Document::redo()
{
  myDoc->Redo();
  myTransactionsAfterSave++;
}

void Model_Document::addFeature(
  std::shared_ptr<ModelAPI_Feature> theFeature, const std::string theGroupID)
{
  TDF_Label aGroupLab = groupLabel(theGroupID);
  TDF_Label anObjLab = aGroupLab.NewChild();
  std::shared_ptr<Model_Object> aData(new Model_Object);
  aData->setLabel(anObjLab);
  theFeature->setData(aData);
  setUniqueName(theFeature, theGroupID);
  theFeature->initAttributes();
  TDataStd_Comment::Set(anObjLab, theFeature->getKind().c_str());

  // event: model is updated
  static Event_ID anEvent = Event_Loop::eventByName(EVENT_MODEL_UPDATED);
  Event_Message anUpdateMsg(anEvent, this);
  Event_Loop::loop()->send(anUpdateMsg);

}

std::shared_ptr<ModelAPI_Feature> Model_Document::feature(TDF_Label& theLabel)
{
  Handle(TDataStd_Comment) aFeatureID;
  if (theLabel.FindAttribute(TDataStd_Comment::GetID(), aFeatureID)) {
    string anID(TCollection_AsciiString(aFeatureID->Get()).ToCString());
    std::shared_ptr<ModelAPI_Feature> aResult = 
      Model_PluginManager::get()->createFeature(anID, false);
    std::shared_ptr<Model_Object> aData(new Model_Object);
    aData->setLabel(theLabel);
    aResult->setData(aData);
    aResult->initAttributes();
    return aResult;
  }
  return std::shared_ptr<ModelAPI_Feature>(); // not found
}

shared_ptr<ModelAPI_Document> Model_Document::subDocument(string theDocID)
{
  return Model_Application::getApplication()->getDocument(theDocID);
}

shared_ptr<ModelAPI_Iterator> Model_Document::featuresIterator(const string theGroup)
{
  shared_ptr<Model_Document> aThis(Model_Application::getApplication()->getDocument(myID));
  return shared_ptr<ModelAPI_Iterator>(new Model_Iterator(aThis, groupLabel(theGroup)));
}

shared_ptr<ModelAPI_Feature> Model_Document::feature(const string& theGroupID, const int theIndex)
{
  // TODO: optimize this method
  shared_ptr<ModelAPI_Iterator>  anIter = featuresIterator(theGroupID);
  for(int a = 0; a != theIndex; anIter->next()) a++;
  return anIter->current();
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
}

TDF_Label Model_Document::groupLabel(const string theGroup)
{
  if (myGroups.find(theGroup) == myGroups.end()) {
    myGroups[theGroup] = myDoc->Main().FindChild(TAG_OBJECTS).NewChild();
    myGroupsNames.push_back(theGroup);
  }
  return myGroups[theGroup];
}

void Model_Document::setUniqueName(
  shared_ptr<ModelAPI_Feature> theFeature, const string theGroupID)
{
  // first count all objects of such kind to start with index = count + 1
  int aNumObjects = 0;
  shared_ptr<ModelAPI_Iterator> anIter = featuresIterator(theGroupID);
  for(; anIter->more(); anIter->next()) {
    if (anIter->currentKind() == theFeature->getKind())
      aNumObjects++;
  }
  // generate candidate name
  stringstream aNameStream;
  aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
  string aName = aNameStream.str();
  // check this is unique, if not, increase index by 1
  for(anIter = featuresIterator(theGroupID); anIter->more();) {
    if (anIter->currentName() == aName) {
      aNumObjects++;
      stringstream aNameStream;
      aNameStream<<theFeature->getKind()<<"_"<<aNumObjects + 1;
      // reinitialize iterator to make sure a new name is unique
      anIter = featuresIterator(theGroupID);
    } else anIter->next();
  }

  theFeature->data()->setName(aName);
}
