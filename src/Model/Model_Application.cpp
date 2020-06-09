// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include <Model_Application.h>
#include <Model_Document.h>

#include <ModelAPI_Events.h>
#include <ModelAPI_Tools.h>

#include <BinDrivers_DocumentRetrievalDriver.hxx>
#include <BinDrivers_DocumentStorageDriver.hxx>

IMPLEMENT_STANDARD_RTTIEXT(Model_Application, TDocStd_Application)

static Handle_Model_Application TheApplication = new Model_Application;

//=======================================================================
Handle(Model_Application) Model_Application::getApplication()
{
  return TheApplication;
}

//=======================================================================
std::shared_ptr<Model_Document> Model_Application::document(const int theDocID)
{
  if (myDocs.find(theDocID) != myDocs.end())
    return myDocs[theDocID];
  return std::shared_ptr<Model_Document>(); // not loaded, so return null
}

//=======================================================================
void Model_Application::createDocument(const int theDocID)
{
  static const std::string thePartSetKind("PartSet");
  static const std::string thePartKind("Part");
  std::shared_ptr<Model_Document> aNew(
    new Model_Document(theDocID, theDocID == 0 ? thePartSetKind : thePartKind));
  myDocs[theDocID] = aNew;

  aNew->setThis(aNew);
  static Events_ID anId = ModelAPI_DocumentCreatedMessage::eventId();
  std::shared_ptr<ModelAPI_DocumentCreatedMessage> aMessage = std::shared_ptr
    <ModelAPI_DocumentCreatedMessage>(new ModelAPI_DocumentCreatedMessage(anId, this));
  aMessage->setDocument(aNew);
  Events_Loop::loop()->send(aMessage);
}

//=======================================================================
bool Model_Application::loadDocument(const std::wstring theDocName, const int theDocID)
{
  static const std::string thePartKind("Part"); // root document is never loaded here
  std::shared_ptr<Model_Document> aNew(new Model_Document(theDocID, thePartKind));
  myDocs[theDocID] = aNew;

  bool aRes = true;
  // load it if it must be loaded by demand
  if (myLoadedByDemand.find(theDocName) != myLoadedByDemand.end() && !myPath.empty()) {
    aRes = aNew->load(myPath.c_str(), ModelAPI_Tools::toString(theDocName).c_str(), aNew);
    myLoadedByDemand.erase(theDocName);  // done, don't do it anymore
  } else { // error
    aRes = false;
  }

  return aRes;
}

//=======================================================================
void Model_Application::deleteDocument(const int theDocID)
{
  if (myDocs.find(theDocID) != myDocs.end()) {
    myDocs[theDocID]->close(true);
    myDocs.erase(theDocID);
  }
  myLoadedByDemand.clear();
}

//=======================================================================
void Model_Application::deleteAllDocuments()
{
  std::map<int, std::shared_ptr<Model_Document> >::iterator aDoc = myDocs.begin();
  for(; aDoc != myDocs.end(); aDoc++) {
    if (aDoc->second->isOpened()) // here is main document was closed before subs and closed subs
      aDoc->second->close(true);
  }
  myDocs.clear();
  myLoadedByDemand.clear();
}

//=======================================================================
bool Model_Application::hasDocument(const int theDocID)
{
  return myDocs.find(theDocID) != myDocs.end();
}

//=======================================================================
bool Model_Application::hasRoot()
{
  return !myDocs.empty();
}

//=======================================================================
std::shared_ptr<Model_Document> Model_Application::rootDocument()
{
  return myDocs[0];
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
void Model_Application::setLoadByDemand(std::wstring theID, const int theDocID)
{
  myLoadedByDemand[theID] = theDocID;
}

//=======================================================================
bool Model_Application::isLoadByDemand(std::wstring theID, const int theDocIndex)
{
  return myLoadedByDemand.find(theID) != myLoadedByDemand.end() &&
    myLoadedByDemand[theID] == theDocIndex;
}

//=======================================================================
int Model_Application::generateDocumentId()
{
  int aResult;
  // count until the result id is unique
  for(aResult = int(myDocs.size()); true; aResult++) {
    if (myDocs.find(aResult) == myDocs.end()) {
      bool aFound = false;
      std::map<std::wstring, int>::iterator aLBDIter = myLoadedByDemand.begin();
      for(; aLBDIter != myLoadedByDemand.end(); aLBDIter++) {
        if (aLBDIter->second == aResult) {
          aFound = true;
          break;
        }
      }
      if (!aFound) break;
    }
  }
  return aResult;
}

//=======================================================================
Model_Application::Model_Application()
{
  // store handle to the application to avoid nullification
  static Handle(Model_Application) TheKeepHandle;
  TheKeepHandle = this;
  // additional file format supported
  static TCollection_ExtendedString THE_DOC_FORMAT("BinShaperPart");
  static TCollection_ExtendedString THE_FILE_EXT("shaperpart");
  Handle(PCDM_RetrievalDriver) aReader = new BinDrivers_DocumentRetrievalDriver;
  Handle(PCDM_StorageDriver) aWriter = new BinDrivers_DocumentStorageDriver;
  TheKeepHandle->DefineFormat(THE_DOC_FORMAT, "Shaper Part document", THE_FILE_EXT,
                              aReader, aWriter);
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
