// File:        ModelAPI_AttributeDocRef.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeDocRef.h"
#include "Model_Application.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

using namespace std;

void Model_AttributeDocRef::setValue(boost::shared_ptr<ModelAPI_Document> theDoc)
{
  myDoc = theDoc;
  TCollection_ExtendedString aNewID(theDoc->id().c_str());
  if (!myIsInitialized || myComment->Get() != aNewID) {
    myComment->Set(TCollection_ExtendedString(theDoc->id().c_str()));
    owner()->data()->sendAttributeUpdated(this);
  }
}

boost::shared_ptr<ModelAPI_Document> Model_AttributeDocRef::value()
{
  return myDoc;
}

Model_AttributeDocRef::Model_AttributeDocRef(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_Comment::GetID(), myComment) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet, just empty string
    myComment = TDataStd_Comment::Set(theLabel, "");
  } else {  // document was already referenced: try to set it as loaded by demand
    Handle(Model_Application) anApp = Model_Application::getApplication();
    string anID(TCollection_AsciiString(myComment->Get()).ToCString());
    if (!anApp->hasDocument(anID)) {
      anApp->setLoadByDemand(anID);
    }
  }
}
