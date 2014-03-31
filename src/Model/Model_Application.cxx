// File:	Model_Application.cxx
// Created:	Fri Sep 2 2011
// Author:	Mikhail PONIKAROV

#include <Model_Application.hxx>
#include <Model_Document.hxx>

IMPLEMENT_STANDARD_HANDLE(Model_Application, TDocStd_Application)
IMPLEMENT_STANDARD_RTTIEXT(Model_Application, TDocStd_Application)

static Handle_Model_Application TheApplication = new Model_Application;

//=======================================================================
//function : getApplication
//purpose  : 
//=======================================================================
Handle_Model_Application Model_Application::GetApplication()
{
  return TheApplication;
}

//=======================================================================
//function : getDocument
//purpose  : 
//=======================================================================
ModelAPI_Document* Model_Application::GetMainDocument()
{

  if (myMainDoc.IsNull()) {
    myMainDoc = new Model_Document("BinOcaf");
  }
  return *myMainDoc;
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
