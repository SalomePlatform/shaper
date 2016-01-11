// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeSelectionList.cpp
// Created:     22 Oct 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeSelectionList.h"
#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"

#include <TDF_ChildIterator.hxx>
#include <TopAbs_ShapeEnum.hxx>

#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>

using namespace std;

void Model_AttributeSelectionList::append(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
    const bool theTemporarily)
{
  // do not use the degenerated edge as a shape, a list is not incremented in this case
  if (theSubShape.get() && !theSubShape->isNull() && theSubShape->isEdge()) {
    const TopoDS_Shape& aSubShape = theSubShape->impl<TopoDS_Shape>();
    if (aSubShape.ShapeType() == TopAbs_EDGE && BRep_Tool::Degenerated(TopoDS::Edge(aSubShape))) {
      return;
    }
  }

  int aNewTag = mySize->Get() + 1;
  TDF_Label aNewLab = mySize->Label().FindChild(aNewTag);

  std::shared_ptr<Model_AttributeSelection> aNewAttr = 
    std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aNewLab));
  if (owner()) {
    aNewAttr->setObject(owner());
  }
  aNewAttr->setID(id());
  mySize->Set(aNewTag);
  aNewAttr->setValue(theContext, theSubShape, theTemporarily);
  if (theTemporarily)
    myTmpAttr = aNewAttr;
  else 
    myTmpAttr.reset();
  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeSelectionList::append(
  const std::string theNamingName, const std::string& theType)
{
  int aNewTag = mySize->Get() + 1;
  TDF_Label aNewLab = mySize->Label().FindChild(aNewTag);

  std::shared_ptr<Model_AttributeSelection> aNewAttr = 
    std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aNewLab));
  if (owner()) {
    aNewAttr->setObject(owner());
  }
  aNewAttr->setID(id());
  mySize->Set(aNewTag);
  aNewAttr->selectSubShape(theType.empty() ? selectionType() : theType, theNamingName);
  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeSelectionList::removeLast() 
{
  int anOldSize = mySize->Get();
  if (anOldSize != 0) {
    mySize->Set(anOldSize - 1);
    TDF_Label aLab = mySize->Label().FindChild(anOldSize);
    std::shared_ptr<Model_AttributeSelection> aOldAttr = 
      std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aLab));
    aOldAttr->setObject(owner());
    REMOVE_BACK_REF(aOldAttr->context());
    aLab.ForgetAllAttributes(Standard_True);
    myTmpAttr.reset();
    owner()->data()->sendAttributeUpdated(this);
  }
}

void Model_AttributeSelectionList::remove(const std::set<int>& theIndices)
{
}

int Model_AttributeSelectionList::size()
{
  return mySize->Get();
}

const std::string Model_AttributeSelectionList::selectionType() const
{
  return TCollection_AsciiString(mySelectionType->Get()).ToCString();
}

void Model_AttributeSelectionList::setSelectionType(const std::string& theType)
{
  mySelectionType->Set(theType.c_str());
}

std::shared_ptr<ModelAPI_AttributeSelection> 
  Model_AttributeSelectionList::value(const int theIndex)
{
  if (myTmpAttr.get() && theIndex == size() - 1) {
    return myTmpAttr;
  }
  TDF_Label aLabel = mySize->Label().FindChild(theIndex + 1);
  // create a new attribute each time, by demand
  // supporting of old attributes is too slow (synch each time) and buggy on redo
  // (if attribute is deleted and created, the abort updates attriute and makes the Attr invalid)
  std::shared_ptr<Model_AttributeSelection> aNewAttr = 
    std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aLabel));
  if (owner()) {
    aNewAttr->setObject(owner());
  }
  return aNewAttr;
}

void Model_AttributeSelectionList::clear()
{
  if (mySize->Get() != 0) {
    mySize->Set(0);
    myTmpAttr.reset();
    TDF_ChildIterator aSubIter(mySize->Label());
    for(; aSubIter.More(); aSubIter.Next()) {
      TDF_Label aLab = aSubIter.Value();
      std::shared_ptr<Model_AttributeSelection> aNewAttr = 
        std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aLab));
      if (owner()) {
        aNewAttr->setObject(owner());
      }
      REMOVE_BACK_REF(aNewAttr->context());

      aLab.ForgetAllAttributes(Standard_True);
    }
    owner()->data()->sendAttributeUpdated(this);
  }
}

bool Model_AttributeSelectionList::isInitialized()
{
  if (size() == 0) { // empty list is not initialized list: sketch will be not valid after add/undo
    return false;
  }
  return ModelAPI_AttributeSelectionList::isInitialized();
}

Model_AttributeSelectionList::Model_AttributeSelectionList(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_Integer::GetID(), mySize) == Standard_True;
  if (!myIsInitialized) {
    mySize = TDataStd_Integer::Set(theLabel, 0);
    mySelectionType = TDataStd_Comment::Set(theLabel, "");
  } else { // recollect mySubs
    theLabel.FindAttribute(TDataStd_Comment::GetID(), mySelectionType);
  }
}
