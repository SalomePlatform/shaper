// File:        ModelAPI_AttributeReference.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeReference.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>

using namespace std;

void Model_AttributeReference::setValue(ObjectPtr theObject)
{
  if (!myIsInitialized || value() != theObject) {
    boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>(
        theObject->data());
    myRef->Set(aData->label().Father());  // references to the feature label
    owner()->data()->sendAttributeUpdated(this);
  }
}

ObjectPtr Model_AttributeReference::value()
{
  if (myIsInitialized) {
    boost::shared_ptr<Model_Document> aDoc = boost::dynamic_pointer_cast<Model_Document>(
        owner()->document());
    if (aDoc) {
      TDF_Label aRefLab = myRef->Get();
      return aDoc->object(aRefLab);
    }
  }
  // not initialized
  return FeaturePtr();
}

Model_AttributeReference::Model_AttributeReference(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDF_Reference::GetID(), myRef) == Standard_True;
  if (!myIsInitialized)
    myRef = TDF_Reference::Set(theLabel, theLabel);  // not initialized references to itself
}
