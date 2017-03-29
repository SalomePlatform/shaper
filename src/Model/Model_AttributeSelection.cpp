// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeSelection.cpp
// Created:     2 Oct 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include "Model_Document.h"
#include "Model_SelectionNaming.h"
#include <Model_Objects.h>
#include <Model_AttributeSelectionList.h>
#include <Model_ResultConstruction.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultCompSolid.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Session.h>
#include <Events_InfoMessage.h>

#include <TNaming_Selector.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_Iterator.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_Name.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TopoDS_Iterator.hxx>

//#define DEB_NAMING 1
#ifdef DEB_NAMING
#include <BRepTools.hxx>
#endif
/// added to the index in the packed map to signalize that the vertex of edge is selected
/// (multiplied by the index of the edge)
static const int kSTART_VERTEX_DELTA = 1000000;
// identifier that there is simple reference: selection equals to context
Standard_GUID kSIMPLE_REF_ID("635eacb2-a1d6-4dec-8348-471fae17cb29");
// reference to Part sub-object
Standard_GUID kPART_REF_ID("635eacb2-a1d6-4dec-8348-471fae17cb27");
// selection is invalid after recomputation
Standard_GUID kINVALID_SELECTION("bce47fd7-80fa-4462-9d63-2f58acddd49d");

// on this label is stored:
// TNaming_NamedShape - selected shape
// TNaming_Naming - topological selection information (for the body)
// TDataStd_IntPackedMap - indexes of edges in composite element (for construction)
// TDataStd_Integer - type of the selected shape (for construction)
// TDF_Reference - from ReferenceAttribute, the context
void Model_AttributeSelection::setValue(const ResultPtr& theContext,
  const std::shared_ptr<GeomAPI_Shape>& theSubShape, const bool theTemporarily)
{
  if (theTemporarily) { // just keep the stored without DF update
    myTmpContext = theContext;
    myTmpSubShape = theSubShape;
    owner()->data()->sendAttributeUpdated(this);
    return;
  } else {
    myTmpContext.reset();
    myTmpSubShape.reset();
  }

  const std::shared_ptr<GeomAPI_Shape>& anOldShape = value();
  bool isOldContext = theContext == myRef.value();
  bool isOldShape = isOldContext &&
    (theSubShape == anOldShape || (theSubShape && anOldShape && theSubShape->isEqual(anOldShape)));
  if (isOldShape) return; // shape is the same, so context is also unchanged
  // update the referenced object if needed
  if (!isOldContext) {
      myRef.setValue(theContext);
  }

  // do noth use naming if selected shape is result shape itself, but not sub-shape
  TDF_Label aSelLab = selectionLabel();
  aSelLab.ForgetAttribute(kSIMPLE_REF_ID);
  aSelLab.ForgetAttribute(kINVALID_SELECTION);

  bool isDegeneratedEdge = false;
  // do not use the degenerated edge as a shape, a null context and shape is used in the case
  if (theSubShape.get() && !theSubShape->isNull() && theSubShape->isEdge()) {
    const TopoDS_Shape& aSubShape = theSubShape->impl<TopoDS_Shape>();
    if (aSubShape.ShapeType() == TopAbs_EDGE)
      isDegeneratedEdge = BRep_Tool::Degenerated(TopoDS::Edge(aSubShape)) == Standard_True;
  }
  if (!theContext.get() || isDegeneratedEdge) {
    // to keep the reference attribute label
    TDF_Label aRefLab = myRef.myRef->Label();
    aSelLab.ForgetAllAttributes(true);
    myRef.myRef = TDF_Reference::Set(aSelLab.Father(), aSelLab.Father());
    return;
  }
  if (theContext->groupName() == ModelAPI_ResultBody::group()) {
    // do not select the whole shape for body:it is already must be in the data framework
    // equal and null selected objects mean the same: object is equal to context,
    if (theContext->shape().get() &&
        (theContext->shape()->isEqual(theSubShape) || !theSubShape.get())) {
      aSelLab.ForgetAllAttributes(true);
      TDataStd_UAttribute::Set(aSelLab, kSIMPLE_REF_ID);
    } else {
      selectBody(theContext, theSubShape);
    }
  } else if (theContext->groupName() == ModelAPI_ResultConstruction::group()) {
    aSelLab.ForgetAllAttributes(true); // to remove old selection data
    std::shared_ptr<Model_ResultConstruction> aConstruction =
      std::dynamic_pointer_cast<Model_ResultConstruction>(theContext);
    std::shared_ptr<GeomAPI_Shape> aSubShape;
    if (theSubShape.get() && !theContext->shape()->isEqual(theSubShape))
      aSubShape = theSubShape; // the whole context
    int anIndex = aConstruction->select(theSubShape, owner()->document());
    TDataStd_Integer::Set(aSelLab, anIndex);
  } else if (theContext->groupName() == ModelAPI_ResultPart::group()) {
    aSelLab.ForgetAllAttributes(true);
    TDataStd_UAttribute::Set(aSelLab, kPART_REF_ID);
    selectPart(theContext, theSubShape);
  }

  owner()->data()->sendAttributeUpdated(this);
}

