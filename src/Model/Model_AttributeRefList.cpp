// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefList.cxx
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeRefList.h"
#include "Model_Application.h"
#include "Model_Data.h"
#include "Model_Objects.h"
#include <ModelAPI_Feature.h>
#include <TDF_ListIteratorOfLabelList.hxx>

using namespace std;

void Model_AttributeRefList::append(ObjectPtr theObject)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
  myRef->Append(aData->label().Father());  // store label of the object
  // do it before the transaction finish to make just created/removed objects know dependencies
  // and reference from composite feature is removed automatically
  ADD_BACK_REF(theObject);

  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeRefList::remove(ObjectPtr theObject)
{
  std::shared_ptr<Model_Data> aData;
  if (theObject.get() != NULL) {
    aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
    myRef->Remove(aData->label().Father());
    REMOVE_BACK_REF(theObject);
  }
  else { // in case of empty object remove, the first empty object is removed from the list
    std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
        owner()->document());
    if (aDoc) {
      const TDF_LabelList& aList = myRef->List();
      for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
        ObjectPtr anObj = aDoc->objects()->object(aLIter.Value());
        if (anObj.get() == NULL) {
          myRef->Remove(aLIter.Value());
          REMOVE_BACK_REF(theObject);
          break;
        }
      }
    }
  }
  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeRefList::clear()
{
  std::list<ObjectPtr> anOldList = list();
  myRef->Clear();
  std::list<ObjectPtr>::iterator anOldIter = anOldList.begin();
  for(; anOldIter != anOldList.end(); anOldIter++) {
    REMOVE_BACK_REF((*anOldIter));
  }
  owner()->data()->sendAttributeUpdated(this);
}

int Model_AttributeRefList::size(const bool theWithEmpty) const
{
  if (theWithEmpty)
    return myRef->Extent();
  int aResult = 0;
  const TDF_LabelList& aList = myRef->List();
  for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
    if (!aLIter.Value().IsNull() && !aLIter.Value().IsRoot()) aResult++;
  }
  return aResult;
}

bool Model_AttributeRefList::isInitialized()
{
  if (size(false) == 0) { // empty list is not initialized list: sketch will be not valid after add/undo
    return false;
  }
  return ModelAPI_AttributeRefList::isInitialized();
}

list<ObjectPtr> Model_AttributeRefList::list()
{
  std::list<ObjectPtr> aResult;
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    const TDF_LabelList& aList = myRef->List();
    for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
      ObjectPtr anObj;
      if (!aLIter.Value().IsNull() && !aLIter.Value().IsRoot())
        anObj = aDoc->objects()->object(aLIter.Value());
      aResult.push_back(anObj);
    }
  }
  return aResult;
}

bool Model_AttributeRefList::isInList(const ObjectPtr& theObj)
{
  if(!theObj.get()) {
    return false;
  }
  std::list<ObjectPtr> aResult;
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theObj->data());
    if (aData.get() && aData->isValid()) {
      TDF_Label anObjLab = aData->label().Father();
      const TDF_LabelList& aList = myRef->List();
      for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
        if (aLIter.Value().IsEqual(anObjLab)) {
          return true;
        }
      }
    }
  }
  return false;
}

ObjectPtr Model_AttributeRefList::object(const int theIndex, const bool theWithEmpty) const
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    const TDF_LabelList& aList = myRef->List();
    int anIndex = -1;
    for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
      if (theWithEmpty || (!aLIter.Value().IsNull() && !aLIter.Value().IsRoot()))
        anIndex++;
      if (anIndex == theIndex) {
        if (aLIter.Value().IsNull() || aLIter.Value().IsRoot()) { // null label => null sub
          return ObjectPtr();
        }
        return aDoc->objects()->object(aLIter.Value());
      }
    }
  }
  return ObjectPtr();
}

void Model_AttributeRefList::substitute(const ObjectPtr& theCurrent, const ObjectPtr& theNew)
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theCurrent->data());
    if (aData.get() && aData->isValid()) {
      TDF_Label aCurrentLab = aData->label().Father();
      TDF_Label aNewLab;
      if (theNew.get() && theNew->data()->isValid()) { // the new may be null
        std::shared_ptr<Model_Data> aNewData = 
          std::dynamic_pointer_cast<Model_Data>(theNew->data());
        aNewLab = aNewData->label().Father();
      } else {
        aNewLab = aCurrentLab.Root(); // root means null object
      }
      // do the substitution
      ADD_BACK_REF(theNew);
      if (myRef->InsertAfter(aNewLab, aCurrentLab)) {
        myRef->Remove(aCurrentLab);
        REMOVE_BACK_REF(theCurrent);
      }
      owner()->data()->sendAttributeUpdated(this);
    }
  }
}

void Model_AttributeRefList::exchange(const ObjectPtr& theObject1, const ObjectPtr& theObject2)
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    std::shared_ptr<Model_Data> aData1 = std::dynamic_pointer_cast<Model_Data>(theObject1->data());
    if (aData1.get() && aData1->isValid()) {
      TDF_Label aLab1 = aData1->label().Father();
      if (theObject2.get() && theObject2->data()->isValid()) { // the new may be null
        std::shared_ptr<Model_Data> aData2 = 
          std::dynamic_pointer_cast<Model_Data>(theObject2->data());
        if (aData2.get() && aData2->isValid()) {
          TDF_Label aLab2 = aData2->label().Father();
          // do the substitution: use the temporary label, as usually in exchange
          TDF_Label aTmpLab = aLab1.Root();
          if (myRef->InsertAfter(aTmpLab, aLab1)) {
            myRef->Remove(aLab1);
          }
          if (myRef->InsertAfter(aLab1, aLab2)) {
            myRef->Remove(aLab2);
          }
          if (myRef->InsertAfter(aLab2, aTmpLab)) {
            myRef->Remove(aTmpLab);
          }
          owner()->data()->sendAttributeUpdated(this);
        }
      }
    }
  }
}

void Model_AttributeRefList::removeLast()
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc && !myRef->IsEmpty()) {
    ObjectPtr anObj = aDoc->objects()->object(myRef->Last());
    if (anObj.get()) {
      myRef->Remove(myRef->Last());
      REMOVE_BACK_REF(anObj);
      owner()->data()->sendAttributeUpdated(this);
    }
  }
}

void Model_AttributeRefList::remove(const std::set<int>& theIndices)
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc && !myRef->IsEmpty()) {
    // collet labels that will be removed
    TDF_LabelList aLabelsToRemove;
    TDF_ListIteratorOfLabelList aLabIter(myRef->List());
    for(int aCurrent = 0; aLabIter.More(); aLabIter.Next(), aCurrent++) {
      if (theIndices.find(aCurrent) != theIndices.end())
        aLabelsToRemove.Append(aLabIter.Value());
    }
    // remove labels
    for(aLabIter.Initialize(aLabelsToRemove); aLabIter.More(); aLabIter.Next()) {
      ObjectPtr anObj = aDoc->objects()->object(aLabIter.Value());
      if (anObj.get()) {
        myRef->Remove(aLabIter.Value());
        REMOVE_BACK_REF(anObj);
      }
    }
    if (!aLabelsToRemove.IsEmpty()) {
      owner()->data()->sendAttributeUpdated(this);
    }
  }
}

Model_AttributeRefList::Model_AttributeRefList(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_ReferenceList::GetID(), myRef) == Standard_True;
  if (!myIsInitialized) {
    myRef = TDataStd_ReferenceList::Set(theLabel);
  }
}
