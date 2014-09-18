// File:	Model_Application.cxx
// Created:	Fri Sep 2 2011
// Author:	Mikhail PONIKAROV

#include <Model_Application.h>
#include <Model_Document.h>

IMPLEMENT_STANDARD_HANDLE(Model_Application, TDocStd_Application)
IMPLEMENT_STANDARD_RTTIEXT(Model_Application, TDocStd_Application)

using namespace std;

static Handle_Model_Application TheApplication = new Model_Application;

//=======================================================================
Handle(Model_Application) Model_Application::getApplication()
{
  return TheApplication;
}

//=======================================================================
const boost::shared_ptr<Model_Document>& Model_Application::getDocument(string theDocID)
{
  if (myDocs.find(theDocID) != myDocs.end())
    return myDocs[theDocID];

  static const std::string thePartSetKind("PartSet");
  static const std::string thePartKind("Part");
  boost::shared_ptr<Model_Document> aNew(
    new Model_Document(theDocID, theDocID == "root" ? thePartSetKind : thePartKind));
  myDocs[theDocID] = aNew;
  // load it if it must be loaded by demand
  if (myLoadedByDemand.find(theDocID) != myLoadedByDemand.end() && !myPath.empty()) {
    aNew->load(myPath.c_str());
    myLoadedByDemand.erase(theDocID);  // done, don't do it anymore
  }

  return myDocs[theDocID];
}

void Model_Application::deleteDocument(string theDocID)
{
  myDocs.erase(theDocID);
}

//=======================================================================
bool Model_Application::hasDocument(std::string theDocID)
{
  return myDocs.find(theDocID) != myDocs.end();
}

//=======================================================================
void Model_Application::setLoadPath(std::string thePath)
{
  myPath = thePath;
}

//=======================================================================
void Model_Application::setLoadByDemand(std::string theID)
{
  myLoadedByDemand.insert(theID);
}

//=======================================================================
Model_Application::Model_Application()
{
  // store handle to the application to avoid nullification
  static Handle(Model_Application) TheKeepHandle;
  TheKeepHandle = this;
}

//=======================================================================
void Model_Application::Formats(TColStd_SequenceOfExtendedString& theFormats)
{
  theFormats.Append(TCollection_ExtendedString("BinOcaf"));  // standard binary schema
}

//=======================================================================
Standard_CString Model_Application::ResourcesName()
{
  return Standard_CString("Standard");
}