void Model_AttributeSelection::removeTemporaryValues()
{
  if (myTmpContext.get() || myTmpSubShape.get()) {
    myTmpContext.reset();
    myTmpSubShape.reset();
  }
}

std::shared_ptr<GeomAPI_Shape> Model_AttributeSelection::value()
{
  GeomShapePtr aResult;
  if (myTmpContext.get() || myTmpSubShape.get()) {
    ResultConstructionPtr aResulConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(myTmpContext);
    if(aResulConstruction.get()) {
      // it is just reference to construction.
      return myTmpSubShape;
    }
    return myTmpSubShape.get() ? myTmpSubShape : myTmpContext->shape();
  }

  TDF_Label aSelLab = selectionLabel();
  if (aSelLab.IsAttribute(kINVALID_SELECTION))
    return aResult;

  if (myRef.isInitialized()) {
    if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // it is just reference to shape, not sub-shape
      ResultPtr aContext = context();
      if (!aContext.get())
        return aResult; // empty result
      return aContext->shape();
    }
    if (aSelLab.IsAttribute(kPART_REF_ID)) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(context());
      if (!aPart.get() || !aPart->isActivated())
        return std::shared_ptr<GeomAPI_Shape>(); // postponed naming needed
      Handle(TDataStd_Integer) anIndex;
      if (aSelLab.FindAttribute(TDataStd_Integer::GetID(), anIndex)) {
        if (anIndex->Get()) { // special selection attribute was created, use it
          return aPart->selectionValue(anIndex->Get());
        } else { // face with name is already in the data model, so try to take it by name
          Handle(TDataStd_Name) aName;
          if (aSelLab.FindAttribute(TDataStd_Name::GetID(), aName)) {
            std::string aSubShapeName(TCollection_AsciiString(aName->Get()).ToCString());
            std::size_t aPartEnd = aSubShapeName.find('/');
            if (aPartEnd != std::string::npos && aPartEnd != aSubShapeName.rfind('/')) {
              std::string aNameInPart = aSubShapeName.substr(aPartEnd + 1);
              int anIndex;
              std::string aType; // to reuse already existing selection the type is not needed
              return aPart->shapeInPart(aNameInPart, aType, anIndex);
            }
          }
        }
      }
    }

    Handle(TNaming_NamedShape) aSelection;
    if (aSelLab.FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
      TopoDS_Shape aSelShape = aSelection->Get();
      aResult = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape);
      aResult->setImpl(new TopoDS_Shape(aSelShape));
    } else { // for simple construction element: just shape of this construction element
      std::shared_ptr<Model_ResultConstruction> aConstr =
        std::dynamic_pointer_cast<Model_ResultConstruction>(context());
      if (aConstr) {
        Handle(TDataStd_Integer) anIndex;
        if (aSelLab.FindAttribute(TDataStd_Integer::GetID(), anIndex)) {
          if (anIndex->Get() == 0) // it is just reference to construction, nothing is in value
            return aResult;
          return aConstr->shape(anIndex->Get(), owner()->document());
        }
      }
    }
  }
  return aResult;
}

bool Model_AttributeSelection::isInvalid()
{
  return selectionLabel().IsAttribute(kINVALID_SELECTION) == Standard_True;
}

