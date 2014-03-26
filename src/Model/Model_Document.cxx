// File:        Model_Document.cxx
// Created:     28 Dec 2011
// Author:      Mikhail PONIKAROV
// Copyright:   CEA 2011

#include <Model_Document.hxx>

#include <TDataStd_Integer.hxx>

IMPLEMENT_STANDARD_HANDLE(Model_Document,MMgt_TShared)
IMPLEMENT_STANDARD_RTTIEXT(Model_Document,MMgt_TShared)

static const int UNDO_LIMIT = 10; // number of possible undo operations in the module

static const int TAG_GENERAL = 1; // general properties tag
static const int TAG_OBJECTS = 2; // tag of the objects sub-tree (Root for Model_ObjectsMgr)
static const int TAG_HISTORY = 3; // tag of the history sub-tree (Root for Model_History)
static const int TAG_ISOTOPES = 4; // tag of the isotopes sub-tree (Root for MaterialMC_Isotope)

using namespace std;

bool Model_Document::Load(const char* theFileName)
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

bool Model_Document::Save(const char* theFileName)
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

void Model_Document::Close()
{
  TDocStd_Document::Close();
}

void Model_Document::StartOperation()
{
  TDocStd_Document::NewCommand();
}

void Model_Document::FinishOperation()
{
  TDocStd_Document::CommitCommand();
  myTransactionsAfterSave++;
}

void Model_Document::AbortOperation()
{
  TDocStd_Document::AbortCommand();
}

bool Model_Document::IsOperation()
{
  return TDocStd_Document::HasOpenCommand() == Standard_True;
}

bool Model_Document::IsModified()
{
  return myTransactionsAfterSave != 0;
}

bool Model_Document::CanUndo()
{
  return TDocStd_Document::GetAvailableUndos() > 0;
}

void Model_Document::Undo()
{
  TDocStd_Document::Undo();
  myTransactionsAfterSave--;
}

bool Model_Document::CanRedo()
{
  return TDocStd_Document::GetAvailableRedos() > 0;
}

void Model_Document::Redo()
{
  TDocStd_Document::Redo();
  myTransactionsAfterSave++;
}

Model_Document::Model_Document(const TCollection_ExtendedString& theStorageFormat)
  : TDocStd_Document(theStorageFormat)
{
  SetUndoLimit(UNDO_LIMIT);
  myTransactionsAfterSave = 0;
}

Model_Document::~Model_Document()
{
}
