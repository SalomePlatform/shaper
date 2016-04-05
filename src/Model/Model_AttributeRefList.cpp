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
#include <TDF_Tool.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>

using namespace std;

void Model_AttributeRefList::append(ObjectPtr theObject)
{
  if (owner()->document() == theObject->document()) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
    myRef->Append(aData->label().Father());  // store label of the object
  } else if (theObject.get() && theObject->data()->isValid()) { // reference to the other document
    myRef->Append(myRef->Label());
    // if these attributes exist, the link is external: keep reference to access the label
    std::ostringstream anIdString; // string with document Id
    anIdString<<theObject->document()->id();
    myExtDocRef->Append(anIdString.str().c_str());
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aData->label().Father(), anEntry);
    myExtDocRef->Append(anEntry);
  } else return; // something is wrong

  // do it before the transaction finish to make just created/removed objects know dependencies
  // and reference from composite feature is removed automatically
  ADD_BACK_REF(theObject);
  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeRefList::remove(ObjectPtr theObject)
{
  if (theObject.get() != NULL) {
    if (owner()->document() == theObject->document()) {
      std::shared_ptr<Model_Data> aData;
      aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
      myRef->Remove(aData->label().Father());
      REMOVE_BACK_REF(theObject);
      owner()->data()->sendAttributeUpdated(this);
    } else {
      // create new lists because for the current moment remove one of the duplicated elements
      // from the list is buggy
      TDF_LabelList anOldList = myRef->List();
      myRef->Clear();
      TDataStd_ListOfExtendedString anOldExts = myExtDocRef->List();
      myExtDocRef->Clear();

      std::ostringstream anIdString; // string with document Id
      anIdString<<theObject->document()->id();
      std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
      TCollection_AsciiString anEntry;
      TDF_Tool::Entry(aData->label().Father(), anEntry);
      bool aFound = false;
      TDataStd_ListIteratorOfListOfExtendedString anExtIter(anOldExts);
      for (TDF_ListIteratorOfLabelList aLIter(anOldList); aLIter.More(); aLIter.Next()) {
        if (aLIter.Value() == myRef->Label()) {
          if (anExtIter.Value() == anIdString.str().c_str()) {
            TDataStd_ListIteratorOfListOfExtendedString anExtIter2 = anExtIter;
            anExtIter2.Next();
            if (anExtIter2.Value() == anEntry) { // fully maches, so, remove(don't copy)
              aFound = true;
              continue;
            }
          }
          myExtDocRef->Append(anExtIter.Value());
          anExtIter.Next();
          myExtDocRef->Append(anExtIter.Value());
          anExtIter.Next();
        }
        myRef->Append(aLIter.Value());
      }
      if (aFound) {
        REMOVE_BACK_REF(theObject);
        owner()->data()->sendAttributeUpdated(this);
      }
    }
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
          owner()->data()->sendAttributeUpdated(this);
          break;
        }
      }
    }
  }
}

void Model_AttributeRefList::clear()
{
  std::list<ObjectPtr> anOldList = list();
  myRef->Clear();
  std::list<ObjectPtr>::iterator anOldIter = anOldList.begin();
  for(; anOldIter != anOldList.end(); anOldIter++) {
    REMOVE_BACK_REF((*anOldIter));
  }
  myExtDocRef->Clear();
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

ObjectPtr Model_AttributeRefList::iteratedObject(TDF_ListIteratorOfLabelList& theLIter,
    TDataStd_ListIteratorOfListOfExtendedString& theExtIter, 
    std::shared_ptr<Model_Document> theDoc) const
{
  ObjectPtr anObj;
  if (!theLIter.Value().IsNull() && !theLIter.Value().IsRoot()) {
    if (theLIter.Value() == myRef->Label()) { // external document object
      int anID = atoi(TCollection_AsciiString(theExtIter.Value()).ToCString());
      theExtIter.Next();
      DocumentPtr aRefDoc = Model_Application::getApplication()->document(anID);
      if (aRefDoc.get()) {
        std::shared_ptr<Model_Document> aDR = std::dynamic_pointer_cast<Model_Document>(aRefDoc);
        TDF_Label aRefLab;
        TDF_Tool::Label(aDR->objects()->featuresLabel().Data(),
          TCollection_AsciiString(theExtIter.Value()).ToCString(), aRefLab);
        if (!aRefLab.IsNull()) {
          anObj = aDR->objects()->object(aRefLab);
        }
      }
      theExtIter.Next();
    } else { // internal document object
      anObj = theDoc->objects()->object(theLIter.Value());
    }
  }
  return anObj;
}

list<ObjectPtr> Model_AttributeRefList::list()
{
  std::list<ObjectPtr> aResult;
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
      owner()->document());
  if (aDoc) {
    const TDF_LabelList& aList = myRef->List();
    TDataStd_ListIteratorOfListOfExtendedString anExtIter(myExtDocRef->List());
    for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
      aResult.push_back(iteratedObject(aLIter, anExtIter, aDoc));
    }
  }
  return aResult;
}

bool Model_AttributeRefList::isInList(const ObjectPtr& theObj)
{
  if(!theObj.get()) {
    return false;
  }
  if (theObj->document() == owner()->document()) { // this document object
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
  } else { // external document object
    // create new lists because for the current moment remove one of the duplicated elements
    // from the list is buggy
    std::ostringstream anIdString; // string with document Id
    anIdString<<theObj->document()->id();
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theObj->data());
    TCollection_AsciiString anEntry;
    TDF_Tool::Entry(aData->label().Father(), anEntry);
    bool aFound = false;
    TDataStd_ListIteratorOfListOfExtendedString anExtIter(myExtDocRef->List());
    for (; anExtIter.More(); anExtIter.Next()) {
      if (anExtIter.Value() == anIdString.str().c_str()) {
        anExtIter.Next();
        if (anExtIter.Value() == anEntry) { // fully maches
          return true;
        }
      } else {
        anExtIter.Next();
      }
    }
  }
  return false;
}

ObjectPtr Model_AttributeRefList::object(const int theIndex, const bool theWithEmpty) const
{
  std::shared_ptr<Model_Document> aDoc = 
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  if (aDoc) {
    int anIndex = -1;
    TDataStd_ListIteratorOfListOfExtendedString anExtIter(myExtDocRef->List());
    for (TDF_ListIteratorOfLabelList aLIter(myRef->List()); aLIter.More(); aLIter.Next()) {
      if (theWithEmpty || (!aLIter.Value().IsNull() && !aLIter.Value().IsRoot()))
        anIndex++;
      if (anIndex == theIndex) {
        return iteratedObject(aLIter, anExtIter, aDoc);
      }
      if (aLIter.Value() == myRef->Label()) {
        anExtIter.Next();
        anExtIter.Next();
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
  if (!theLabel.FindAttribute(TDataStd_ExtStringList::GetID(), myExtDocRef)) {
    myExtDocRef = TDataStd_ExtStringList::Set(theLabel);
  }
}