bool Model_AttributeSelection::isInitialized()
{
  if (ModelAPI_AttributeSelection::isInitialized()) { // additional checks if it is initialized
    std::shared_ptr<GeomAPI_Shape> aResult;
    if (myRef.isInitialized()) {
      TDF_Label aSelLab = selectionLabel();
      if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // it is just reference to shape, not sub-shape
        ResultPtr aContext = context();
        return aContext.get() != NULL;
      }
      Handle(TNaming_NamedShape) aSelection;
      if (selectionLabel().FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
        return !aSelection->Get().IsNull();
      } else { // for simple construction element: just shape of this construction element
        std::shared_ptr<Model_ResultConstruction> aConstr =
          std::dynamic_pointer_cast<Model_ResultConstruction>(context());
        if (aConstr.get()) {
          Handle(TDataStd_Integer) anIndex;
          if (aSelLab.FindAttribute(TDataStd_Integer::GetID(), anIndex)) {
            // for the whole shape it may return null, so, if index exists, returns true
            return true;
          }
        }
      }
    }
  }
  return false;
}

Model_AttributeSelection::Model_AttributeSelection(TDF_Label& theLabel)
  : myRef(theLabel)
{
  myIsInitialized = myRef.isInitialized();
  myParent = NULL;
}

void Model_AttributeSelection::setID(const std::string theID)
{
  myRef.setID(theID);
  ModelAPI_AttributeSelection::setID(theID);
}

ResultPtr Model_AttributeSelection::context() {
  if (myTmpContext.get() || myTmpSubShape.get()) {
    return myTmpContext;
  }

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
  // for parts there could be same-data result, so take the last enabled
  if (aResult.get() && aResult->groupName() == ModelAPI_ResultPart::group()) {
    int aSize = aResult->document()->size(ModelAPI_ResultPart::group());
    for(int a = aSize - 1; a >= 0; a--) {
      ObjectPtr aPart = aResult->document()->object(ModelAPI_ResultPart::group(), a);
      if (aPart.get() && aPart->data() == aResult->data()) {
        ResultPtr aPartResult = std::dynamic_pointer_cast<ModelAPI_Result>(aPart);
        FeaturePtr anOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
        // check that this result is not this-feature result (it is forbidden t oselect itself)
        if (anOwnerFeature.get() && anOwnerFeature->firstResult() != aPartResult) {
          return aPartResult;
        }
      }
    }
  }
  return aResult;
}


void Model_AttributeSelection::setObject(const std::shared_ptr<ModelAPI_Object>& theObject)
{
  ModelAPI_AttributeSelection::setObject(theObject);
  myRef.setObject(theObject);
}

TDF_LabelMap& Model_AttributeSelection::scope()
{
  if (myScope.IsEmpty()) { // create a new scope if not yet done
    // gets all features with named shapes that are before this feature label (before in history)
    DocumentPtr aMyDoc = owner()->document();
    std::list<std::shared_ptr<ModelAPI_Feature> > allFeatures = aMyDoc->allFeatures();
    std::list<std::shared_ptr<ModelAPI_Feature> >::iterator aFIter = allFeatures.begin();
    bool aMePassed = false;
    CompositeFeaturePtr aComposite =
      std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(owner());
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
    CompositeFeaturePtr aCompositeOwner, aCompositeOwnerOwner;
    if (aFeature.get()) {
      aCompositeOwner = ModelAPI_Tools::compositeOwner(aFeature);
      if (aCompositeOwner.get()) {
         aCompositeOwnerOwner = ModelAPI_Tools::compositeOwner(aCompositeOwner);
      }
    }
    // for group Scope is not limitet: this is always up to date objects
    bool isGroup = aFeature.get() && aFeature->getKind() == "Group";
    for(; aFIter != allFeatures.end(); aFIter++) {
      if (*aFIter == owner()) {  // the left features are created later (except subs of composite)
        aMePassed = true;
        continue;
      }
      if (isGroup) aMePassed = false;
      bool isInScope = !aMePassed;
      if (!isInScope && aComposite.get()) {
        // try to add sub-elements of composite if this is composite
        if (aComposite->isSub(*aFIter))
          isInScope = true;
      }
      // remove the composite-owner of this feature (sketch in extrusion-cut)
      if (isInScope && (aCompositeOwner == *aFIter || aCompositeOwnerOwner == *aFIter))
        isInScope = false;

      if (isInScope && aFIter->get() && (*aFIter)->data()->isValid()) {
        TDF_Label aFeatureLab = std::dynamic_pointer_cast<Model_Data>(
          (*aFIter)->data())->label().Father();
        TDF_ChildIDIterator aNSIter(aFeatureLab, TNaming_NamedShape::GetID(), true);
        for(; aNSIter.More(); aNSIter.Next()) {
          Handle(TNaming_NamedShape) aNS = Handle(TNaming_NamedShape)::DownCast(aNSIter.Value());
          if (!aNS.IsNull() && aNS->Evolution() != TNaming_SELECTED) {
            myScope.Add(aNS->Label());
          }
        }
      }
    }
    // also add all naming labels created for external constructions
    std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(aMyDoc);
    TDF_Label anExtDocLab = aDoc->extConstructionsLabel();
    TDF_ChildIDIterator aNSIter(anExtDocLab, TNaming_NamedShape::GetID(), true);
    for(; aNSIter.More(); aNSIter.Next()) {
      Handle(TNaming_NamedShape) aNS = Handle(TNaming_NamedShape)::DownCast(aNSIter.Value());
      if (!aNS.IsNull() && aNS->Evolution() != TNaming_SELECTED) {
        myScope.Add(aNS->Label());
      }
    }
  }
  return myScope;
}

