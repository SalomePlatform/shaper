// File:	Model_Application.cxx
// Created:	Fri Sep 2 2011
// Author:	Mikhail PONIKAROV

#include <Model_Application.h>
#include <Model_Document.h>

IMPLEMENT_STANDARD_HANDLE(Model_Application, TDocStd_Application)
IMPLEMENT_STANDARD_RTTIEXT(Model_Application, TDocStd_Application)

static Handle_Model_Application TheApplication = new Model_Application;

//=======================================================================
//function : getApplication
//purpose  : 
//=======================================================================
Handle(Model_Application) Model_Application::getApplication()
{
  return TheApplication;
}

//=======================================================================
//function : getDocument
//purpose  : 
//=======================================================================
std::shared_ptr<Model_Document> Model_Application::getDocument(std::string theDocID)
{
  if (myDocs.find(theDocID) != myDocs.end())
    return myDocs[theDocID];

  std::shared_ptr<Model_Document> aNew(new Model_Document);
  myDocs[theDocID] = aNew;
  return aNew;
}

//=======================================================================
//function : OCAFApp_Application
//purpose  : 
//=======================================================================
Model_Application::Model_Application()
{
  // store handle to the application to avoid nullification
  static Handle(Model_Application) TheKeepHandle;
  TheKeepHandle = this;
}

//=======================================================================
//function : Formats
//purpose  : 
//=======================================================================
void Model_Application::Formats(TColStd_SequenceOfExtendedString& theFormats)
{
  theFormats.Append(TCollection_ExtendedString("BinOcaf")); // standard binary schema
}

//=======================================================================
//function : ResourcesName
//purpose  : 
//=======================================================================
Standard_CString Model_Application::ResourcesName()
{
  return Standard_CString("Standard");
}
