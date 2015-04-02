// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefAttr.cxx
// Created:     2 Apr 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeRefAttr.h"
#include "Model_Application.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>

using namespace std;

bool Model_AttributeRefAttr::isObject()
{
  return myID->Get().Length() == 0;
}

void Model_AttributeRefAttr::setAttr(std::shared_ptr<ModelAPI_Attribute> theAttr)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(
      theAttr->owner()->data());
  string anID = aData->id(theAttr);
  if (myIsInitialized && object() == theAttr->owner() && myID->Get().IsEqual(anID.c_str()))
    return;  // nothing is changed
  REMOVE_BACK_REF(theAttr->owner());
  myRef->Set(aData->label().Father());
  myID->Set(aData->id(theAttr).c_str());
  ADD_BACK_REF(theAttr->owner());
  owner()->data()->sendAttributeUpdated(this);
}

std::shared_ptr<ModelAPI_Attribute> Model_AttributeRefAttr::attr()
{
  ObjectPtr anObj = object();
  if (anObj && anObj->data()) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(anObj->data());
    return aData->attribute(TCollection_AsciiString(myID->Get()).ToCString());
  }
  // not initialized
  return std::shared_ptr<ModelAPI_Attribute>();
}

void Model_AttributeRefAttr::setObject(ObjectPtr theObject)
{
  // the back reference from the previous object to the attribute should be removed
  ObjectPtr anObject = object();
  if (theObject && (!myIsInitialized || myID->Get().Length() != 0 || object() != theObject)) {
    REMOVE_BACK_REF(anObject);

    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(
        theObject->data());
    myRef->Set(aData->label().Father());
    myID->Set("");  // feature is identified by the empty ID

    // do it before the transaction finish to make just created/removed objects know dependencies
    // and reference from composite feature is removed automatically
    FeaturePtr anOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
    if (anOwnerFeature.get()) {
      aData->addBackReference(anOwnerFeature, id(), false);
    }
    ADD_BACK_REF(theObject);
    owner()->data()->sendAttributeUpdated(this);
  } else if (theObject.get() == NULL) {
    REMOVE_BACK_REF(anObject);
    myRef->Set(myRef->Label()); // reference to itself means that object is null
    myID->Set("");  // feature is identified by the empty ID
    owner()->data()->sendAttributeUpdated(this);
  }
}

ObjectPtr Model_AttributeRefAttr::object()
{
  if (myRef->Get() != myRef->Label()) {  // initialized
    std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
        owner()->document());
    if (aDoc) {
      TDF_Label aRefLab = myRef->Get();
      return aDoc->object(aRefLab);
    }
  }
  // not initialized
  return ObjectPtr();
}

Model_AttributeRefAttr::Model_AttributeRefAttr(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_Comment::GetID(), myID) == Standard_True;
  if (!myIsInitialized) {
    // create attribute: not initialized by value yet
    myID = TDataStd_Comment::Set(theLabel, "");
    myRef = TDF_Reference::Set(theLabel, theLabel);  // not initialized: reference to itself
  } else {
    theLabel.FindAttribute(TDF_Reference::GetID(), myRef);
  }
}