/// Sets the invalid flag if flag is false, or removes it if "true"
/// Returns theFlag
static bool setInvalidIfFalse(TDF_Label& theLab, const bool theFlag) {
  if (theFlag) {
    theLab.ForgetAttribute(kINVALID_SELECTION);
  } else {
    TDataStd_UAttribute::Set(theLab, kINVALID_SELECTION);
  }
  return theFlag;
}

void Model_AttributeSelection::split(
  ResultPtr theContext, TopoDS_Shape theNewShape, TopAbs_ShapeEnum theType)
{
  TopTools_ListOfShape aSubs;
  for(TopoDS_Iterator anExplorer(theNewShape); anExplorer.More(); anExplorer.Next()) {
    if (!anExplorer.Value().IsNull() &&
      anExplorer.Value().ShapeType() == theType) {
        aSubs.Append(anExplorer.Value());
    } else { // invalid case; bad result shape, so, impossible to split easily
      aSubs.Clear();
      break;
    }
  }
  if (aSubs.Extent() > 1) { // ok to split
    TopTools_ListIteratorOfListOfShape aSub(aSubs);
    GeomShapePtr aSubSh(new GeomAPI_Shape);
    aSubSh->setImpl(new TopoDS_Shape(aSub.Value()));
    setValue(theContext, aSubSh);
    for(aSub.Next(); aSub.More(); aSub.Next()) {
      GeomShapePtr aSubSh(new GeomAPI_Shape);
      aSubSh->setImpl(new TopoDS_Shape(aSub.Value()));
      myParent->append(theContext, aSubSh);
    }
  }
}

bool Model_AttributeSelection::update()
{
  TDF_Label aSelLab = selectionLabel();
  ResultPtr aContext = context();
  if (!aContext.get())
    return setInvalidIfFalse(aSelLab, false);
  if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // it is just reference to shape, not sub-shape
    return setInvalidIfFalse(aSelLab, aContext->shape() && !aContext->shape()->isNull());
  }

  if (aSelLab.IsAttribute(kPART_REF_ID)) { // it is reference to the part object
    std::shared_ptr<GeomAPI_Shape> aNoSelection;
    bool aResult = selectPart(aContext, aNoSelection, true);
    aResult = setInvalidIfFalse(aSelLab, aResult);
    if (aResult) {
      owner()->data()->sendAttributeUpdated(this);
    }
    return aResult;
  }

  if (aContext->groupName() == ModelAPI_ResultBody::group()) {
    // body: just a named shape, use selection mechanism from OCCT
    TNaming_Selector aSelector(aSelLab);
    TopoDS_Shape anOldShape;
    if (!aSelector.NamedShape().IsNull()) {
      anOldShape = aSelector.NamedShape()->Get();
    }
    bool aResult = aSelector.Solve(scope()) == Standard_True;
    // must be before sending of updated attribute (1556)
    aResult = setInvalidIfFalse(aSelLab, aResult);
    TopoDS_Shape aNewShape;
    if (!aSelector.NamedShape().IsNull()) {
      aNewShape = aSelector.NamedShape()->Get();
    }
    if (anOldShape.IsNull() || aNewShape.IsNull() ||
        !anOldShape.IsEqual(aSelector.NamedShape()->Get())) {
      // shape type shoud not not changed: if shape becomes compound of such shapes, then split
      if (myParent && !anOldShape.IsNull() && !aNewShape.IsNull() &&
          anOldShape.ShapeType() != aNewShape.ShapeType() &&
          (aNewShape.ShapeType() == TopAbs_COMPOUND || aNewShape.ShapeType() == TopAbs_COMPSOLID))
      {
        split(aContext, aNewShape, anOldShape.ShapeType());
      }
      owner()->data()->sendAttributeUpdated(this);  // send updated if shape is changed
    }
    return aResult;
  }

  if (aContext->groupName() == ModelAPI_ResultConstruction::group()) {
    Handle(TDataStd_Integer) anIndex;
    if (aSelLab.FindAttribute(TDataStd_Integer::GetID(), anIndex)) {
      std::shared_ptr<Model_ResultConstruction> aConstructionContext =
        std::dynamic_pointer_cast<Model_ResultConstruction>(aContext);
      bool aModified = true;
      bool aValid = aConstructionContext->update(anIndex->Get(), owner()->document(), aModified);
      setInvalidIfFalse(aSelLab, aValid);
      if (aModified)
        owner()->data()->sendAttributeUpdated(this);
      return aValid;
    }
  }
  return setInvalidIfFalse(aSelLab, false); // unknown case
}

