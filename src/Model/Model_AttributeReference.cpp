// File:        ModelAPI_AttributeReference.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeReference.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>

using namespace std;

void Model_AttributeReference::setValue(FeaturePtr theFeature)
{
  if (!myIsInitialized || value() != theFeature) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
    if (myRef.IsNull()) {
      boost::shared_ptr<Model_Data> aMyData = 
        boost::dynamic_pointer_cast<Model_Data>(owner()->data());
      myRef = TDF_Reference::Set(aMyData->label(), aData->label());
    } else {
      myRef->Set(aData->label());
    }
    owner()->data()->sendAttributeUpdated(this);
  }
}

FeaturePtr Model_AttributeReference::value()
{
  if (!myRef.IsNull()) {
    boost::shared_ptr<Model_Document> aDoc = 
      boost::dynamic_pointer_cast<Model_Document>(owner()->document());
    if (aDoc) {
      TDF_Label aRefLab = myRef->Get();
      return aDoc->feature(aRefLab);
    }
  }
  // not initialized
  return FeaturePtr();
}

Model_AttributeReference::Model_AttributeReference(TDF_Label& theLabel)
{
  // not initialized by value yet: attribute is not set to the label!
  myIsInitialized = theLabel.FindAttribute(TDF_Reference::GetID(), myRef) == Standard_True;
}
