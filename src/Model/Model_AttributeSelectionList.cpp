// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeSelectionList.h
// Created:     22 Oct 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeSelectionList.h"
#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"

#include <TDF_ChildIterator.hxx>
#include <TopAbs_ShapeEnum.hxx>

using namespace std;

void Model_AttributeSelectionList::append(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  int aNewTag = mySize->Get() + 1;
  TDF_Label aNewLab = mySize->Label().FindChild(aNewTag);

  std::shared_ptr<Model_AttributeSelection> aNewAttr = 
    std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aNewLab));
  if (owner()) {
    aNewAttr->setObject(owner());
  }
  mySize->Set(aNewTag);
  aNewAttr->setValue(theContext, theSubShape);
  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeSelectionList::append(std::string theNamingName)
{
  int aNewTag = mySize->Get() + 1;
  TDF_Label aNewLab = mySize->Label().FindChild(aNewTag);

  std::shared_ptr<Model_AttributeSelection> aNewAttr = 
    std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aNewLab));
  if (owner()) {
    aNewAttr->setObject(owner());
  }
  mySize->Set(aNewTag);
  TopAbs_ShapeEnum aType = (TopAbs_ShapeEnum)selectionType();
  string aTypeName;
  switch(aType) {
  case TopAbs_VERTEX: aTypeName = "VERT"; break;
  case TopAbs_EDGE: aTypeName = "EDGE"; break;
  case TopAbs_WIRE: aTypeName = "WIRE"; break;
  case TopAbs_FACE: aTypeName = "FACE"; break;
  case TopAbs_SHELL: aTypeName = "SHEL"; break;
  case TopAbs_SOLID: aTypeName = "SOLD"; break;
  case TopAbs_COMPOUND: aTypeName = "COMP"; break;
  case TopAbs_COMPSOLID: aTypeName = "COMS"; break;
  default: 
    return; // invalid case => empty new attribute
  };
  aNewAttr->selectSubShape(aTypeName, theNamingName);
  owner()->data()->sendAttributeUpdated(this);
}

int Model_AttributeSelectionList::size()
{
  return mySize->Get();
}

int Model_AttributeSelectionList::selectionType()
{
  return (int) mySelectionType->Get();
}

void Model_AttributeSelectionList::setSelectionType(int theType)
{
  mySelectionType->Set((double) theType);
}

std::shared_ptr<ModelAPI_AttributeSelection> 
  Model_AttributeSelectionList::value(const int theIndex)
{
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
    TDF_ChildIterator aSubIter(mySize->Label());
    for(; aSubIter.More(); aSubIter.Next()) {
      aSubIter.Value().ForgetAllAttributes(Standard_True);
    }
    owner()->data()->sendAttributeUpdated(this);
  }
}

Model_AttributeSelectionList::Model_AttributeSelectionList(TDF_Label& theLabel)
{
  myIsInitialized = theLabel.FindAttribute(TDataStd_Integer::GetID(), mySize) == Standard_True;
  if (!myIsInitialized) {
    mySize = TDataStd_Integer::Set(theLabel, 0);
    mySelectionType = TDataStd_Real::Set(theLabel, 0);
  } else { // recollect mySubs
    theLabel.FindAttribute(TDataStd_Real::GetID(), mySelectionType);
  }
}
