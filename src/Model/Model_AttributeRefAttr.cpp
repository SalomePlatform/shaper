// File:        ModelAPI_AttributeRefAttr.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeRefAttr.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>
#include <Events_Loop.h>

using namespace std;

bool Model_AttributeRefAttr::isFeature()
{
  return myID->Get().Length() == 0;
}

void Model_AttributeRefAttr::setAttr(boost::shared_ptr<ModelAPI_Attribute> theAttr)
{
  boost::shared_ptr<Model_Data> aData = 
    boost::dynamic_pointer_cast<Model_Data>(theAttr->owner()->data());
  string anID = aData->id(theAttr);
  if (feature() == theAttr->owner() && myID->Get().IsEqual(anID.c_str()))
    return; // nothing is changed

  myRef->Set(aData->label());
  myID->Set(aData->id(theAttr).c_str());

  static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
  Model_FeatureUpdatedMessage aMsg(owner(), anEvent);
  Events_Loop::loop()->send(aMsg);
}

boost::shared_ptr<ModelAPI_Attribute> Model_AttributeRefAttr::attr()
{
  boost::shared_ptr<ModelAPI_Feature> aFeature = feature();
  if (aFeature) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>(aFeature->data());
    return aData->attribute(TCollection_AsciiString(myID->Get()).ToCString());
  }
  // not initialized
  return boost::shared_ptr<ModelAPI_Attribute>();
}

void Model_AttributeRefAttr::setFeature(boost::shared_ptr<ModelAPI_Feature> theFeature)
{
  if (myID->Get().Length() != 0 || feature() != theFeature) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
    myRef->Set(aData->label());
    myID->Set(""); // feature is identified by the empty ID

    static Events_ID anEvent = Events_Loop::eventByName(EVENT_FEATURE_UPDATED);
    Model_FeatureUpdatedMessage aMsg(owner(), anEvent);
    Events_Loop::loop()->send(aMsg);
  }
}

boost::shared_ptr<ModelAPI_Feature> Model_AttributeRefAttr::feature()
{
  if (myRef->Get() != myRef->Label()) { // initialized
    boost::shared_ptr<Model_Document> aDoc = 
      boost::dynamic_pointer_cast<Model_Document>(owner()->document());
    if (aDoc) {
      TDF_Label aRefLab = myRef->Get();
      TDF_Label aFeatureLab = aRefLab.Father();
      return aDoc->feature(aRefLab);
    }
  }
  // not initialized
  return boost::shared_ptr<ModelAPI_Feature>();
}

Model_AttributeRefAttr::Model_AttributeRefAttr(TDF_Label& theLabel)
{
  // check the attribute could be already presented in this doc (after load document)
  if (!theLabel.FindAttribute(TDataStd_Comment::GetID(), myID)) {
    // create attribute: not initialized by value yet
    myID = TDataStd_Comment::Set(theLabel, "");
    myRef = TDF_Reference::Set(theLabel, theLabel); // not initialized: reference to itself
  } else {
    theLabel.FindAttribute(TDF_Reference::GetID(), myRef);
  }
}
