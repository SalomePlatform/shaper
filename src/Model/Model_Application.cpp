// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:	Model_Application.cxx
// Created:	Fri Sep 2 2011
// Author:	Mikhail PONIKAROV

#include <Model_Application.h>
#include <Model_Document.h>

#include <ModelAPI_Events.h>

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
const std::shared_ptr<Model_Document>& Model_Application::getDocument(string theDocID)
{
  if (myDocs.find(theDocID) != myDocs.end())
    return myDocs[theDocID];

  static const std::string thePartSetKind("PartSet");
  static const std::string thePartKind("Part");
  bool isRoot = theDocID == "root"; // the document is root
  std::shared_ptr<Model_Document> aNew(
    new Model_Document(theDocID, isRoot ? thePartSetKind : thePartKind));
  aNew->setThis(aNew);
  myDocs[theDocID] = aNew;

  // load it if it must be loaded by demand
  if (myLoadedByDemand.find(theDocID) != myLoadedByDemand.end() && !myPath.empty()) {
    aNew->load(myPath.c_str());
    myLoadedByDemand.erase(theDocID);  // done, don't do it anymore
  } else {
    static Events_ID anId = ModelAPI_DocumentCreatedMessage::eventId();
    std::shared_ptr<ModelAPI_DocumentCreatedMessage> aMessage = std::shared_ptr
      <ModelAPI_DocumentCreatedMessage>(new ModelAPI_DocumentCreatedMessage(anId, this));
    aMessage->setDocument(aNew);
    Events_Loop::loop()->send(aMessage);
  }

  return myDocs[theDocID];
}

void Model_Application::deleteDocument(string theDocID)
{
  if (myDocs.find(theDocID) != myDocs.end()) {
    myDocs[theDocID]->close(true);
    myDocs.erase(theDocID);
  }
  myLoadedByDemand.clear();
}

void Model_Application::deleteAllDocuments()
{
  std::map<std::string, std::shared_ptr<Model_Document> >::iterator aDoc = myDocs.begin();
  for(; aDoc != myDocs.end(); aDoc++) {
    aDoc->second->close();
  }
  myDocs.clear();
  myLoadedByDemand.clear();
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
const std::string& Model_Application::loadPath() const
{
  return myPath;
}

//=======================================================================
void Model_Application::setLoadByDemand(std::string theID)
{
  myLoadedByDemand.insert(theID);
}

//=======================================================================
bool Model_Application::isLoadByDemand(std::string theID)
{
  return myLoadedByDemand.find(theID) != myLoadedByDemand.end();
}

//=======================================================================
void Model_Application::removeUselessDocuments(
  std::list<std::shared_ptr<ModelAPI_Document> > theUsedDocs)
{
  std::map<std::string, std::shared_ptr<Model_Document> >::iterator aDoc = myDocs.begin();
  while(aDoc != myDocs.end()) {
    bool aFound = false;
    std::list<std::shared_ptr<ModelAPI_Document> >::iterator aUsed = theUsedDocs.begin();
    for(; !aFound && aUsed != theUsedDocs.end(); aUsed++) {
      aFound = aDoc->second == *aUsed;
    }
    if (!aFound) { // remove the useless
      aDoc->second->close();
      myDocs.erase(aDoc);
      aDoc = myDocs.begin();
    } else {
      aDoc++;
    }
  }
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
