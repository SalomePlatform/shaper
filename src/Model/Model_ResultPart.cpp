// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_ResultPart.cpp
// Created:     07 Jul 2014
// Author:      Mikhail PONIKAROV

#include <Model_ResultPart.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_AttributeDocRef.h>
#include <ModelAPI_Session.h>

std::shared_ptr<ModelAPI_Document> Model_ResultPart::partDoc()
{
  return data()->document("PartDocument")->value();
}

std::shared_ptr<ModelAPI_Feature> Model_ResultPart::owner()
{
  return std::shared_ptr<ModelAPI_Feature>();  // return empty pointer
}

Model_ResultPart::Model_ResultPart()
{
  setIsConcealed(false);
}

void Model_ResultPart::setData(std::shared_ptr<ModelAPI_Data> theData)
{
  ModelAPI_Result::setData(theData);
  if (theData) {
    data()->addAttribute(DOC_REF(), ModelAPI_AttributeDocRef::typeId());
  }
}

void Model_ResultPart::activate()
{
  std::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->document(DOC_REF());
  
  if (!aDocRef->value().get()) {  // create (or open) a document if it is not yet created
    std::shared_ptr<ModelAPI_Document> aDoc = document()->subDocument(data()->name());
    if (aDoc) {
      aDocRef->setValue(aDoc);
    }
  }
  if (aDocRef->value().get()) {
    ModelAPI_Session::get()->setActiveDocument(aDocRef->value());
  }
}

bool Model_ResultPart::isActivated() 
{
  std::shared_ptr<ModelAPI_AttributeDocRef> aDocRef = data()->document(DOC_REF());
  return aDocRef->value().get();
}