void Model_AttributeSelection::selectBody(
  const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  // perform the selection
  TNaming_Selector aSel(selectionLabel());
  TopoDS_Shape aContext;

  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theContext);//myRef.value()
  if (aBody) {
    aContext = aBody->shape()->impl<TopoDS_Shape>();
  } else {
    ResultPtr aResult =
      std::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
    if (aResult) {
      aContext = aResult->shape()->impl<TopoDS_Shape>();
    } else {
      Events_InfoMessage("Model_AttributeSelection", "A result with shape is expected").send();
      return;
    }
  }

  // with "recover" feature the selected context may be not up to date (issue 1710)
  Handle(TNaming_NamedShape) aResult;
  TDF_Label aSelLab = selectionLabel();
  TopoDS_Shape aNewContext = aContext;
  bool isUpdated = true;
  while(!aNewContext.IsNull() && isUpdated) {
    // searching for the very last shape that was produced from this one
    isUpdated = false;
    if (!TNaming_Tool::HasLabel(aSelLab, aNewContext))
      // to avoid crash of TNaming_SameShapeIterator if pure shape does not exists
      break;
    for(TNaming_SameShapeIterator anIter(aNewContext, aSelLab); anIter.More(); anIter.Next()) {
      TDF_Label aNSLab = anIter.Label();
      if (!scope().Contains(aNSLab))
        continue;
      Handle(TNaming_NamedShape) aNS;
      if (aNSLab.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
        for(TNaming_Iterator aShapesIter(aNS); aShapesIter.More(); aShapesIter.Next()) {
          if (aShapesIter.Evolution() == TNaming_SELECTED)
            continue; // don't use the selection evolution
          if (!aShapesIter.OldShape().IsNull() && aShapesIter.OldShape().IsSame(aNewContext)) {
             // found the original shape
            aNewContext = aShapesIter.NewShape(); // go to the newer shape
            isUpdated = true;
            break;
          }
        }
      }
    }
  }
  if (aNewContext.IsNull()) { // a context is already deleted
    setInvalidIfFalse(aSelLab, false);
    Events_InfoMessage("Model_AttributeSelection", "Failed to select shape already deleted").send();
    return;
  }

  TopoDS_Shape aNewSub = theSubShape ? theSubShape->impl<TopoDS_Shape>() : aContext;
  if (!aNewSub.IsEqual(aContext)) { // searching for subshape in the new context
    bool isFound = false;
    TopExp_Explorer anExp(aNewContext, aNewSub.ShapeType());
    for(; anExp.More(); anExp.Next()) {
      if (anExp.Current().IsEqual(aNewSub)) {
        isFound = true;
        break;
      }
    }
    if (!isFound) { // sub-shape is not found in the up-to-date instance of the context shape
      // if context is sub-result of compound/compsolid, selection of sub-shape better propagate to
      // the main result (which is may be modified), case is in 1799
      ResultCompSolidPtr aMain = ModelAPI_Tools::compSolidOwner(theContext);
      if (aMain.get()) {
        selectBody(aMain, theSubShape);
        return;
      }
      setInvalidIfFalse(aSelLab, false);
      Events_InfoMessage("Model_AttributeSelection",
        "Failed to select sub-shape already modified").send();
      return;
    }
  }

  /// fix for issue 411: result modified shapes must not participate in this selection mechanism
  if (!aContext.IsNull()) {
    FeaturePtr aFeatureOwner = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
    bool aEraseResults = false;
    if (aFeatureOwner.get()) {
      aEraseResults = !aFeatureOwner->results().empty();
      if (aEraseResults) // erase results without flash deleted and redisplay: do it after Select
        aFeatureOwner->removeResults(0, false);
    }
    aSel.Select(aNewSub, aNewContext);

    if (aEraseResults) { // flash after Select : in Groups it makes selection with shift working
      static Events_Loop* aLoop = Events_Loop::loop();
      static const Events_ID kDeletedEvent = aLoop->eventByName(EVENT_OBJECT_DELETED);
      aLoop->flush(kDeletedEvent);
    }
  }
}

