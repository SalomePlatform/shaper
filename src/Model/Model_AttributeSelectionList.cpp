// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeSelectionList.cpp
// Created:     22 Oct 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeSelectionList.h"
#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"

#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_RelocationTable.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>

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

  if (myIsCashed && !theTemporarily) {
    myCash[theContext].push_back(theSubShape);
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

// copies named shapes: we need the implementation of this 
static void CopyNS(const Handle(TNaming_NamedShape)& theFrom,
  const Handle(TDF_Attribute)& theTo)
{ 
  TDF_Label aLab = theTo->Label();
  TNaming_Builder B(aLab);

  TNaming_Iterator It (theFrom);
  for ( ;It.More() ; It.Next()) {
    const TopoDS_Shape& OS     = It.OldShape();
    const TopoDS_Shape& NS     = It.NewShape();
    TNaming_Evolution   Status = It.Evolution();

    switch (Status) {
    case TNaming_PRIMITIVE :
      B.Generated(NS);
      break;
    case TNaming_GENERATED :
      B.Generated(OS, NS);
      break;
    case TNaming_MODIFY : 
      B.Modify(OS, NS);
      break;
    case TNaming_DELETE : 
      B.Delete (OS);
      break;
    case TNaming_SELECTED :
      B.Select(NS, OS);
      break;
    default:
      break;
    }
  }
}

/// makes copy of all attributes on the given label and all sub-labels
static void copyAttrs(TDF_Label theSource, TDF_Label theDestination) {
  TDF_AttributeIterator anAttrIter(theSource);
  for(; anAttrIter.More(); anAttrIter.Next()) {
    Handle(TDF_Attribute) aTargetAttr;
    if (!theDestination.FindAttribute(anAttrIter.Value()->ID(), aTargetAttr)) {
      // create a new attribute if not yet exists in the destination
	    aTargetAttr = anAttrIter.Value()->NewEmpty();
      theDestination.AddAttribute(aTargetAttr);
    }
    // for named shape copy exact shapes (in NamedShape Paste method the CopyTool is used)
    Handle(TNaming_NamedShape) aNS = Handle(TNaming_NamedShape)::DownCast(anAttrIter.Value());
    if (aNS.IsNull()) {
      Handle(TDF_RelocationTable) aRelocTable = new TDF_RelocationTable(); // no relocation, empty map
      anAttrIter.Value()->Paste(aTargetAttr, aRelocTable);
    } else {
      CopyNS(aNS, aTargetAttr);
    }
  }
  // copy the sub-labels content
  TDF_ChildIterator aSubLabsIter(theSource);
  for(; aSubLabsIter.More(); aSubLabsIter.Next()) {
    copyAttrs(aSubLabsIter.Value(), theDestination.FindChild(aSubLabsIter.Value().Tag()));
  }
}

void Model_AttributeSelectionList::remove(const std::set<int>& theIndices)
{
  int anOldSize = mySize->Get();
  int aRemoved = 0;
  // iterate one by one and shifting the removed indicies
  for(int aCurrent = 0; aCurrent < anOldSize; aCurrent++) {
    if (theIndices.find(aCurrent) == theIndices.end()) { // not removed
      if (aRemoved) { // but must be shifted to the removed position
        TDF_Label aSource = mySize->Label().FindChild(aCurrent + 1);
        TDF_Label aDest = mySize->Label().FindChild(aCurrent + 1 - aRemoved);
        copyAttrs(aSource, aDest);
        // remove the moved source
        aSource.ForgetAllAttributes(Standard_True);
      }
    } else { // this is removed, so remove everything from this label
      TDF_Label aLab = mySize->Label().FindChild(aCurrent + 1);
      std::shared_ptr<Model_AttributeSelection> aOldAttr = 
        std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aLab));
      aOldAttr->setObject(owner());
      REMOVE_BACK_REF(aOldAttr->context());
      aLab.ForgetAllAttributes(Standard_True);
      myTmpAttr.reset();
      aRemoved++;
    }
  }
  if (aRemoved) { // remove was performed, so, update the size and this attribute
    mySize->Set(anOldSize - aRemoved);
    owner()->data()->sendAttributeUpdated(this);
  }
}

int Model_AttributeSelectionList::size()
{
  return mySize->Get();
}

bool Model_AttributeSelectionList::isInList(const ResultPtr& theContext,
                                            const std::shared_ptr<GeomAPI_Shape>& theSubShape,
                                            const bool theTemporarily)
{
  if (myIsCashed) { // the cashing is active
    std::map<ResultPtr, std::list<std::shared_ptr<GeomAPI_Shape> > >::iterator aContext =
      myCash.find(theContext);
    if (aContext != myCash.end()) {
      // iterate shapes because "isEqual" method must be called for each shape
      std::list<std::shared_ptr<GeomAPI_Shape> >::iterator aShapes = aContext->second.begin();
      for(; aShapes != aContext->second.end(); aShapes++) {
        if (!theSubShape.get()) {
          if (!aShapes->get())
            return true;
        } else {
          if (theSubShape->isEqual(*aShapes))
            return true;
        }
      }
    }
    return false;
  }
  // no-cash method
  for(int anIndex = size() - 1; anIndex >= 0; anIndex--) {
    AttributeSelectionPtr anAttr = value(anIndex);
    if (anAttr.get()) {
      if (anAttr->context() == theContext) { // contexts are equal, so, check that values are also
        std::shared_ptr<GeomAPI_Shape> aValue = anAttr->value();
        if (!aValue.get()) {
          if (!theSubShape.get()) { // both are null
            return true;
          }
        } else {
          if (aValue->isEqual(theSubShape)) // shapes are equal
            return true;
        }
      }
    }
  }
  return false;
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
  myIsCashed = false;
}

void Model_AttributeSelectionList::cashValues(const bool theEnabled)
{
  myIsCashed = theEnabled;
  myCash.clear(); // empty list as indicator that cash is not used
  if (theEnabled) {
    for(int anIndex = size() - 1; anIndex >= 0; anIndex--) {
      AttributeSelectionPtr anAttr = value(anIndex);
      if (anAttr.get()) {
        myCash[anAttr->context()].push_back(anAttr->value());
      }
    }
  }
}
