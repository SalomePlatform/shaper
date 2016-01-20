// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModelAPI_AttributeRefAttrList.cxx
// Created:     8 May 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeRefAttrList.h"
#include "Model_Application.h"
#include "Model_Data.h"
#include "Model_Objects.h"
#include <ModelAPI_Feature.h>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>

using namespace std;

void Model_AttributeRefAttrList::append(ObjectPtr theObject)
{
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
  myRef->Append(aData->label().Father());  // store label of the object
  myIDs->Append(""); // for the object store an empty string
  // do it before the transaction finish to make just created/removed objects know dependencies
  // and reference from composite feature is removed automatically
  ADD_BACK_REF(theObject);

  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeRefAttrList::append(AttributePtr theAttr)
{
  std::shared_ptr<Model_Data> aData = 
    std::dynamic_pointer_cast<Model_Data>(theAttr->owner()->data());
  myRef->Append(aData->label().Father());  // store label of the object
  myIDs->Append(theAttr->id().c_str()); // store the ID of the referenced attribute
  // do it before the transaction finish to make just created/removed objects know dependencies
  // and reference from composite feature is removed automatically
  ADD_BACK_REF(theAttr->owner());

  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeRefAttrList::remove(ObjectPtr theObject)
{
  TDF_Label aTheObjLab;
  if (theObject.get() != NULL) {
    aTheObjLab = std::dynamic_pointer_cast<Model_Data>(theObject->data())->label().Father();
  }
  std::shared_ptr<Model_Document> aDoc = 
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  // remove from the both lists by clearing the list and then appending one by one
  // TODO: in OCCT 7.0 there are methods for removing by index, seems will be more optimal
  TDF_LabelList aRefList = myRef->List();
  myRef->Clear();
  TDataStd_ListOfExtendedString anIDList = myIDs->List();
  myIDs->Clear();
  bool aOneisDeleted = false;
  TDF_ListIteratorOfLabelList aRefIter(aRefList);
  TDataStd_ListIteratorOfListOfExtendedString anIDIter(anIDList);
  for (; aRefIter.More(); aRefIter.Next(), anIDIter.Next()) {
    // append now only not removed
    if (aOneisDeleted || aRefIter.Value() != aTheObjLab || !anIDIter.Value().IsEmpty() ||
      (aTheObjLab.IsNull() && aDoc->objects()->object(aRefIter.Value()) != NULL)) {
      myRef->Append(aRefIter.Value());
      myIDs->Append(anIDIter.Value());
    } else if (aTheObjLab.IsNull() && aDoc->objects()->object(aRefIter.Value()) != NULL) {
      aOneisDeleted = true;
    }
  }
  if (aOneisDeleted) {
    REMOVE_BACK_REF(theObject);
    owner()->data()->sendAttributeUpdated(this);
  }
}

void Model_AttributeRefAttrList::remove(AttributePtr theAttr)
{
  TDF_Label aTheObjLab;
  if (theAttr->owner().get() != NULL) {
    aTheObjLab = std::dynamic_pointer_cast<Model_Data>(theAttr->owner()->data())->label().Father();
  }
  std::shared_ptr<Model_Document> aDoc = 
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  // remove from the both lists by clearing the list and then appending one by one
  // TODO: in OCCT 7.0 there are methods for removing by index, seems will be more optimal
  TDF_LabelList aRefList = myRef->List();
  myRef->Clear();
  TDataStd_ListOfExtendedString anIDList = myIDs->List();
  myIDs->Clear();
  bool aOneisDeleted = false;
  TDF_ListIteratorOfLabelList aRefIter(aRefList);
  TDataStd_ListIteratorOfListOfExtendedString anIDIter(anIDList);
  for (; aRefIter.More(); aRefIter.Next(), anIDIter.Next()) {
    if (aOneisDeleted || anIDIter.Value() != theAttr->id().c_str() ||  // append now only not removed
        aRefIter.Value() != aTheObjLab || // append now only not removed
        (aTheObjLab.IsNull() && aDoc->objects()->object(aRefIter.Value()) != NULL)) {
      myRef->Append(aRefIter.Value());
      myIDs->Append(anIDIter.Value());
    } else {
      aOneisDeleted = true;
    }
  }
  if (aOneisDeleted) {
    REMOVE_BACK_REF(theAttr->owner());
    owner()->data()->sendAttributeUpdated(this);
  }
}

void Model_AttributeRefAttrList::clear()
{
  std::list<std::pair<ObjectPtr, AttributePtr> > anOldList = list();
  myRef->Clear();
  myIDs->Clear();
  std::list<std::pair<ObjectPtr, AttributePtr> >::iterator anOldIter = anOldList.begin();
  for(; anOldIter != anOldList.end(); anOldIter++) {
    REMOVE_BACK_REF((anOldIter->first));
  }
  owner()->data()->sendAttributeUpdated(this);
}

int Model_AttributeRefAttrList::size() const
{
  return myRef->Extent();
}

bool Model_AttributeRefAttrList::isInitialized()
{
  if (size() == 0) { // empty list is not initialized list: sketch will be not valid after add/undo
    return false;
  }
  return ModelAPI_AttributeRefAttrList::isInitialized();
}

std::list<std::pair<ObjectPtr, AttributePtr> > Model_AttributeRefAttrList::list()
{
  std::list<std::pair<ObjectPtr, AttributePtr> > aResult;
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    const TDF_LabelList& aList = myRef->List();
    const TDataStd_ListOfExtendedString& anIDList = myIDs->List();
    TDF_ListIteratorOfLabelList aLIter(aList);
    TDataStd_ListIteratorOfListOfExtendedString anIDIter(anIDList);
    for (; aLIter.More(); aLIter.Next(), anIDIter.Next()) {
      ObjectPtr anObj;
      if (!aLIter.Value().IsNull())
        anObj = aDoc->objects()->object(aLIter.Value());
      aResult.push_back(std::pair<ObjectPtr, AttributePtr>(anObj, 
        anObj->data()->attribute(TCollection_AsciiString(anIDIter.Value()).ToCString())));
    }
  }
  return aResult;
}

bool Model_AttributeRefAttrList::isInList(const ObjectPtr& theObj)
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
      const TDataStd_ListOfExtendedString& anIDList = myIDs->List();
      TDF_ListIteratorOfLabelList aLIter(aList);
      TDataStd_ListIteratorOfListOfExtendedString anIDIter(anIDList);
      for (; aLIter.More(); aLIter.Next(), anIDIter.Next()) {
        if (anIDIter.Value().IsEmpty() && aLIter.Value().IsEqual(anObjLab)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Model_AttributeRefAttrList::isInList(const AttributePtr& theAttr)
{
  if (!theAttr->owner().get()) {
    return false;
  }
  std::list<ObjectPtr> aResult;
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    std::shared_ptr<Model_Data> aData = 
      std::dynamic_pointer_cast<Model_Data>(theAttr->owner()->data());
    if (aData.get() && aData->isValid()) {
      TDF_Label anObjLab = aData->label().Father();
      const TDF_LabelList& aList = myRef->List();
      const TDataStd_ListOfExtendedString& anIDList = myIDs->List();
      TDF_ListIteratorOfLabelList aLIter(aList);
      TDataStd_ListIteratorOfListOfExtendedString anIDIter(anIDList);
      for (; aLIter.More(); aLIter.Next(), anIDIter.Next()) {
        if (anIDIter.Value() == theAttr->id().c_str() && aLIter.Value().IsEqual(anObjLab)) {
          return true;
        }
      }
    }
  }
  return false;
}

bool Model_AttributeRefAttrList::isAttribute(const int theIndex) const
{
  const TDataStd_ListOfExtendedString& anIDList = myIDs->List();
  TDataStd_ListIteratorOfListOfExtendedString anIDIter(anIDList);
  int anIndex = -1;
  for (; anIDIter.More(); anIDIter.Next()) {
    anIndex++;
    if (anIndex == theIndex) {
      return !anIDIter.Value().IsEmpty();
    }
  }
  return false;
}

ObjectPtr Model_AttributeRefAttrList::object(const int theIndex) const
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    const TDF_LabelList& aList = myRef->List();
    int anIndex = -1;
    for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
      anIndex++;
      if (anIndex == theIndex) {
        if (aLIter.Value().IsNull()) { // null label => null sub
          return ObjectPtr();
        }
        return aDoc->objects()->object(aLIter.Value());
      }
    }
  }
  return ObjectPtr();
}