bool Model_AttributeSelection::selectPart(
  const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape,
  const bool theUpdate)
{
  ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(theContext);
  if (!aPart.get() || !aPart->isActivated())
    return true; // postponed naming
  if (theUpdate) {
    Handle(TDataStd_Integer) anIndex;
    if (selectionLabel().FindAttribute(TDataStd_Integer::GetID(), anIndex)) {
      // by internal selection
      if (anIndex->Get() > 0) {
        // update the selection by index
        return aPart->updateInPart(anIndex->Get());
      } else {
        return true; // nothing to do, referencing just by name
      }
    }
    return true; // nothing to do, referencing just by name
  }
  // store the shape (in case part is not loaded it should be useful
  TopoDS_Shape aShape;
  std::string aName = theContext->data()->name();
  if (!theSubShape.get() || theSubShape->isNull()) {// the whole part shape is selected
    aShape = theContext->shape()->impl<TopoDS_Shape>();
  } else {
    aShape = theSubShape->impl<TopoDS_Shape>();
    int anIndex;
    aName += "/" + aPart->nameInPart(theSubShape, anIndex);
    TDataStd_Integer::Set(selectionLabel(), anIndex);
  }
  TNaming_Builder aBuilder(selectionLabel());
  aBuilder.Select(aShape, aShape);
  // identify by name in the part
  TDataStd_Name::Set(selectionLabel(), aName.c_str());
  return !aName.empty();
}

TDF_Label Model_AttributeSelection::selectionLabel()
{
  return myRef.myRef->Label().FindChild(1);
}

std::string Model_AttributeSelection::namingName(const std::string& theDefaultName)
{
  std::string aName("");
  if(!this->isInitialized())
    return !theDefaultName.empty() ? theDefaultName : aName;

  std::shared_ptr<GeomAPI_Shape> aSubSh = value();
  ResultPtr aCont = context();

  if (!aCont.get()) // in case of selection of removed result
    return "";

  Model_SelectionNaming aSelNaming(selectionLabel());
  return aSelNaming.namingName(
    aCont, aSubSh, theDefaultName, owner()->document() != aCont->document());
}

// type ::= COMP | COMS | SOLD | SHEL | FACE | WIRE | EDGE | VERT
void Model_AttributeSelection::selectSubShape(
  const std::string& theType, const std::string& theSubShapeName)
{
  if(theSubShapeName.empty() || theType.empty()) return;

  // check this is Part-name: 2 delimiters in the name
  std::size_t aPartEnd = theSubShapeName.find('/');
  if (aPartEnd != std::string::npos && aPartEnd != theSubShapeName.rfind('/')) {
    std::string aPartName = theSubShapeName.substr(0, aPartEnd);
    ObjectPtr aFound = owner()->document()->objectByName(ModelAPI_ResultPart::group(), aPartName);
    if (aFound.get()) { // found such part, so asking it for the name
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aFound);
      std::string aNameInPart = theSubShapeName.substr(aPartEnd + 1);
      int anIndex;
      std::shared_ptr<GeomAPI_Shape> aSelected = aPart->shapeInPart(aNameInPart, theType, anIndex);
      if (aSelected.get()) {
        setValue(aPart, aSelected);
        TDataStd_Integer::Set(selectionLabel(), anIndex);
        return;
      }
    }
  }

  Model_SelectionNaming aSelNaming(selectionLabel());
  std::shared_ptr<Model_Document> aDoc =
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected;
  ResultPtr aCont;
  if (aSelNaming.selectSubShape(theType, theSubShapeName, aDoc, aShapeToBeSelected, aCont)) {
    // try to find the last context to find the up to date shape
    if (aCont->shape().get() && !aCont->shape()->isNull() &&
      aCont->groupName() == ModelAPI_ResultBody::group() && aDoc == owner()->document()) {
      const TopoDS_Shape aConShape = aCont->shape()->impl<TopoDS_Shape>();
      if (!aConShape.IsNull()) {
        Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aConShape, selectionLabel());
        if (!aNS.IsNull()) {
          aNS = TNaming_Tool::CurrentNamedShape(aNS);
          if (!aNS.IsNull()) {
            TDF_Label aLab = aNS->Label();
            while(aLab.Depth() != 7 && aLab.Depth() > 5)
              aLab = aLab.Father();
            ObjectPtr anObj = aDoc->objects()->object(aLab);
            if (anObj.get()) {
              ResultPtr aRes = std::dynamic_pointer_cast<ModelAPI_Result>(anObj);
              if (aRes)
                aCont = aRes;
            }
          }
        }
      }
    }
    setValue(aCont, aShapeToBeSelected);
  }

  setInvalidIfFalse(selectionLabel(), false);
  reset();
}

