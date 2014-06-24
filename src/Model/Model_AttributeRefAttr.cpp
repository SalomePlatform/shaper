// File:        ModelAPI_AttributeRefAttr.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeRefAttr.h"
#include "Model_Application.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>

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
  if (myIsInitialized && feature() == theAttr->owner() && myID->Get().IsEqual(anID.c_str()))
    return; // nothing is changed

  myRef->Set(aData->label());
  myID->Set(aData->id(theAttr).c_str());
  owner()->data()->sendAttributeUpdated(this);
}

boost::shared_ptr<ModelAPI_Attribute> Model_AttributeRefAttr::attr()
{
  FeaturePtr aFeature = feature();
  if (aFeature) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>(aFeature->data());
    return aData->attribute(TCollection_AsciiString(myID->Get()).ToCString());
  }
  // not initialized
  return boost::shared_ptr<ModelAPI_Attribute>();
}

void Model_AttributeRefAttr::setFeature(FeaturePtr theFeature)
{
  if (!myIsInitialized || myID->Get().Length() != 0 || feature() != theFeature) {
    boost::shared_ptr<Model_Data> aData = 
      boost::dynamic_pointer_cast<Model_Data>(theFeature->data());
    myRef->Set(aData->label());
    myID->Set(""); // feature is identified by the empty ID
    owner()->data()->sendAttributeUpdated(this);
  }
}

FeaturePtr Model_AttributeRefAttr::feature()
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
  return FeaturePtr();
}

Model_AttributeRefAttr::Model_AttributeRefAttr(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_Comment::GetID(), myID) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet
    myID = TDataStd_Comment::Set(theLabel, "");
    myRef = TDF_Reference::Set(theLabel, theLabel); // not initialized: reference to itself
  } else {
    theLabel.FindAttribute(TDF_Reference::GetID(), myRef);
  }
}
