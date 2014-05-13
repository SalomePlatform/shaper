// File:        ModelAPI_AttributeReference.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeReference.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>
#include <Events_Loop.h>

using namespace std;

void Model_AttributeReference::setValue(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (value() != theFeature) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
    if (myRef.IsNull()) {
      boost::shared_ptr<Model_Data> aMyData = 
        boost::dynamic_pointer_cast<Model_Data>(owner()->data());
      TDF_Reference::Set(aMyData->label(), aData->label());
    } else {
      myRef->Set(aData->label());
    }

    static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
    Model_FeatureUpdatedMessage aMsg(owner(), anEvent);
    Events_Loop::loop()->send(aMsg);
  }
}

boost::shared_ptr<ModelAPI_Feature> Model_AttributeReference::value()
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
  return boost::shared_ptr<ModelAPI_Feature>();
}

Model_AttributeReference::Model_AttributeReference(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDF_Reference::GetID(), myRef)) {
    // create attribute: not initialized by value yet: attribute is not set to the label!
  }
}
