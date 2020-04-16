// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "Model_AttributeRefList.h"
#include "Model_Application.h"
#include "Model_Data.h"
#include "Model_Objects.h"
#include <ModelAPI_Feature.h>
#include <TDF_ListIteratorOfLabelList.hxx>
#include <TDF_Tool.hxx>
#include <TDataStd_ListIteratorOfListOfExtendedString.hxx>

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

  if (myHashUsed) {
    myHashObjects.insert(theObject);
    myHashIndex[myRef->Extent() - 1] = theObject;
    myHashIndexNoEmpty[int(myHashIndexNoEmpty.size())] = theObject;
  }

  // do it before the transaction finish to make just created/removed objects know dependencies
  // and reference from composite feature is removed automatically
  ADD_BACK_REF(theObject);
  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeRefList::remove(ObjectPtr theObject)
{
  eraseHash();
  if (theObject.get() != NULL) {
    if (owner()->document() == theObject->document()) {
      std::shared_ptr<Model_Data> aData;
      aData = std::dynamic_pointer_cast<Model_Data>(theObject->data());
      myRef->Remove(aData->label().Father());
      REMOVE_BACK_REF(theObject);
      owner()->data()->sendAttributeUpdated(this);
    } else {
      // LCOV_EXCL_START
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
            if (anExtIter2.Value() == anEntry) { // fully matches, so, remove(don't copy)
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
      // LCOV_EXCL_STOP
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
  eraseHash();
  owner()->data()->sendAttributeUpdated(this);
}

int Model_AttributeRefList::size(const bool theWithEmpty) const
{
  if (theWithEmpty)
    return myRef->Extent();

  if (myHashUsed)
    return int(myHashIndexNoEmpty.size());

  int aResult = 0;
  const TDF_LabelList& aList = myRef->List();
  for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
    if (!aLIter.Value().IsNull() && !aLIter.Value().IsRoot()) aResult++;
  }
  return aResult;
}

bool Model_AttributeRefList::isInitialized()
{
  if (size(false) == 0) {
    // empty list is not initialized list: sketch will be not valid after add/undo
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

std::list<ObjectPtr> Model_AttributeRefList::list()
{
  createHash();
  std::list<ObjectPtr> aResult;
  std::map<int, ObjectPtr>::iterator aHashIter = myHashIndex.begin();
  for(; aHashIter != myHashIndex.end(); aHashIter++) {
    aResult.push_back(aHashIter->second);
  }
  return aResult;
}

bool Model_AttributeRefList::isInList(const ObjectPtr& theObj)
{
  if(!theObj.get()) {
    return false;
  }
  createHash();
  return myHashObjects.count(theObj) != 0;
}

ObjectPtr Model_AttributeRefList::object(const int theIndex, const bool theWithEmpty)
{
  createHash();
  std::map<int, ObjectPtr>::iterator aFind;
  if (theWithEmpty) {
    aFind = myHashIndex.find(theIndex);
    if (aFind == myHashIndex.end())
      return ObjectPtr();
  } else {
    aFind = myHashIndexNoEmpty.find(theIndex);
    if (aFind == myHashIndexNoEmpty.end())
      return ObjectPtr();
  }
  return aFind->second;
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
      eraseHash();
      ADD_BACK_REF(theNew);
      if (myRef->InsertAfter(aNewLab, aCurrentLab)) {
        myRef->Remove(aCurrentLab);
        REMOVE_BACK_REF(theCurrent);
      }
      owner()->data()->sendAttributeUpdated(this);
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
      eraseHash();
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
    // collect labels that will be removed
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
      eraseHash();
      owner()->data()->sendAttributeUpdated(this);
    }
  }
}

Model_AttributeRefList::Model_AttributeRefList(TDF_Label& theLabel)
{
  myLab = theLabel;
  reinit();
}

void Model_AttributeRefList::reinit()
{
  myIsInitialized = myLab.FindAttribute(TDataStd_ReferenceList::GetID(), myRef) == Standard_True;
  if (!myIsInitialized) {
    myRef = TDataStd_ReferenceList::Set(myLab);
  }
  if (!myLab.FindAttribute(TDataStd_ExtStringList::GetID(), myExtDocRef)) {
    myExtDocRef = TDataStd_ExtStringList::Set(myLab);
  }
  eraseHash();
}

void Model_AttributeRefList::createHash()
{
  if (myHashUsed)
    return;
  eraseHash();
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
    owner()->document());
  if (aDoc) {
    const TDF_LabelList& aList = myRef->List();
    TDataStd_ListIteratorOfListOfExtendedString anExtIter(myExtDocRef->List());
    for (TDF_ListIteratorOfLabelList aLIter(aList); aLIter.More(); aLIter.Next()) {
      ObjectPtr anObj = iteratedObject(aLIter, anExtIter, aDoc);
      myHashIndex[int(myHashIndex.size())] = anObj;
      if (anObj.get()) {
        myHashIndexNoEmpty[int(myHashIndexNoEmpty.size())] = anObj;
        myHashObjects.insert(anObj);
      }
    }
  }
  if (!myHashObjects.empty()) // on open document with multi-rotation referenced have no results
    myHashUsed = true;
}

void Model_AttributeRefList::eraseHash()
{
  myHashObjects.clear();
  myHashIndex.clear();
  myHashIndexNoEmpty.clear();
  myHashUsed = false;
}
