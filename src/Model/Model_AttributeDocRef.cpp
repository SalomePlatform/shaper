// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeDocRef.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeDocRef.h"
#include "Model_Application.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_Data.h>

using namespace std;

void Model_AttributeDocRef::setValue(std::shared_ptr<ModelAPI_Document> theDoc)
{
  if (myID->Get() != theDoc->id()) {
    myID->Set(theDoc->id());
    owner()->data()->sendAttributeUpdated(this);
  }
}

std::shared_ptr<ModelAPI_Document> Model_AttributeDocRef::value()
{
  return Model_Application::getApplication()->document(myID->Get());
}

int Model_AttributeDocRef::docId()
{
  return myID->Get();
}

Model_AttributeDocRef::Model_AttributeDocRef(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_Integer::GetID(), myID) == Standard_True;
  if (!myIsInitialized) {
    int aNewID = Model_Application::getApplication()->generateDocumentId();
    myID = TDataStd_Integer::Set(theLabel, aNewID);
  }
}
