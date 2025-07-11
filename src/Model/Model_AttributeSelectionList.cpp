// Copyright (C) 2014-2025  CEA, EDF
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

#include "Model_AttributeSelectionList.h"
#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include "Model_Objects.h"

#include "ModelAPI_Session.h"
#include "ModelAPI_FiltersFactory.h"
#include "ModelAPI_FiltersFeature.h"

#include <Events_Loop.h>

#include <GeomAPI_Pnt.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_ShapeIterator.h>

#include <TDF_AttributeIterator.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_RelocationTable.hxx>
#include <TDF_DeltaOnAddition.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Edge.hxx>
#include <BRep_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Iterator.hxx>
#include <TNaming_NamedShape.hxx>
#include <NCollection_List.hxx>

/// GUID for UAttribute that indicates the list has "To add all elements that share the same
/// topology" flag enabled
static const Standard_GUID kIS_GEOMETRICAL_SELECTION("f16987b6-e6c8-435c-99fa-03a7e0b06e83");

/// GUID for TDataStd_ReferenceList attribute that refers the selection filters feature
static const Standard_GUID kSELECTION_FILTERS_REF("ea5b1dbf-a740-4a0b-a1b2-3c3c756e691a");


void Model_AttributeSelectionList::append(
    const ObjectPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
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
    aNewAttr->setParent(this);
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
  const std::wstring& theNamingName, const std::string& theType)
{
  int aNewTag = mySize->Get() + 1;
  TDF_Label aNewLab = mySize->Label().FindChild(aNewTag);

  std::shared_ptr<Model_AttributeSelection> aNewAttr =
    std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aNewLab));
  if (owner()) {
    aNewAttr->setObject(owner());
    aNewAttr->setParent(this);
  }
  aNewAttr->setID(id());
  mySize->Set(aNewTag);
  aNewAttr->selectSubShape(theType.empty() ? selectionType() : theType, theNamingName);
  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeSelectionList::append(const GeomPointPtr& thePoint, const std::string& theType)
{
  int aNewTag = mySize->Get() + 1;
  TDF_Label aNewLab = mySize->Label().FindChild(aNewTag);

  std::shared_ptr<Model_AttributeSelection> aNewAttr =
    std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aNewLab));
  if (owner()) {
    aNewAttr->setObject(owner());
    aNewAttr->setParent(this);
  }
  aNewAttr->setID(id());
  mySize->Set(aNewTag);
  aNewAttr->selectSubShape(theType, thePoint);
  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeSelectionList::append(const std::string& theType,
  const std::wstring& theContextName, const int theIndex)
{
  int aNewTag = mySize->Get() + 1;
  TDF_Label aNewLab = mySize->Label().FindChild(aNewTag);

  std::shared_ptr<Model_AttributeSelection> aNewAttr =
    std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aNewLab));
  if (owner()) {
    aNewAttr->setObject(owner());
    aNewAttr->setParent(this);
  }
  aNewAttr->setID(id());
  mySize->Set(aNewTag);
  aNewAttr->selectSubShape(theType, theContextName, theIndex);
  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeSelectionList::removeTemporaryValues()
{
  if (myTmpAttr.get()) {
    myTmpAttr.reset();
  }
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

  // TNaming_Iterator iterates in reversed way than it was put in Builder, so, keep it in container
  // and iterate from end to begin
  NCollection_List<TopoDS_Shape> aOlds;
  NCollection_List<TopoDS_Shape> aNews;
  NCollection_List<TNaming_Evolution> aStatuses;

  TNaming_Iterator anIt (theFrom);
  for ( ;anIt.More() ; anIt.Next()) {
    aOlds.Prepend(anIt.OldShape());
    aNews.Prepend(anIt.NewShape());
    aStatuses.Prepend(anIt.Evolution());
  }

  NCollection_List<TopoDS_Shape>::Iterator aOldIter(aOlds);
  NCollection_List<TopoDS_Shape>::Iterator aNewIter(aNews);
  NCollection_List<TNaming_Evolution>::Iterator aStatIter(aStatuses);
  for(; aOldIter.More(); aOldIter.Next(), aNewIter.Next(), aStatIter.Next()) {
    switch (aStatIter.Value()) {
    case TNaming_PRIMITIVE :
      B.Generated(aNewIter.Value());
      break;
    case TNaming_GENERATED :
      B.Generated(aOldIter.Value(), aNewIter.Value());
      break;
    case TNaming_MODIFY :
      B.Modify(aOldIter.Value(), aNewIter.Value());
      break;
    case TNaming_DELETE :
      B.Delete (aOldIter.Value());
      break;
    case TNaming_SELECTED :
      B.Select(aNewIter.Value(), aOldIter.Value());
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
      if (aTargetAttr->ID() != anAttrIter.Value()->ID())
        aTargetAttr->SetID(anAttrIter.Value()->ID());
      theDestination.AddAttribute(aTargetAttr);
    }
    // for named shape copy exact shapes (in NamedShape Paste method the CopyTool is used)
    Handle(TNaming_NamedShape) aNS = Handle(TNaming_NamedShape)::DownCast(anAttrIter.Value());
    if (aNS.IsNull()) {
      // no special relocation, empty map, but self-relocation is on: copy references w/o changes
      Handle(TDF_RelocationTable) aRelocTable = new TDF_RelocationTable(Standard_True);
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
  // iterate one by one and shifting the removed indices
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

void Model_AttributeSelectionList::copyTo(AttributeSelectionListPtr theTarget) const
{
  std::shared_ptr<Model_AttributeSelectionList> aTarget =
    std::dynamic_pointer_cast<Model_AttributeSelectionList>(theTarget);
  if (aTarget) {
    copyAttrs(myLab, aTarget->myLab);
    aTarget->reinit();
  }
}

int Model_AttributeSelectionList::size()
{
  return mySize->Get();
}

// LCOV_EXCL_START

// returns true if theShape is same with theInList or is contained in it (a compound)
static bool isIn(GeomShapePtr theInList, GeomShapePtr theShape) {
  if (theShape->isSame(theInList))
    return true;
  if (theInList.get() && theInList->shapeType() == GeomAPI_Shape::COMPOUND) {
    for(GeomAPI_ShapeIterator anIter(theInList); anIter.more(); anIter.next()) {
      if (!anIter.current()->isNull() && anIter.current()->isSame(theShape))
        return true;
    }
  }
  return false;
}

bool Model_AttributeSelectionList::isInList(const ObjectPtr& theContext,
                                            const std::shared_ptr<GeomAPI_Shape>& theSubShape,
                                            const bool /*theTemporarily*/)
{
  if (myIsCashed) { // the cashing is active
    if (theContext.get()) {
      std::map<ObjectPtr, std::list<std::shared_ptr<GeomAPI_Shape> > >::iterator aContext =
        myCash.find(theContext);
      if (aContext != myCash.end()) {
        // iterate shapes because "isSame" method must be called for each shape
        std::list<std::shared_ptr<GeomAPI_Shape> >::iterator aShapes = aContext->second.begin();
        for(; aShapes != aContext->second.end(); aShapes++) {
          if (!theSubShape.get()) {
            if (!aShapes->get())
              return true;
            ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aContext->first);
            FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(aContext->first);
            if ((aRes.get() && (*aShapes)->isSame(aRes->shape())) || aFeat.get())
              return true;
          } else {
            // we need to call here isSame instead of isEqual to do not check shapes orientation
            if (isIn(*aShapes, theSubShape))
              return true;
          }
        }
      }
      return false;
    }
  }
  // no-cash method
  bool isFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(theContext).get() != NULL;
  ResultPtr aRes;
  if (!isFeature)
    aRes = std::dynamic_pointer_cast<ModelAPI_Result>(theContext);
  for(int anIndex = size() - 1; anIndex >= 0; anIndex--) {
    AttributeSelectionPtr anAttr = value(anIndex);
    if (anAttr.get()) {
      if (isFeature && anAttr->contextFeature() == theContext)
        return true; // for the feature value should not be compared
      if (!isFeature && anAttr->context() == theContext) {
        // contexts are equal, so, check that values are also
        std::shared_ptr<GeomAPI_Shape> aValue = anAttr->value();
        if (!theSubShape.get()) {
          if (!aValue.get() || (aRes.get() && aValue->isSame(aRes->shape())))
            return true;
        } else {
          // we need to call here isSame instead of isEqual to do not check shapes orientation
          if (isIn(aValue, theSubShape)) // shapes are equal
            return true;
        }
      }
    }
  }
  return false;
}
// LCOV_EXCL_STOP

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
  // supporting of old attributes is too slow (sync each time) and buggy on redo
  // (if attribute is deleted and created, the abort updates attribute and makes the Attr invalid)
  std::shared_ptr<Model_AttributeSelection> aNewAttr =
    std::shared_ptr<Model_AttributeSelection>(new Model_AttributeSelection(aLabel));
  if (owner()) {
    aNewAttr->setObject(owner());
    aNewAttr->setParent(this);
  }
  aNewAttr->setID(id());
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
        aNewAttr->setParent(this);
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
: ModelAPI_AttributeSelectionList(), myLab(theLabel)
{
  reinit();
}

void Model_AttributeSelectionList::reinit()
{
  myIsInitialized = myLab.FindAttribute(TDataStd_Integer::GetID(), mySize) == Standard_True;
  if (!myIsInitialized) {
    mySize = TDataStd_Integer::Set(myLab, 0);
    mySelectionType = TDataStd_Comment::Set(myLab, "");
  } else { // recollect mySubs
    myLab.FindAttribute(TDataStd_Comment::GetID(), mySelectionType);
  }
  myIsCashed = false;
}

// LCOV_EXCL_START
void Model_AttributeSelectionList::cashValues(const bool theEnabled)
{
  myIsCashed = theEnabled;
  myCash.clear(); // empty list as indicator that cash is not used
  if (theEnabled) {
    for(int anIndex = size() - 1; anIndex >= 0; anIndex--) {
      AttributeSelectionPtr anAttr = value(anIndex);
      if (anAttr.get()) {
        myCash[anAttr->contextObject()].push_back(anAttr->value());
      }
    }
  }
}
// LCOV_EXCL_STOP

bool Model_AttributeSelectionList::isGeometricalSelection() const
{
  return myLab.IsAttribute(kIS_GEOMETRICAL_SELECTION);
}

void Model_AttributeSelectionList::setGeometricalSelection(const bool theIsGeometricalSelection)
{
  if (isGeometricalSelection() == theIsGeometricalSelection)
    return; // nothing to do
  if (theIsGeometricalSelection) // store the state
    TDataStd_UAttribute::Set(myLab, kIS_GEOMETRICAL_SELECTION);
  else
    myLab.ForgetAttribute(kIS_GEOMETRICAL_SELECTION);
  std::set<int> anIndiciesToRemove;  // Update list according to the flag
  if (theIsGeometricalSelection) { // all objects with same geometry must be combined into single
    std::list<AttributeSelectionPtr> anAttributes; // collect attributes with geometrical compounds
    for(int anIndex = 0; anIndex < size(); anIndex++) {
      AttributeSelectionPtr anAttr = value(anIndex);
      if (!anAttr.get() || !anAttr->context().get())
        continue;
      anAttr->combineGeometrical();
      if (!anAttr->value().get() || anAttr->value()->shapeType() != GeomAPI_Shape::COMPOUND)
        continue;
      // check it is equal to some other attribute already presented in the list
      std::list<AttributeSelectionPtr>::iterator anAttrIter = anAttributes.begin();
      for(; anAttrIter != anAttributes.end(); anAttrIter++) {
        if (anAttr->context() == (*anAttrIter)->context() &&
            anAttr->namingName() == (*anAttrIter)->namingName()) {
          anIndiciesToRemove.insert(anIndex);
          break;
        }
      }
      if (anAttrIter == anAttributes.end()) // not removed, so, add to the compare-list
        anAttributes.push_back(anAttr);
    }
  } else { // all objects with same geometry must be divided into separated sub-attributes
    int anInitialSize = size();
    for(int anIndex = 0; anIndex < anInitialSize; anIndex++) {
      AttributeSelectionPtr anAttr = value(anIndex);
      if (!anAttr.get() || !anAttr->context().get())
        continue;
      GeomShapePtr aValue = anAttr->value();
      if (!aValue.get() || aValue->shapeType() != GeomAPI_Shape::COMPOUND)
        continue;
      for(GeomAPI_ShapeIterator anIter(aValue); anIter.more(); anIter.next()) {
        append(anAttr->context(), anIter.current());
      }
      anIndiciesToRemove.insert(anIndex);
    }
  }
  remove(anIndiciesToRemove);
  myIsCashed = false;
  myCash.clear(); // empty list as indicator that cash is not used
  owner()->data()->sendAttributeUpdated(this);
}

FiltersFeaturePtr Model_AttributeSelectionList::filters() const
{
  Handle(TDataStd_ReferenceList) aRef;
  if (myLab.FindAttribute(kSELECTION_FILTERS_REF, aRef) && !aRef->IsEmpty()) {
    if (owner().get()) {
      std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(
        owner()->document());
      if (aDoc) {
        const TDF_Label& aRefLab = aRef->First();
        if (!aRefLab.IsNull()) {  // it may happen with old document, issue #285
          ObjectPtr anObj = aDoc->objects()->object(aRefLab);
          FiltersFeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_FiltersFeature>(anObj);
          if (aFeat.get()) {
            aFeat->setAttribute(owner()->data()->attribute(id()));
            return aFeat;
          }
        }
      }
    }
  }
  return FiltersFeaturePtr(); // null pointer if nothing is defined
}

void Model_AttributeSelectionList::setFilters(FiltersFeaturePtr theFeature)
{
  Handle(TDataStd_ReferenceList) aRef = TDataStd_ReferenceList::Set(myLab, kSELECTION_FILTERS_REF);
  if (theFeature.get()) {
    std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(theFeature->data());
    if (aData->isValid())  {
      TDF_Label anObjLab = aData->label().Father(); // object label
      if (!aRef->IsEmpty())
        aRef->Clear();
      aRef->Append(anObjLab);
      theFeature->setAttribute(owner()->data()->attribute(id()));
      return;
    }
  }
  // remove attribute if something is wrong
  myLab.ForgetAttribute(TDataStd_ReferenceList::GetID());
}

void Model_AttributeSelectionList::applyFilters()
{
  FiltersFeaturePtr aFiltersFeat = this->filters();
  if (!aFiltersFeat->filters().empty()) {
    // finish operation to make sure the selection is done on the current state of the history
    static Events_ID anID = Events_Loop::eventByName("FinishOperation");
    std::shared_ptr<Events_Message> aMsg(new Events_Message(anID, this));
    Events_Loop* aLoop = Events_Loop::loop();
    aLoop->send(aMsg);

    // obtaining the selected shapes from the ModelAPI_FiltersFactory select method
    GeomAPI_Shape::ShapeType aShapeType = GeomAPI_Shape::shapeTypeByStr(this->selectionType());
    SessionPtr aSession = ModelAPI_Session::get();
    std::list< std::pair< ResultPtr, GeomShapePtr > > aResList = aSession->filters()->select(aFiltersFeat, aShapeType);
    std::list< std::pair<ResultPtr, GeomShapePtr> >::const_iterator itSelected = aResList.cbegin();

    // clearing the content of the AttrList
    this->clear();

    // filling the content of the AttrList with the results of the select() method
    for (; itSelected != aResList.cend(); itSelected++) {
      ResultPtr aCurRes = (*itSelected).first;
      GeomShapePtr aSubShape = (*itSelected).second;
      this->append(aCurRes, aSubShape);
    }
  }
}

void Model_AttributeSelectionList::removeFilters()
{
  FiltersFeaturePtr aFilters = this->filters();
  if (aFilters.get()) {
    if (!aFilters->filters().empty()) {
      std::list<std::string> aFiltersList = aFilters->filters();
      std::list<std::string>::iterator aFilterName = aFiltersList.begin();
      for(; aFilterName != aFiltersList.end(); aFilterName++) {
        aFilters->removeFilter(*aFilterName);
      }
    }
  }
  aFilters.reset();
  myLab.ForgetAttribute(kSELECTION_FILTERS_REF);
}