int Model_AttributeSelection::Id()
{
  int anID = 0;
  std::shared_ptr<GeomAPI_Shape> aSelection = value();
  std::shared_ptr<GeomAPI_Shape> aContext = context()->shape();
  // support for compsolids:
  if (context().get() && ModelAPI_Tools::compSolidOwner(context()).get())
    aContext = ModelAPI_Tools::compSolidOwner(context())->shape();


  TopoDS_Shape aMainShape = aContext->impl<TopoDS_Shape>();
  const TopoDS_Shape& aSubShape = aSelection->impl<TopoDS_Shape>();
  // searching for the latest main shape
  if (aSelection && !aSelection->isNull() &&
    aContext   && !aContext->isNull())
  {
    std::shared_ptr<Model_Document> aDoc =
      std::dynamic_pointer_cast<Model_Document>(context()->document());
    if (aDoc.get()) {
      Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aMainShape, aDoc->generalLabel());
      if (!aNS.IsNull()) {
        aMainShape = TNaming_Tool::CurrentShape(aNS);
      }
    }

    TopTools_IndexedMapOfShape aSubShapesMap;
    TopExp::MapShapes(aMainShape, aSubShapesMap);
    anID = aSubShapesMap.FindIndex(aSubShape);
  }
  return anID;
}

void Model_AttributeSelection::setId(int theID)
{
  const ResultPtr& aContext = context();
  std::shared_ptr<GeomAPI_Shape> aSelection;

  std::shared_ptr<GeomAPI_Shape> aContextShape = aContext->shape();
  // support for compsolids:
  if (aContext.get() && ModelAPI_Tools::compSolidOwner(aContext).get())
    aContextShape = ModelAPI_Tools::compSolidOwner(aContext)->shape();

  TopoDS_Shape aMainShape = aContextShape->impl<TopoDS_Shape>();
  // searching for the latest main shape
  if (theID > 0 &&
      aContextShape && !aContextShape->isNull())
  {
    std::shared_ptr<Model_Document> aDoc =
      std::dynamic_pointer_cast<Model_Document>(aContext->document());
    if (aDoc.get()) {
      Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aMainShape, aDoc->generalLabel());
      if (!aNS.IsNull()) {
        aMainShape = TNaming_Tool::CurrentShape(aNS);
      }
    }

    TopTools_IndexedMapOfShape aSubShapesMap;
    TopExp::MapShapes(aMainShape, aSubShapesMap);
    const TopoDS_Shape& aSelShape = aSubShapesMap.FindKey(theID);

    std::shared_ptr<GeomAPI_Shape> aResult(new GeomAPI_Shape);
    aResult->setImpl(new TopoDS_Shape(aSelShape));

    aSelection = aResult;
  }

  setValue(aContext, aSelection);
}

std::string Model_AttributeSelection::contextName(const ResultPtr& theContext) const
{
  std::string aResult;
  if (owner()->document() != theContext->document()) {
    if (theContext->document() == ModelAPI_Session::get()->moduleDocument()) {
      aResult = theContext->document()->kind() + "/";
    } else {
      ResultPtr aDocRes = ModelAPI_Tools::findPartResult(
        ModelAPI_Session::get()->moduleDocument(), theContext->document());
      if (aDocRes.get()) {
        aResult = aDocRes->data()->name() + "/";
      }
    }
  }
  aResult += theContext->data()->name();
  return aResult;
}

