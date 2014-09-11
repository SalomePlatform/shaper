// File:        ModelAPI_ResultPart.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultPart.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Session.h>

boost::shared_ptr<ModelAPI_Document> Model_ResultPart::partDoc()
{
  return data()->docRef("PartDocument")->value();
}

boost::shared_ptr<ModelAPI_Feature> Model_ResultPart::owner()
{
  return boost::shared_ptr<ModelAPI_Feature>();  // return empty pointer
}

Model_ResultPart::Model_ResultPart()
{
}

void Model_ResultPart::setData(boost::shared_ptr<ModelAPI_Data> theData)
{
  ModelAPI_Result::setData(theData);
  if (theData) {
    data()->addAttribute(DOC_REF(), ModelAPI_AttributeDocRef::type());
  }
}

void Model_ResultPart::activate()
{
  boost::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->docRef(DOC_REF());
  
  if (!aDocRef->value()) {  // create (or open) a document if it is not yet created
    boost::shared_ptr<ModelAPI_Document> aDoc = document()->subDocument(data()->name());
    if (aDoc) {
      aDocRef->setValue(aDoc);
    }
  }
  if (aDocRef->value())
    ModelAPI_Session::get()->setActiveDocument(aDocRef->value());
}
