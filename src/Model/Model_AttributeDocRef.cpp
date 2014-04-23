// File:        ModelAPI_AttributeDocRef.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeDocRef.h"
#include "Model_Application.h"

using namespace std;

void Model_AttributeDocRef::setValue(boost::shared_ptr<ModelAPI_Document> theDoc)
{
  myComment->Set(TCollection_ExtendedString(theDoc->id().c_str()));
}

boost::shared_ptr<ModelAPI_Document> Model_AttributeDocRef::value()
{
  if (myComment->Get().Length())
    return Model_Application::getApplication()->getDocument(
      TCollection_AsciiString(myComment->Get()).ToCString());
  // not initialized
  return boost::shared_ptr<ModelAPI_Document>();
}

Model_AttributeDocRef::Model_AttributeDocRef(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_Comment::GetID(), myComment)) {
    // create attribute: not initialized by value yet, just empty string
    myComment = TDataStd_Comment::Set(theLabel, "");
  }
}