void Model_AttributeSelection::updateInHistory()
{
  ResultPtr aContext = std::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
  // only bodies may be modified later in the history, don't do anything otherwise
  if (!aContext.get() || aContext->groupName() != ModelAPI_ResultBody::group())
    return;
  std::shared_ptr<Model_Data> aContData = std::dynamic_pointer_cast<Model_Data>(aContext->data());
  if (!aContData.get() || !aContData->isValid())
    return;
  TDF_Label aContLab = aContData->label(); // named shape where the selected context is located
  Handle(TNaming_NamedShape) aContNS;
  if (!aContLab.FindAttribute(TNaming_NamedShape::GetID(), aContNS))
    return;
  std::shared_ptr<Model_Document> aDoc =
    std::dynamic_pointer_cast<Model_Document>(aContext->document());
  FeaturePtr aThisFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
  FeaturePtr aCurrentModifierFeat = aDoc->feature(aContext);
  // iterate the context shape modifications in order to find a feature that is upper in history
  // that this one and is really modifies the referenced result to refer to it
  ResultPtr aModifierResFound;
  TNaming_Iterator aPairIter(aContNS);
  if (!aPairIter.More())
    return;
  TopoDS_Shape aNewShape = aPairIter.NewShape();
  bool anIterate = true;
  // trying to update also the sub-shape selected
  GeomShapePtr aSubShape = value();
  if (aSubShape.get() && aSubShape->isEqual(aContext->shape()))
    aSubShape.reset();

  while(anIterate) {
    anIterate = false;
    TNaming_SameShapeIterator aModifIter(aNewShape, aContLab);
    for(; aModifIter.More(); aModifIter.Next()) {
      ResultPtr aModifierObj = std::dynamic_pointer_cast<ModelAPI_Result>
        (aDoc->objects()->object(aModifIter.Label().Father()));
      if (!aModifierObj.get())
        break;
      FeaturePtr aModifierFeat = aDoc->feature(aModifierObj);
      if (!aModifierFeat.get())
        break;
      if (aModifierFeat == aThisFeature || aDoc->objects()->isLater(aModifierFeat, aThisFeature))
        continue; // the modifier feature is later than this, so, should not be used
      if (aCurrentModifierFeat == aModifierFeat ||
        aDoc->objects()->isLater(aCurrentModifierFeat, aModifierFeat))
        continue; // the current modifier is later than the found, so, useless
      Handle(TNaming_NamedShape) aNewNS;
      aModifIter.Label().FindAttribute(TNaming_NamedShape::GetID(), aNewNS);
      if (aNewNS->Evolution() == TNaming_MODIFY || aNewNS->Evolution() == TNaming_GENERATED) {
        aModifierResFound = aModifierObj;
        aCurrentModifierFeat = aModifierFeat;
        TNaming_Iterator aPairIter(aNewNS);
        aNewShape = aPairIter.NewShape();
        anIterate = true;
        break;
      } else if (aNewNS->Evolution() == TNaming_DELETE) { // a shape was deleted => result is null
        ResultPtr anEmptyContext;
        std::shared_ptr<GeomAPI_Shape> anEmptyShape;
        setValue(anEmptyContext, anEmptyShape); // nullify the selection
        return;
      } else { // not-precessed modification => don't support it
        continue;
      }
    }
  }
  if (aModifierResFound.get()) {
    // update scope to reset to a new one
    myScope.Clear();
    myRef.setValue(aModifierResFound);
    // if context shape type is changed to more complicated and this context is selected, split
    if (myParent &&!aSubShape.get() && aModifierResFound->shape().get() && aContext->shape().get())
    {
      TopoDS_Shape anOldShape = aContext->shape()->impl<TopoDS_Shape>();
      TopoDS_Shape aNewShape = aModifierResFound->shape()->impl<TopoDS_Shape>();
      if (!anOldShape.IsNull() && !aNewShape.IsNull() &&
        anOldShape.ShapeType() != aNewShape.ShapeType() &&
        (aNewShape.ShapeType() == TopAbs_COMPOUND || aNewShape.ShapeType() == TopAbs_COMPSOLID)) {
        // prepare for split in "update"
        TDF_Label aSelLab = selectionLabel();
        split(aContext, aNewShape, anOldShape.ShapeType());
      }
    }
    update(); // it must recompute a new sub-shape automatically
  }
}

void Model_AttributeSelection::setParent(Model_AttributeSelectionList* theParent)
{
  myParent = theParent;
}