AttributePtr Model_AttributeRefAttrList::attribute(const int theIndex) const
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    int anIndex = -1;
    const TDF_LabelList& aList = myRef->List();
    const TDataStd_ListOfExtendedString& anIDList = myIDs->List();
    TDF_ListIteratorOfLabelList aLIter(aList);
    TDataStd_ListIteratorOfListOfExtendedString anIDIter(anIDList);
    for (; aLIter.More(); aLIter.Next(), anIDIter.Next()) {
      anIndex++;
      if (anIndex == theIndex) {
        if (aLIter.Value().IsNull()) { // null label => null sub
          return AttributePtr();
        }
        return aDoc->objects()->object(aLIter.Value())->data()->
          attribute(TCollection_AsciiString(anIDIter.Value().ToExtString()).ToCString());
      }
    }
  }
  return AttributePtr();
}

void Model_AttributeRefAttrList::removeLast()
{
  // remove from the both lists by clearing the list and then appending one by one
  // TODO: in OCCT 7.0 there are methods for removing by index, seems will be more optimal
  std::set<int> aLastSet;
  aLastSet.insert(size() - 1);
  remove(aLastSet);
}

void Model_AttributeRefAttrList::remove(const std::set<int>& theIndices)
{
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc && !myRef->IsEmpty()) {
    // remove from the both lists by clearing the list and then appending one by one
    // TODO: in OCCT 7.0 there are methods for removing by index, seems will be more optimal
    TDF_LabelList aRefList = myRef->List();
    myRef->Clear();
    TDataStd_ListOfExtendedString anIDList = myIDs->List();
    myIDs->Clear();
    bool aOneisDeleted = false;
    TDF_ListIteratorOfLabelList aRefIter(aRefList);
    TDataStd_ListIteratorOfListOfExtendedString anIDIter(anIDList);
    for (int anIndex = 0; aRefIter.More(); aRefIter.Next(), anIDIter.Next(), anIndex++) {
      if (theIndices.find(anIndex) == theIndices.end()) { // not found
        myRef->Append(aRefIter.Value());
        myIDs->Append(anIDIter.Value());
      } else { // found, so need to update the dependencies
        aOneisDeleted = true;
        ObjectPtr anObj = aDoc->objects()->object(aRefIter.Value());
        if (anObj.get()) {
          REMOVE_BACK_REF(anObj);
        }
      }
    }
    if (aOneisDeleted) {
      owner()->data()->sendAttributeUpdated(this);
    }
  }
}

Model_AttributeRefAttrList::Model_AttributeRefAttrList(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_ReferenceList::GetID(), myRef) == Standard_True;
  if (!myIsInitialized) {
    myRef = TDataStd_ReferenceList::Set(theLabel);
    myIDs = TDataStd_ExtStringList::Set(theLabel);
  } else {
    theLabel.FindAttribute(TDataStd_ExtStringList::GetID(), myIDs);
  }
}
