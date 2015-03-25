// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefList.cxx
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeRefList.h"
#include "Model_Application.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>
#include <TDF_ListIteratorOfLabelList.hxx>

using namespace std;

void Model_AttributeRefList::append(ObjectPtr theObject)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
  myRef->Append(aData->label().Father());  // store label of the object
  // do it before the transaction finish to make just created/removed objects know dependencies
  // and reference from composite feature is removed automatically
  FeaturePtr anOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
  if (anOwnerFeature.get()) {
    aData->addBackReference(anOwnerFeature, id());
  }

  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeRefList::remove(ObjectPtr theObject)
{
  std::shared_ptr<Model_Data> aData;
  if (theObject.get() != NULL) {
    aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
    myRef->Remove(aData->label().Father());
  }
  else { // in case of empty object remove, the first empty object is removed from the list
    std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
        owner()->document());
    if (aDoc) {
      const TDF_LabelList& aList = myRef->List();
      for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
        ObjectPtr anObj = aDoc->object(aLIter.Value());
        if (anObj.get() == NULL) {
          myRef->Remove(aLIter.Value());
          break;
        }
      }
    }
  }
  owner()->data()->sendAttributeUpdated(this);
}

int Model_AttributeRefList::size() const
{
  return myRef->Extent();
}

list<ObjectPtr> Model_AttributeRefList::list()
{
  std::list<ObjectPtr> aResult;
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    const TDF_LabelList& aList = myRef->List();
    for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
      ObjectPtr anObj = aDoc->object(aLIter.Value());
      aResult.push_back(anObj);
    }
  }
  return aResult;
}

ObjectPtr Model_AttributeRefList::object(const int theIndex) const
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    const TDF_LabelList& aList = myRef->List();
    int anIndex = 0;
    for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next(), anIndex++) {
      if (anIndex == theIndex)
        return aDoc->object(aLIter.Value());
    }
  }
  return ObjectPtr();
}

Model_AttributeRefList::Model_AttributeRefList(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_ReferenceList::GetID(), myRef) == Standard_True;
  if (!myIsInitialized) {
    myRef = TDataStd_ReferenceList::Set(theLabel);
  }
}
