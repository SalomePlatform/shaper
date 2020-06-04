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

#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include "Model_Document.h"
#include <Model_Objects.h>
#include <Model_AttributeSelectionList.h>
#include <Model_ResultConstruction.h>
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>
#include <ModelGeomAlgo_Shape.h>
#include <Events_InfoMessage.h>
#include <GeomAPI_Edge.h>
#include <GeomAPI_Pnt.h>
#include <GeomAPI_ShapeIterator.h>
#include <GeomAPI_Vertex.h>
#include <GeomAPI_ShapeExplorer.h>
#include <GeomAlgoAPI_CompoundBuilder.h>
#include <GeomAlgoAPI_NExplode.h>
#include <Selector_Selector.h>

#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_SameShapeIterator.hxx>
#include <TNaming_NewShapeIterator.hxx>
#include <TNaming_Iterator.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_Name.hxx>
#include <TopTools_ListOfShape.hxx>
#include <TopTools_DataMapOfShapeShape.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <TopoDS_Edge.hxx>
#include <TopExp.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TopoDS_Iterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <Geom_Circle.hxx>
#include <Geom_Ellipse.hxx>
#include <Geom_TrimmedCurve.hxx>
#include <BRep_Builder.hxx>

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

// identifier of the selection of the center of circle on edge
Standard_GUID kCIRCLE_CENTER("d0d0e0f1-217a-4b95-8fbb-0c4132f23718");
// identifier of the selection of the first focus point of ellipse on edge
Standard_GUID kELLIPSE_CENTER1("f70df04c-3168-4dc9-87a4-f1f840c1275d");
// identifier of the selection of the second focus point of ellipse on edge
Standard_GUID kELLIPSE_CENTER2("1395ae73-8e02-4cf8-b204-06ff35873a32");

// prefix for the whole feature context identification
const static std::string kWHOLE_FEATURE = "all-in-";

// on this label is stored:
// TNaming_NamedShape - selected shape
// TNaming_Naming - topological selection information (for the body)
// TDataStd_IntPackedMap - indexes of edges in composite element (for construction)
// TDataStd_Integer - type of the selected shape (for construction)
// TDF_Reference - from ReferenceAttribute, the context
bool Model_AttributeSelection::setValue(const ObjectPtr& theContext,
  const std::shared_ptr<GeomAPI_Shape>& theSubShape, const bool theTemporarily)
{
  if (theTemporarily) {
    // just keep the stored without DF update
    myTmpContext = theContext;
    myTmpSubShape = theSubShape;
    owner()->data()->sendAttributeUpdated(this);
    return true;
  } else {
    myTmpContext.reset();
    myTmpSubShape.reset();
    myTmpCenterType = NOT_CENTER;
  }

  CenterType aType;
  const std::shared_ptr<GeomAPI_Shape>& anOldShape = internalValue(aType);
  bool isOldContext = theContext == myRef.value();
  bool isOldShape = isOldContext &&
    (theSubShape == anOldShape || (theSubShape && anOldShape && theSubShape->isEqual(anOldShape)));
  if (isOldShape) return false; // shape is the same, so context is also unchanged
  bool aToUnblock = false;
  // update the referenced object if needed
  if (!isOldContext) {
    aToUnblock = !owner()->data()->blockSendAttributeUpdated(true);
    myRef.setValue(theContext);
  }

  // do not use naming if selected shape is result shape itself, but not sub-shape
  TDF_Label aSelLab = selectionLabel();
  aSelLab.ForgetAttribute(kSIMPLE_REF_ID);
  aSelLab.ForgetAttribute(kINVALID_SELECTION);
  aSelLab.ForgetAttribute(kCIRCLE_CENTER);
  aSelLab.ForgetAttribute(kELLIPSE_CENTER1);
  aSelLab.ForgetAttribute(kELLIPSE_CENTER2);

  bool isDegeneratedEdge = false;
  // do not use the degenerated edge as a shape, a null context and shape is used in the case
  if (theSubShape.get() && !theSubShape->isNull() && theSubShape->isEdge()) {
    const TopoDS_Shape& aSubShape = theSubShape->impl<TopoDS_Shape>();
    if (aSubShape.ShapeType() == TopAbs_EDGE)
      isDegeneratedEdge = BRep_Tool::Degenerated(TopoDS::Edge(aSubShape)) == Standard_True;
  }
  if (!theContext.get() || isDegeneratedEdge) {
    aSelLab.ForgetAllAttributes(true);
    myRef.myRef = TDF_Reference::Set(aSelLab.Father(), aSelLab.Father());
    if (aToUnblock)
      owner()->data()->blockSendAttributeUpdated(false);
    return false;
  }
  bool isSelectBody = theContext->groupName() == ModelAPI_ResultBody::group();
  if (!isSelectBody) {
    ResultConstructionPtr aContextConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theContext);
    isSelectBody = aContextConstruction.get() && !aContextConstruction->isInfinite();
  }
  if (isSelectBody) {
    ResultPtr aContextResult = std::dynamic_pointer_cast<ModelAPI_Result>(theContext);
    GeomShapePtr aContextShape = aContextResult->shape();
    // do not select the whole shape for body:it is already must be in the data framework
    // equal and null selected objects mean the same: object is equal to context,
    if (aContextShape.get() && (aContextShape->isEqual(theSubShape) || !theSubShape.get())) {
      aSelLab.ForgetAllAttributes(true);
      TDataStd_UAttribute::Set(aSelLab, kSIMPLE_REF_ID);
    } else {
      selectBody(aContextResult, theSubShape);
    }
  } else if (theContext->groupName() == ModelAPI_ResultConstruction::group()) {
    aSelLab.ForgetAllAttributes(true); // to remove old selection data
    std::shared_ptr<Model_ResultConstruction> aConstruction =
      std::dynamic_pointer_cast<Model_ResultConstruction>(theContext);
    std::shared_ptr<GeomAPI_Shape> aSubShape;
    if (theSubShape.get() && !aConstruction->shape()->isEqual(theSubShape))
      aSubShape = theSubShape; // the whole context
  } else if (theContext->groupName() == ModelAPI_ResultPart::group()) {
    aSelLab.ForgetAllAttributes(true);
    TDataStd_UAttribute::Set(aSelLab, kPART_REF_ID);
    selectPart(std::dynamic_pointer_cast<ModelAPI_Result>(theContext), theSubShape);
  } else if (theContext->groupName() == ModelAPI_ResultGroup::group()) {
    aSelLab.ForgetAllAttributes(true);
    TDataStd_UAttribute::Set(aSelLab, kSIMPLE_REF_ID);
  } else { // check the feature context: only construction features of PartSet could be selected
    FeaturePtr aFeatureContext = std::dynamic_pointer_cast<ModelAPI_Feature>(theContext);
    if (aFeatureContext.get() && owner()->document() != aFeatureContext->document()) {
      if (aFeatureContext->results().empty() ||
          aFeatureContext->firstResult()->groupName() != ModelAPI_ResultConstruction::group()) {
        aSelLab.ForgetAllAttributes(true);
        myRef.setValue(ObjectPtr());
        if (aToUnblock)
          owner()->data()->blockSendAttributeUpdated(false);
        return false;
      }
    }
  }

  owner()->data()->sendAttributeUpdated(this);

  if (aToUnblock)
    owner()->data()->blockSendAttributeUpdated(false);

  return true;
}

void Model_AttributeSelection::setValueCenter(
    const ObjectPtr& theContext, const std::shared_ptr<GeomAPI_Edge>& theEdge,
    const CenterType theCenterType, const bool theTemporarily)
{
  bool anUpdated = setValue(theContext, theEdge, theTemporarily);
  if (theTemporarily) {
    myTmpCenterType = theCenterType;
  } else { // store in the data structure
    TDF_Label aSelLab = selectionLabel();
    switch(theCenterType) {
    case CIRCLE_CENTER:
      if (!anUpdated)
        anUpdated = !aSelLab.IsAttribute(kCIRCLE_CENTER);
      TDataStd_UAttribute::Set(aSelLab, kCIRCLE_CENTER);
      break;
    case ELLIPSE_FIRST_FOCUS:
      if (!anUpdated)
        anUpdated = !aSelLab.IsAttribute(kELLIPSE_CENTER1);
      TDataStd_UAttribute::Set(aSelLab, kELLIPSE_CENTER1);
      break;
    case ELLIPSE_SECOND_FOCUS:
      if (!anUpdated)
        anUpdated = !aSelLab.IsAttribute(kELLIPSE_CENTER2);
      TDataStd_UAttribute::Set(aSelLab, kELLIPSE_CENTER2);
      break;
    default: // [to avoid compilation warning]
      break;
    }
    if (anUpdated)
      owner()->data()->sendAttributeUpdated(this);
  }
}

void Model_AttributeSelection::selectValue(
    const std::shared_ptr<ModelAPI_AttributeSelection>& theSource)
{
  CenterType aType;
  std::shared_ptr<GeomAPI_Shape> aValue =
    std::dynamic_pointer_cast<Model_AttributeSelection>(theSource)->internalValue(aType);
  if (!aValue.get() || aType == NOT_CENTER) {
    setValue(theSource->context(), aValue);
  } else {
    std::shared_ptr<GeomAPI_Edge> anEdge(new GeomAPI_Edge);
    anEdge->setImpl(new TopoDS_Shape(aValue->impl<TopoDS_Shape>()));
    setValueCenter(theSource->context(), anEdge, aType);
  }
}

void Model_AttributeSelection::removeTemporaryValues()
{
  if (myTmpContext.get() || myTmpSubShape.get()) {
    myTmpContext.reset();
    myTmpSubShape.reset();
  }
}

// returns the center of the edge: circular or elliptic
GeomShapePtr centerByEdge(GeomShapePtr theEdge, ModelAPI_AttributeSelection::CenterType theType)
{
  if (theType != ModelAPI_AttributeSelection::NOT_CENTER && theEdge.get() != NULL) {
    TopoDS_Shape aShape = theEdge->impl<TopoDS_Shape>();
    if (!aShape.IsNull() && aShape.ShapeType() == TopAbs_EDGE) {
      TopoDS_Edge anEdge = TopoDS::Edge(aShape);
      double aFirst, aLast;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
      if (!aCurve.IsNull()) {
        TopoDS_Vertex aVertex;
        BRep_Builder aBuilder;
        if (theType == ModelAPI_AttributeSelection::CIRCLE_CENTER) {
          while(!aCurve.IsNull() && aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
            aCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();
          Handle(Geom_Circle) aCirc = Handle(Geom_Circle)::DownCast(aCurve);
          if (!aCirc.IsNull()) {
            aBuilder.MakeVertex(aVertex, aCirc->Location(), Precision::Confusion());
          }
        } else { // ellipse
          while(!aCurve.IsNull() && aCurve->DynamicType() == STANDARD_TYPE(Geom_TrimmedCurve))
            aCurve = Handle(Geom_TrimmedCurve)::DownCast(aCurve)->BasisCurve();
          Handle(Geom_Ellipse) anEll = Handle(Geom_Ellipse)::DownCast(aCurve);
          if (!anEll.IsNull()) {
            aBuilder.MakeVertex(aVertex,
              theType == ModelAPI_AttributeSelection::ELLIPSE_FIRST_FOCUS ?
              anEll->Focus1() : anEll->Focus2(), Precision::Confusion());
          }
        }
        if (!aVertex.IsNull()) {
          std::shared_ptr<GeomAPI_Vertex> aResult(new GeomAPI_Vertex);
          aResult->setImpl(new TopoDS_Vertex(aVertex));
          return aResult;
        }
      }
    }
  }
  return theEdge; // no vertex, so, return the initial edge
}

std::shared_ptr<GeomAPI_Shape> Model_AttributeSelection::value()
{
  if (!myRef.isInitialized() && !myTmpContext.get() && !myTmpSubShape.get())
    return std::shared_ptr<GeomAPI_Shape>();
  CenterType aType = NOT_CENTER;
  std::shared_ptr<GeomAPI_Shape> aResult = internalValue(aType);
  return centerByEdge(aResult, aType);
}

std::shared_ptr<GeomAPI_Shape> Model_AttributeSelection::internalValue(CenterType& theType)
{
  theType = NOT_CENTER;
  GeomShapePtr aResult;
  if (myTmpContext.get() || myTmpSubShape.get()) {
    theType = myTmpCenterType;
    ResultConstructionPtr aResulConstruction =
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(myTmpContext);
    if(aResulConstruction.get()) {
      // it is just reference to construction.
      return myTmpSubShape;
    }
    FeaturePtr aFeature =
      std::dynamic_pointer_cast<ModelAPI_Feature>(myTmpContext);
    if (aFeature.get()) {
      // it is just reference to construction.
      return myTmpSubShape;
    }
    return myTmpSubShape.get() ? myTmpSubShape :
      std::dynamic_pointer_cast<ModelAPI_Result>(myTmpContext)->shape();
  }

  TDF_Label aSelLab = selectionLabel();
  if (aSelLab.IsAttribute(kINVALID_SELECTION))
    return aResult;

  if (aSelLab.IsAttribute(kCIRCLE_CENTER))
    theType = CIRCLE_CENTER;
  else if (aSelLab.IsAttribute(kELLIPSE_CENTER1))
    theType = ELLIPSE_FIRST_FOCUS;
  else if (aSelLab.IsAttribute(kELLIPSE_CENTER2))
    theType = ELLIPSE_SECOND_FOCUS;


  if (myRef.isInitialized()) {
    if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // it is just reference to shape, not sub-shape
      ResultPtr aContext = context();
      if (!aContext.get() || aContext->groupName() == ModelAPI_ResultConstruction::group())
        return aResult; // empty result, for whole construction selection also
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
              int anInd;
              std::string aType; // to reuse already existing selection the type is not needed
              return aPart->shapeInPart(aNameInPart, aType, anInd);
            }
          }
        }
      }
    }

    std::shared_ptr<Model_ResultConstruction> aConstr =
      std::dynamic_pointer_cast<Model_ResultConstruction>(context());
    if (aConstr) {
      if (aConstr->isInfinite())
        return aResult; // empty result
    }
    if (!aConstr.get()) { // for construction context, return empty result as usual even
      // the whole feature is selected
      FeaturePtr aFeature = contextFeature();
      if (aFeature.get()) {
        std::list<GeomShapePtr> allShapes;
        std::list<ResultPtr>::const_iterator aRes = aFeature->results().cbegin();
        for (; aRes != aFeature->results().cend(); aRes++) {
          if (aRes->get() && !(*aRes)->isDisabled()) {
            GeomShapePtr aShape = (*aRes)->shape();
            if (aShape.get() && !aShape->isNull()) {
              allShapes.push_back(aShape);
            }
          }
        }
        return GeomAlgoAPI_CompoundBuilder::compound(allShapes);
      }
    } else {
      if (contextFeature().get()) {
        return aResult; // for the whole sketch feature selected return null => all faces
      }
    }

    Handle(TNaming_NamedShape) aSelection;
    if (aSelLab.FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
      TopoDS_Shape aSelShape = aSelection->Get();
      aResult = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape);
      aResult->setImpl(new TopoDS_Shape(aSelShape));
    } else if (aConstr) { // simple construction element: just shape of this construction element
      aResult = aConstr->shape();
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
  if (myRef.isInitialized()) {
    TDF_Label aSelLab = selectionLabel();
    // it is just reference to shape, not sub-shape
    if (aSelLab.IsAttribute(kSIMPLE_REF_ID) || aSelLab.IsAttribute(kPART_REF_ID)) {
      ResultPtr aContext = context();
      return aContext.get() != NULL;
    }
    Handle(TNaming_NamedShape) aSelection;
    if (selectionLabel().FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
      return !aSelection->Get().IsNull();
    } else { // for simple construction element: just shape of this construction element
      if (myRef.value().get())
        return true;
      // check that this is on open of document, so, results are not initialized yet
      TDF_Label aRefLab = myRef.myRef->Get();
      if (aRefLab.IsNull() || !owner().get())
        return false;
      std::shared_ptr<Model_Document> aMyDoc =
        std::dynamic_pointer_cast<Model_Document>(owner()->document());
      if (!aMyDoc.get())
        return false;
      // check at least the feature exists
      return aMyDoc->featureByLab(aRefLab).get() != NULL;
    }
  }
  return false;
}

Model_AttributeSelection::Model_AttributeSelection(TDF_Label& theLabel)
: myRef(theLabel),
  myIsGeometricalSelection(false)
{
  myIsInitialized = myRef.isInitialized();
  myParent = NULL;
}

void Model_AttributeSelection::setID(const std::string theID)
{
  myRef.setID(theID);
  ModelAPI_AttributeSelection::setID(theID);
  FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
  if (myParent) {
    // to be able to select as geometrical selection and then - split
    myIsGeometricalSelection = true; // myParent->isGeometricalSelection();
  } else {
    myIsGeometricalSelection =
      ModelAPI_Session::get()->validators()->isGeometricalSelection(aFeature->getKind(), id());
  }
}

ResultPtr Model_AttributeSelection::context()
{
  if (!myRef.isInitialized() && !myTmpContext.get() && !myTmpSubShape.get())
    return ResultPtr();

  if (myTmpContext.get() || myTmpSubShape.get()) {
    return std::dynamic_pointer_cast<ModelAPI_Result>(myTmpContext);
  }

  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
  // for parts there could be same-data result, so take the last enabled
  if (aResult.get()) {
    if(aResult->groupName() == ModelAPI_ResultPart::group()) {
      int aSize = aResult->document()->size(ModelAPI_ResultPart::group());
      for(int a = aSize - 1; a >= 0; a--) {
        ObjectPtr aPart = aResult->document()->object(ModelAPI_ResultPart::group(), a);
        if(aPart.get() && aPart->data() == aResult->data()) {
          ResultPtr aPartResult = std::dynamic_pointer_cast<ModelAPI_Result>(aPart);
          FeaturePtr anOwnerFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
          // check that this result is not this-feature result (it is forbidden to select itself)
          if(anOwnerFeature.get() && anOwnerFeature->firstResult() != aPartResult) {
            return aPartResult;
          }
        }
      }
    }
  } else { // if feature - construction is selected, it has only one result, return this result
    FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(myRef.value());
    if (aFeature.get() && aFeature->results().size() == 1 &&
        aFeature->firstResult()->groupName() == ModelAPI_ResultConstruction::group())
      return aFeature->firstResult();
  }
  return aResult;
}

FeaturePtr Model_AttributeSelection::contextFeature() {
  if (myTmpContext.get()) {
    return std::dynamic_pointer_cast<ModelAPI_Feature>(myTmpContext);
  }
  return std::dynamic_pointer_cast<ModelAPI_Feature>(myRef.value());
}
ObjectPtr Model_AttributeSelection::contextObject() {
  FeaturePtr aRes = contextFeature();
  if (aRes.get())
    return aRes;
  return context();
}


void Model_AttributeSelection::setObject(const std::shared_ptr<ModelAPI_Object>& theObject)
{
  ModelAPI_AttributeSelection::setObject(theObject);
  myRef.setObject(theObject);
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
      aSubSh.reset(new GeomAPI_Shape);
      aSubSh->setImpl(new TopoDS_Shape(aSub.Value()));
      myParent->append(theContext, aSubSh);
    }
  }
}

bool Model_AttributeSelection::update()
{
  FeaturePtr aContextFeature = contextFeature();
  if (aContextFeature.get()) {
    owner()->data()->sendAttributeUpdated(this);  // send updated if "update" called in any way
    return true;
  }
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
    // body: just a named shape, use topological selection mechanism
    bool aResult = false;
    TopoDS_Shape anOldShape;
    Handle(TNaming_NamedShape) aNS;
    if (aSelLab.FindAttribute(TNaming_NamedShape::GetID(), aNS))
      anOldShape = aNS->Get();

    TopoDS_Shape aContextShape = aContext->shape()->impl<TopoDS_Shape>();
    Selector_Selector aSelector(aSelLab, baseDocumentLab());
    aResult = aSelector.restore(aContextShape);
    bool aWasInvalid = aSelLab.IsAttribute(kINVALID_SELECTION);
    setInvalidIfFalse(aSelLab, aResult);

    TopoDS_Shape aNewShape;
    if (aSelLab.FindAttribute(TNaming_NamedShape::GetID(), aNS))
      aNewShape = aNS->Get();

    if (anOldShape.IsNull() || aNewShape.IsNull() || !anOldShape.IsEqual(aNewShape) || aWasInvalid)
    {
      // shape type should not be changed: if shape becomes compound of such shapes, then split
      if (myParent && !anOldShape.IsNull() && !aNewShape.IsNull() &&
          anOldShape.ShapeType() != aNewShape.ShapeType() &&
          aNewShape.ShapeType() == TopAbs_COMPOUND) {
        split(aContext, aNewShape, anOldShape.ShapeType());
      }
      // for issue #3076 check that the new value belongs to the new context
      if (!aNewShape.IsNull() && !aContextShape.IsNull() &&
          (aNewShape.ShapeType() == TopAbs_VERTEX || aNewShape.ShapeType() == TopAbs_EDGE ||
           aNewShape.ShapeType() == TopAbs_FACE)) {
        TopExp_Explorer anExp(aContextShape, aNewShape.ShapeType());
        for(; anExp.More(); anExp.Next()) {
          if (anExp.Current().IsSame(aNewShape))
            break;
        }
        aResult = setInvalidIfFalse(aSelLab, anExp.More());
      }
      owner()->data()->sendAttributeUpdated(this);  // send updated if shape is changed
    }
    return aResult;
  }

  if (aContext->groupName() == ModelAPI_ResultConstruction::group()) {
    bool aResult = true;
    std::shared_ptr<Model_ResultConstruction> aConstructionContext =
      std::dynamic_pointer_cast<Model_ResultConstruction>(aContext);
    if (!aConstructionContext->isInfinite()) {
      TopoDS_Shape aContextShape = aContext->shape()->impl<TopoDS_Shape>();
      Selector_Selector aSelector(aSelLab, baseDocumentLab());
      TopoDS_Shape anOldShape = aSelector.value();
      aResult = aSelector.restore(aContextShape);
      setInvalidIfFalse(aSelLab, aResult);
      if (aResult && !anOldShape.IsEqual(aSelector.value()))
        owner()->data()->sendAttributeUpdated(this);  // send updated if shape is changed
    } else {
      owner()->data()->sendAttributeUpdated(this);  // send updated if "update" called in any way
    }
    return aResult;
  }
  return setInvalidIfFalse(aSelLab, false); // unknown case
}

void Model_AttributeSelection::selectBody(
  const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  // perform the selection
  TopoDS_Shape aContext;

  ResultPtr aBody = std::dynamic_pointer_cast<ModelAPI_Result>(theContext);
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

  if (!aContext.IsNull()) {
    TDF_Label aSelLab = selectionLabel();
    TopoDS_Shape aNewSub = theSubShape->impl<TopoDS_Shape>();

    bool aSelectorOk = true;
    Selector_Selector aSelector(aSelLab, baseDocumentLab());
    try {
      aSelectorOk = aSelector.select(aContext, aNewSub, myIsGeometricalSelection);
      if (aSelectorOk) {
        aSelectorOk = aSelector.store(aContext);
      }
    } catch(...) {
      aSelectorOk = false;
    }
    if (aSelectorOk) {
      TopoDS_Shape aShape = aSelector.value();
      aSelectorOk = !aShape.IsNull() && aShape.ShapeType() == aNewSub.ShapeType();
    }
    setInvalidIfFalse(aSelLab, aSelectorOk);
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

/// prefixes of the shape names with centers defined
static std::map<ModelAPI_AttributeSelection::CenterType, std::string> kCENTERS_PREFIX;

/// returns the map that contains all possible prefixes of the center-names
static std::map<ModelAPI_AttributeSelection::CenterType, std::string>& centersMap()
{
  if (kCENTERS_PREFIX.empty()) { // fill map by initial values
    kCENTERS_PREFIX[ModelAPI_AttributeSelection::CIRCLE_CENTER] = "__cc";
    kCENTERS_PREFIX[ModelAPI_AttributeSelection::ELLIPSE_FIRST_FOCUS] = "__eff";
    kCENTERS_PREFIX[ModelAPI_AttributeSelection::ELLIPSE_SECOND_FOCUS] = "__esf";
  }
  return kCENTERS_PREFIX;
}

std::string Model_AttributeSelection::namingName(const std::string& theDefaultName)
{
  std::string aName;
  if(!this->isInitialized())
    return !theDefaultName.empty() ? theDefaultName : aName;

  // not argument has not parametric name (filters)
  if (!this->isArgument() || (myParent && !myParent->isArgument())) {
    GeomShapePtr aShape = value();
    if (!aShape.get() && context().get())
      aShape = context()->shape();
    if (aShape.get()) {
      aName = aShape->shapeTypeStr();
      if (myParent) {
        aName += std::string("_") +
          TCollection_AsciiString(selectionLabel().Father().Tag()).ToCString();
      }
    }
    return aName;
  }

  CenterType aCenterType = NOT_CENTER;
  std::shared_ptr<GeomAPI_Shape> aSubSh = internalValue(aCenterType);

  FeaturePtr aContFeature = contextFeature();
  if (aContFeature.get()) {
    std::string aResName;
    // checking part-owner
    if (aContFeature->document() != owner()->document())
        aResName += aContFeature->document()->kind() + "/";
    // selection of a full feature
    if (aContFeature.get()) {
      return aResName + kWHOLE_FEATURE + aContFeature->name();
    }
    // in case of selection of removed result
    return "";
  }

  ResultPtr aCont = context();
  if (!aCont.get()) {
    return ""; // invalid case
  }
  TDF_Label aSelLab = selectionLabel();
  if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // whole context, no value
    return contextName(aCont);
  }

  // if it is in result of another part
  if (aCont->groupName() == ModelAPI_ResultPart::group()) {
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aCont);
    int anIndex;
    std::string aResult = aSubSh.get() ?
      aPart->data()->name() + "/" + aPart->nameInPart(aSubSh, anIndex) : aPart->data()->name();
    if (aCenterType != NOT_CENTER)
      aResult += centersMap()[aCenterType];
    return aResult;
  }

  // whole infinitive construction
  if (aCont->groupName() == ModelAPI_ResultConstruction::group()) {
    ResultConstructionPtr aConstr = std::dynamic_pointer_cast<Model_ResultConstruction>(aCont);
    if (aConstr->isInfinite()) {
      return contextName(aCont);
    }
  }

  Selector_Selector aSelector(aSelLab, baseDocumentLab());
  std::string aResult;
  if (aCont->shape().get() && aSelector.restore(aCont->shape()->impl<TopoDS_Shape>()))
    aResult = aSelector.name(this);
  if (aCenterType != NOT_CENTER) {
    aResult += centersMap()[aCenterType];
  }
  return aResult;
}

// returns the center type and modifies the shape name if this name is center-name
static ModelAPI_AttributeSelection::CenterType centerTypeByName(std::string& theShapeName)
{
  std::map<ModelAPI_AttributeSelection::CenterType, std::string>::iterator aPrefixIter =
    centersMap().begin();
  for(; aPrefixIter != centersMap().end(); aPrefixIter++) {
    std::size_t aFound = theShapeName.find(aPrefixIter->second);
    if (aFound != std::string::npos &&
        aFound == theShapeName.size() - aPrefixIter->second.size()) {
      theShapeName = theShapeName.substr(0, aFound);
      return aPrefixIter->first;
    }
  }
  return ModelAPI_AttributeSelection::NOT_CENTER;
}

// type ::= COMP | COMS | SOLD | SHEL | FACE | WIRE | EDGE | VERT
void Model_AttributeSelection::selectSubShape(
  const std::string& theType, const std::string& theSubShapeName)
{
  if(theSubShapeName.empty() || theType.empty()) return;

  std::string aSubShapeName = theSubShapeName;
  CenterType aCenterType = theType[0] == 'v' || theType[0] == 'V' ? // only for vertex-type
    centerTypeByName(aSubShapeName) : NOT_CENTER;
  std::string aType = aCenterType == NOT_CENTER ? theType : "EDGE"; // search for edge now
  static const GeomShapePtr anEmptyShape;

  // first iteration is selection by name without center prefix, second - in case of problem,
  // try with initial name
  for(int aUseCenter = 1; aUseCenter >= 0; aUseCenter--)  {
    if (aUseCenter == 0 && aCenterType != NOT_CENTER) {
      aSubShapeName = theSubShapeName;
      aCenterType = NOT_CENTER;
      aType = theType;
    } else if (aUseCenter != 1) continue;
    TopAbs_ShapeEnum aShapeType =  TopAbs_ShapeEnum(GeomAPI_Shape::shapeTypeByStr(aType));

    std::shared_ptr<Model_Document> aDoc =
      std::dynamic_pointer_cast<Model_Document>(owner()->document());
    // check this is Part-name: 2 delimiters in the name
    std::size_t aPartEnd = aSubShapeName.find('/');
    if (aPartEnd != std::string::npos) {
      std::string aPartName = aSubShapeName.substr(0, aPartEnd);
      DocumentPtr aRootDoc = ModelAPI_Session::get()->moduleDocument();
      if (aPartName == aRootDoc->kind()) {
        aDoc = std::dynamic_pointer_cast<Model_Document>(aRootDoc);
        aSubShapeName = aSubShapeName.substr(aPartEnd + 1);
      }
      else {
        ObjectPtr aFound =
          owner()->document()->objectByName(ModelAPI_ResultPart::group(), aPartName);
        if (aFound.get()) { // found such part, so asking it for the name
          ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aFound);
          std::string aNameInPart = aSubShapeName.substr(aPartEnd + 1);
          if (aNameInPart.empty()) { // whole part
            setValue(aPart, anEmptyShape);
            return;
          }
          int anIndex;
          std::shared_ptr<GeomAPI_Shape> aSelected =
            aPart->shapeInPart(aNameInPart, aType, anIndex);
          if (aSelected.get()) {
            if (aCenterType != NOT_CENTER) {
              if (!aSelected->isEdge())
                continue;
              std::shared_ptr<GeomAPI_Edge> aSelectedEdge(new GeomAPI_Edge(aSelected));
              setValueCenter(aPart, aSelectedEdge, aCenterType);
            }
            else
              setValue(aPart, aSelected);
            TDataStd_Integer::Set(selectionLabel(), anIndex);
            return;
          }
        } else { // for the ImportResult feature Objects widget this may be a result in other part
       // result may be hidden (like, tranlsatiomn of part) in PartSet, so iterate Part-features
          int aNum = aRootDoc->size(ModelAPI_Feature::group());
          for (int a = 0; a < aNum; a++) {
            FeaturePtr aFeat = std::dynamic_pointer_cast<ModelAPI_Feature>(
              aRootDoc->object(ModelAPI_Feature::group(), a));
            if (aFeat.get() && aFeat->data() && aFeat->data()->isValid() &&
              aFeat->getKind() == "Part" && aFeat->results().size()) {
              ResultPartPtr aPart =
                std::dynamic_pointer_cast<ModelAPI_ResultPart>(aFeat->firstResult());
              if (aPart.get() && aPart->partDoc().get() && aPart->data()->name() == aPartName) {
                aDoc = std::dynamic_pointer_cast<Model_Document>(aPart->partDoc());
                aSubShapeName = aSubShapeName.substr(aPartEnd + 1);
              }
            }
          }
        }
      }
    }

    // check this is a whole feature context
    if (aSubShapeName.size() > kWHOLE_FEATURE.size() &&
      aSubShapeName.substr(0, kWHOLE_FEATURE.size()) == kWHOLE_FEATURE) {
      std::string aFeatureName = aSubShapeName.substr(kWHOLE_FEATURE.size());
      ObjectPtr anObj = aDoc->objectByName(ModelAPI_Feature::group(), aFeatureName);
      if (anObj.get()) {
        setValue(anObj, anEmptyShape);
        return;
      }
    }

    // the whole result selection check
    if (aSubShapeName.find('/') == std::string::npos) {
      ObjectPtr aRes = aDoc->objectByName(ModelAPI_ResultConstruction::group(), aSubShapeName);
      if (!aRes.get()) {
        aRes = aDoc->objectByName(ModelAPI_ResultBody::group(), aSubShapeName);
        if (!aRes.get())
          aRes = aDoc->objectByName(ModelAPI_ResultGroup::group(), aSubShapeName);
      }
      if (aRes.get()) {
        setValue(aRes, anEmptyShape);
        return;
      }
    }

    Selector_Selector aSelector(selectionLabel(), baseDocumentLab());
    myRestoreDocument = aDoc;
    TDF_Label aContextLabel = aSelector.restoreByName(
      aSubShapeName, aShapeType, this, myIsGeometricalSelection);
    myRestoreDocument.reset();
    if (!aContextLabel.IsNull()) {
      ResultPtr aContext = aDoc->resultByLab(aContextLabel); // any label for document access
      if (aContext.get() && aContext->shape().get()) {
        TopoDS_Shape aContextShape = aContext->shape()->impl<TopoDS_Shape>();
        if (aSelector.solve(aContextShape)) {
          TopoDS_Shape aSelectorShape = aSelector.value();
          GeomShapePtr aShapeToBeSelected(new GeomAPI_Shape);
          aShapeToBeSelected->setImpl<TopoDS_Shape>(new TopoDS_Shape(aSelectorShape));
          // make the context result the latest existing
          aContext = newestContext(aContext, aShapeToBeSelected);
          if (myIsGeometricalSelection || aCenterType == NOT_CENTER) {
            // store the currently generated name
            selectionLabel().ForgetAllAttributes(true);
            bool aToUnblock = false;
            aToUnblock = !owner()->data()->blockSendAttributeUpdated(true);
            myRef.setValue(aContext);
            aSelector.store(aContextShape);
            owner()->data()->sendAttributeUpdated(this);
            if (aToUnblock)
              owner()->data()->blockSendAttributeUpdated(false);
            return;
          } else { // re-select center of circle/arc by context and value
            if (!aShapeToBeSelected->isEdge())
              continue;
            std::shared_ptr<GeomAPI_Edge> aSelectedEdge(new GeomAPI_Edge(aShapeToBeSelected));
            setValueCenter(aContext, aSelectedEdge, aCenterType);
          }
          return;
        }
      }
    }
  }
  // invalid
  TDF_Label aSelLab = selectionLabel();
  setInvalidIfFalse(aSelLab, false);
  reset();
}

void Model_AttributeSelection::selectSubShape(const std::string& theType,
                                              const GeomPointPtr& thePoint)
{
  if (theType.empty() || !thePoint)
    return;

  // list of parent features
  FeaturePtr anOwner = ModelAPI_Feature::feature(owner());
  std::set<FeaturePtr> aParents = ModelAPI_Tools::getParents(anOwner);

  int aSelectionIndex = 0;
  GeomAPI_Shape::ShapeType aType = GeomAPI_Shape::shapeTypeByStr(theType);
  if (aType == GeomAPI_Shape::SHAPE) {
    // possibly, the string consists of the type and the index,
    // thus, try to separate them
    size_t aUndersporePos = theType.find_first_of('_');
    if (aUndersporePos != std::string::npos)
      aType = GeomAPI_Shape::shapeTypeByStr(theType.substr(0, aUndersporePos));

    if (aType != GeomAPI_Shape::SHAPE) {
      for (std::string::const_iterator aChar = theType.begin() + aUndersporePos + 1;
           aChar != theType.end(); ++aChar) {
        if (std::isdigit(*aChar))
          aSelectionIndex = aSelectionIndex * 10 + (*aChar - '0');
        else {
          aSelectionIndex = 1;
          break;
        }
      }
      aSelectionIndex -= 1;
    }
  }

  std::list<ModelGeomAlgo_Shape::SubshapeOfResult> anAppropriate;

  // collect features from PartSet and the current part
  SessionPtr aSession = ModelAPI_Session::get();
  std::list<FeaturePtr> aFeatures = aSession->moduleDocument()->allFeatures();
  if (aSession->moduleDocument() != owner()->document()) {
    std::list<FeaturePtr> aPartFeatures = owner()->document()->allFeatures();
    aFeatures.insert(aFeatures.end(), aPartFeatures.begin(), aPartFeatures.end());
  }
  // Process results of all features from the last to the first
  // to find appropriate sub-shape
  for (std::list<FeaturePtr>::const_reverse_iterator anIt = aFeatures.rbegin();
       anIt != aFeatures.rend(); ++anIt) {
    // selection cannot be linked to the parent features
    if (aParents.find(*anIt) != aParents.end())
      continue;
    // check the feature is a part of composite feature (like sketch elements),
    // then do not process it, it will be processed in scope of composite feature
    bool isSubOfComposite = false;
    const std::set<AttributePtr>& aRefs = (*anIt)->data()->refsToMe();
    for (std::set<AttributePtr>::const_iterator aRefIt = aRefs.begin();
         aRefIt != aRefs.end() && !isSubOfComposite; ++aRefIt) {
      FeaturePtr aFeature = ModelAPI_Feature::feature((*aRefIt)->owner());
      CompositeFeaturePtr aCompFeature =
          std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFeature);
      isSubOfComposite = aCompFeature && aCompFeature->isSub(*anIt);
    }
    if (isSubOfComposite)
      continue;

    // process results of the current feature to find appropriate sub-shape
    if (ModelGeomAlgo_Shape::findSubshapeByPoint(*anIt, thePoint, aType, anAppropriate)) {
      std::list<ModelGeomAlgo_Shape::SubshapeOfResult>::iterator anApIt = anAppropriate.begin();
      for (; aSelectionIndex > 0 && anApIt != anAppropriate.end(); --aSelectionIndex)
        ++anApIt; // skip this shape, because one of the previous is selected

      if (anApIt != anAppropriate.end()) {
        if (anApIt->myCenterType == (int)ModelAPI_AttributeSelection::NOT_CENTER)
          setValue(anApIt->myResult, anApIt->mySubshape);
        else
          setValueCenter(anApIt->myResult, anApIt->mySubshape->edge(),
                         (ModelAPI_AttributeSelection::CenterType)anApIt->myCenterType);
        return;
      }
    }
  }

  TDF_Label aSelLab = selectionLabel();
  setInvalidIfFalse(aSelLab, false);
  reset();
}

void Model_AttributeSelection::selectSubShape(const std::string& theType,
  const std::string& theContextName, const int theIndex)
{
  // selection of context by name
  selectSubShape(theType, theContextName);
  ResultPtr aContext = context();
  if (aContext.get()) {
    GeomShapePtr aContShape = aContext->shape();
    if (aContShape.get()) {
      GeomAlgoAPI_NExplode aNExp(aContShape, GeomAPI_Shape::shapeTypeByStr(theType));
      GeomShapePtr aValue = aNExp.shape(theIndex);
      if (aValue.get())
        setValue(aContext, aValue);
    }
  }
}

void Model_AttributeSelection::setId(int theID)
{
  std::shared_ptr<GeomAPI_Shape> aSelection;

  ResultPtr aContextRes = context();
  // support for compsolids:
  while(ModelAPI_Tools::bodyOwner(aContextRes).get()) {
    aContextRes = ModelAPI_Tools::bodyOwner(aContextRes);
  }
  std::shared_ptr<GeomAPI_Shape> aContext = aContextRes->shape();

  TopoDS_Shape aMainShape = aContext->impl<TopoDS_Shape>();
  // searching for the latest main shape
  if (theID > 0 && aContext && !aContext->isNull())
  {
    std::shared_ptr<Model_Document> aDoc =
      std::dynamic_pointer_cast<Model_Document>(aContextRes->document());
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

  setValue(aContextRes, aSelection);
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

void Model_AttributeSelection::computeValues(
  ResultPtr theOldContext, ResultPtr theNewContext, TopoDS_Shape theValShape,
  TopTools_ListOfShape& theShapes)
{
  bool aWasWholeContext = theValShape.IsNull();
  if (aWasWholeContext) {
    theValShape = theOldContext->shape()->impl<TopoDS_Shape>();
  }
  TopAbs_ShapeEnum aValType = theValShape.ShapeType();
  TopoDS_Shape aNewContShape = theNewContext->shape()->impl<TopoDS_Shape>();
  // if a new value is unchanged in the new context, do nothing: value is correct
  TopExp_Explorer aSubExp(aNewContShape, aValType);
  for(; aSubExp.More(); aSubExp.Next()) {
    if (aSubExp.Current().IsSame(theValShape)) {
      theShapes.Append(theValShape);
      return;
    }
  }
  // if new context becomes compsolid, the resulting sub may be in sub-solids
  std::list<ResultPtr> aNewToIterate;
  aNewToIterate.push_back(theNewContext);
  ResultBodyPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultBody>(theNewContext);
  if (aComp.get()) {
    std::list<ResultPtr> allNewContextSubs;
    ModelAPI_Tools::allSubs(aComp, allNewContextSubs);
    std::list<ResultPtr>::iterator aSub = allNewContextSubs.begin();
    for(; aSub != allNewContextSubs.end(); aSub++) {
      ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aSub);
      if (aBody.get() && aBody->numberOfSubs() == 0) // add only lower level subs
        aNewToIterate.push_back(aBody);
    }
  }

  // first iteration: searching for the whole shape appearance (like face of the box)
  // second iteration: searching for sub-shapes that contain the sub (like vertex on faces)
  int aToFindPart = 0;
  TopTools_DataMapOfShapeShape aNewToOld; // map from new containers to old containers (with val)
  TopTools_MapOfShape anOlds; // to know how many olds produced new containers
  for(; aToFindPart != 2 && theShapes.IsEmpty(); aToFindPart++) {
    std::list<ResultPtr>::iterator aNewContIter = aNewToIterate.begin();
    for(; aNewContIter != aNewToIterate.end(); aNewContIter++) {
      std::shared_ptr<Model_Data> aNewData =
        std::dynamic_pointer_cast<Model_Data>((*aNewContIter)->data());
      TDF_Label aNewLab = aNewData->shapeLab();
      // searching for produced sub-shape fully on some label
      TDF_ChildIDIterator aNSIter(aNewLab, TNaming_NamedShape::GetID());
      for(; aNSIter.More(); aNSIter.Next()) {
        Handle(TNaming_NamedShape) aNS = Handle(TNaming_NamedShape)::DownCast(aNSIter.Value());
        for(TNaming_Iterator aPairIter(aNS); aPairIter.More(); aPairIter.Next()) {
          if (aToFindPart == 0) { // search shape is fully inside
            if (aPairIter.OldShape().IsSame(theValShape)) {
              if (aPairIter.NewShape().IsNull()) {// value was removed
                theShapes.Clear();
                return;
              }
              // don't add edges generated from faces
              if (aPairIter.NewShape().ShapeType() <= aValType)
                theShapes.Append(aPairIter.NewShape());
            }
          } else if (!aPairIter.OldShape().IsNull()) { // search shape that contains this sub
            TopExp_Explorer anExp(aPairIter.OldShape(), aValType);
            for(; anExp.More(); anExp.Next()) {
              if (anExp.Current().IsSame(theValShape)) { // found a new container
                if (aPairIter.NewShape().IsNull()) // skip removed high-level shape
                  continue;
                aNewToOld.Bind(aPairIter.NewShape(), aPairIter.OldShape());
                anOlds.Add(aPairIter.OldShape());
                break;
              }
            }
          }
        }
      }
    }
  }
  if (aToFindPart == 2 && !aNewToOld.IsEmpty()) {
    // also iterate the whole old shape to find not-modified shapes that contain this old
    TopoDS_Shape anOldContShape = theOldContext->shape()->impl<TopoDS_Shape>();
    NCollection_Map<TopAbs_ShapeEnum> aNewTypes; // types of shapes to iterate
    TopTools_DataMapOfShapeShape::Iterator aNewTypeIter(aNewToOld);
    for(; aNewTypeIter.More(); aNewTypeIter.Next()) {
      if (aNewTypeIter.Key().ShapeType() != aValType)
        aNewTypes.Add(aNewTypeIter.Key().ShapeType());
    }
    NCollection_Map<TopAbs_ShapeEnum>::Iterator aTypeIter(aNewTypes);
    for(; aTypeIter.More(); aTypeIter.Next()) {
      for(TopExp_Explorer anExp(anOldContShape, aTypeIter.Value()); anExp.More(); anExp.Next()) {
        TopoDS_Shape anOld = anExp.Current();
        if (aNewToOld.IsBound(anOld) || anOlds.Contains(anOld)) // this was modified
          continue;
        TopExp_Explorer aValExp(anOld, aValType);
        for(; aValExp.More(); aValExp.Next()) {
          const TopoDS_Shape& anUnchanged = aValExp.Current();
          if (anUnchanged.IsSame(theValShape)) {
            aNewToOld.Bind(anOld, anOld);
            anOlds.Add(anOld);
            break;
          }
        }
      }
    }

    // map of sub-shapes -> number of occurrences of these shapes in containers
    NCollection_DataMap<TopoDS_Shape, TopTools_MapOfShape, TopTools_ShapeMapHasher> aSubs;
    TopTools_DataMapOfShapeShape::Iterator aContIter(aNewToOld);
    for(; aContIter.More(); aContIter.Next()) {
      for(aSubExp.Init(aContIter.Key(), aValType); aSubExp.More(); aSubExp.Next()) {
        if (!aSubs.IsBound(aSubExp.Current())) {
          aSubs.Bind(aSubExp.Current(), TopTools_MapOfShape());
        }
        // store old to know how many olds produced this shape
        aSubs.ChangeFind(aSubExp.Current()).Add(aContIter.Value());
      }
    }
    // if sub is appeared same times in containers as the number of old shapes that contain it
    int aCountInOld = anOlds.Size();
    NCollection_DataMap<TopoDS_Shape, TopTools_MapOfShape, TopTools_ShapeMapHasher>::Iterator
      aSubsIter(aSubs);
    for(; aSubsIter.More(); aSubsIter.Next()) {
      if (aSubsIter.Value().Size() == aCountInOld) {
        TopoDS_Shape anOld = aSubsIter.Key();
        // check this exists in the new shape
        TopExp_Explorer aNew(aNewContShape, anOld.ShapeType());
        for (; aNew.More(); aNew.Next()) {
          if (aNew.Current().IsSame(anOld))
            break;
        }
        if (aNew.More())
          theShapes.Append(anOld);
      }
    }
  }
  if (theShapes.IsEmpty()) { // nothing was changed
    if (aWasWholeContext)
      theShapes.Append(TopoDS_Shape());
    else { // if theValShape exists in new context, add it without changes, otherwise - nothing
      for (TopExp_Explorer aNew(aNewContShape, aValType); aNew.More(); aNew.Next()){
        if (aNew.Current().IsSame(theValShape)) {
          theShapes.Append(theValShape);
          break;
        }
      }
    }
  } else if (theShapes.Size() > 1) {
    // check it is possible to remove extra sub-shapes:
    // keep only shapes with the same number of containers if possible
    TopAbs_ShapeEnum anAncType = TopAbs_FACE;
    if (aValType == TopAbs_VERTEX)
      anAncType = TopAbs_EDGE;
    TopoDS_Shape anOldContext = theOldContext->shape()->impl<TopoDS_Shape>();
    TopTools_IndexedDataMapOfShapeListOfShape anOldMap;
    TopExp::MapShapesAndUniqueAncestors(anOldContext, aValType,  anAncType, anOldMap);
    if (anOldMap.Contains(theValShape)) {
      int aNumInOld = anOldMap.FindFromKey(theValShape).Extent();
      TopTools_IndexedDataMapOfShapeListOfShape aNewMap;
      TopExp::MapShapesAndUniqueAncestors(aNewContShape, aValType,  anAncType, aNewMap);
      TopTools_ListOfShape aNewResults;
      for(TopTools_ListOfShape::Iterator aNewSubs(theShapes); aNewSubs.More(); aNewSubs.Next()) {
        TopoDS_Shape aCand = aNewSubs.Value();
        if (aNewMap.Contains(aCand) && aNewMap.FindFromKey(aCand).Extent() == aNumInOld)
          aNewResults.Append(aCand);
      }
      if (!aNewResults.IsEmpty() && aNewResults.Size() < theShapes.Size())
        theShapes = aNewResults;
    }
  }
}


void Model_AttributeSelection::concealedFeature(
  const FeaturePtr theFeature, const FeaturePtr theStop, const bool theCheckCopy,
  std::list<FeaturePtr>& theConcealers, const ResultPtr theResultOfFeature,
  const bool theCheckWholeFeature)
{
  std::set<FeaturePtr> alreadyProcessed;
  alreadyProcessed.insert(theFeature);
  if (theStop.get())
    alreadyProcessed.insert(theStop);
  /// iterate all results to find the concealment-attribute
  std::list<ResultPtr> aRootRes;
  if (theResultOfFeature.get()) {
    ResultPtr aRoot = ModelAPI_Tools::bodyOwner(theResultOfFeature, true);
    aRootRes.push_back(aRoot ? aRoot : theResultOfFeature);
  } else { // all results of a feature
   aRootRes = theFeature->results();
  }
  std::list<ResultPtr>::const_iterator aRootIter = aRootRes.cbegin();
  for(; aRootIter != aRootRes.cend(); aRootIter++) {
    std::list<DataPtr> allRes;
    allRes.push_back((*aRootIter)->data());
    ResultBodyPtr aRootBody = ModelAPI_Tools::bodyOwner(*aRootIter, true);
    if (!aRootBody.get())
      aRootBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aRootIter);
    if (aRootBody.get()) {
      std::list<ResultPtr> allSub;
      ModelAPI_Tools::allSubs(aRootBody, allSub);
      for(std::list<ResultPtr>::iterator anIt = allSub.begin(); anIt != allSub.end(); anIt++)
        allRes.push_back((*anIt)->data());
    }
    if (theCheckWholeFeature)
      allRes.push_back(theFeature->data());
    for(std::list<DataPtr>::iterator aRIter = allRes.begin(); aRIter != allRes.end(); aRIter++) {
      const std::set<AttributePtr>& aRefs = (*aRIter)->refsToMe();
      std::set<AttributePtr>::const_iterator aRef = aRefs.cbegin();
      for (; aRef != aRefs.cend(); aRef++) {
        if (!aRef->get() || !(*aRef)->owner().get())
          continue;
        // concealed attribute only
        FeaturePtr aRefFeat = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRef)->owner());
        if (alreadyProcessed.find(aRefFeat) != alreadyProcessed.end()) // optimization
          continue;
        alreadyProcessed.insert(aRefFeat);
        if (ModelAPI_Session::get()->validators()->isConcealed(aRefFeat->getKind(), (*aRef)->id())
          || (theCheckCopy &&
              std::dynamic_pointer_cast<ModelAPI_FeatureCopyInterface>(aRefFeat).get()))
        {
          // for extrusion cut in python script the nested sketch reference may be concealed before
          // it is nested, so, check this composite feature is valid
          static ModelAPI_ValidatorsFactory* aFactory = ModelAPI_Session::get()->validators();
          // need to be validated to update the "Apply" state if not previewed
          if (aFactory->validate(aRefFeat)) {
            if (theStop.get()) {
              std::shared_ptr<Model_Document> aDoc =
                std::dynamic_pointer_cast<Model_Document>(theStop->document());
              if (!aDoc->isLaterByDep(theStop, aRefFeat)) // skip feature later than stop
                continue;
            }
            theConcealers.push_back(aRefFeat);
          }
        }
      }
    }
  }
}

bool Model_AttributeSelection::searchNewContext(std::shared_ptr<Model_Document> theDoc,
  const TopoDS_Shape theContShape, ResultPtr theContext, TopoDS_Shape theValShape,
  TDF_Label theAccessLabel,
  std::list<ResultPtr>& theResults, TopTools_ListOfShape& theValShapes)
{
  std::list<ResultPtr> aResults; // keep order, new context, null if deleted
  std::set<ResultPtr> aResultsSet; // to avoid duplicates
  // iterate context and shape, but also if it is sub-shape of main shape, check also it
  TopTools_ListOfShape aContextList;
  aContextList.Append(theContShape);
  if (theContext.get()) {
    ResultPtr aComposite = ModelAPI_Tools::bodyOwner(theContext);
    if (aComposite.get() && aComposite->shape().get() && !aComposite->shape()->isNull())
      aContextList.Append(aComposite->shape()->impl<TopoDS_Shape>());
  }
  for(TopTools_ListOfShape::Iterator aContIter(aContextList); aContIter.More(); aContIter.Next()) {
    TNaming_SameShapeIterator aModifIter(aContIter.ChangeValue(), theAccessLabel);
    for(; aModifIter.More(); aModifIter.Next()) {
      TDF_Label anObjLab = aModifIter.Label().Father();
      ResultPtr aModifierObj = std::dynamic_pointer_cast<ModelAPI_Result>
        (theDoc->objects()->object(anObjLab));
      if (!aModifierObj.get()) {
        // #2241: shape may be sub-element of new object, not main (shell created from faces)
        if (!anObjLab.IsRoot())
          aModifierObj = std::dynamic_pointer_cast<ModelAPI_Result>
          (theDoc->objects()->object(anObjLab.Father()));
        if (!aModifierObj.get())
          continue;
      }
      FeaturePtr aModifierFeat = theDoc->feature(aModifierObj);
      if (!aModifierFeat.get())
        continue;
      FeaturePtr aThisFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
      if (aModifierFeat == aThisFeature || !theDoc->isLaterByDep(aThisFeature, aModifierFeat))
        continue; // the modifier feature is later than this, so, should not be used
      FeaturePtr aCurrentModifierFeat = theDoc->feature(theContext);
      if (aCurrentModifierFeat == aModifierFeat ||
        !theDoc->isLaterByDep(aModifierFeat, aCurrentModifierFeat))
        continue; // the current modifier is later than the found, so, useless
      Handle(TNaming_NamedShape) aNewNS;
      aModifIter.Label().FindAttribute(TNaming_NamedShape::GetID(), aNewNS);
      if (aNewNS->Evolution() == TNaming_MODIFY || aNewNS->Evolution() == TNaming_GENERATED) {
        if (aResultsSet.find(aModifierObj) == aResultsSet.end()) {
          aResultsSet.insert(aModifierObj);
          aResults.push_back(aModifierObj);
        }
      } else if (aNewNS->Evolution() == TNaming_DELETE) { // a shape was deleted => result is empty
        aResults.push_back(ResultPtr());
      } else { // not-processed modification => don't support it
        continue;
      }
    }
  }
  // if there exist context composite and sub-result(s), leave only sub(s)
  for(std::list<ResultPtr>::iterator aResIter = aResults.begin(); aResIter != aResults.end();) {
    ResultPtr aParent = ModelAPI_Tools::bodyOwner(*aResIter);
    for(; aParent.get(); aParent = ModelAPI_Tools::bodyOwner(aParent))
      if (aResultsSet.count(aParent))
        break;
    if (aParent.get()) {
      aResultsSet.erase(aParent);
      for(std::list<ResultPtr>::iterator anIt = aResults.begin(); anIt != aResults.end(); anIt++) {
        if (*anIt == aParent) {
          aResults.erase(anIt);
          aResIter = aResults.begin(); // erase from set, so, restart iteration
          break;
        }
      }
    } else aResIter++;
  }

  bool aStaySame = false;
  if (aResults.empty()) {
    // check the context become concealed by operation which is earlier than this selection
    FeaturePtr aThisFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
    FeaturePtr aContextOwner = theDoc->feature(theContext);
    std::list<FeaturePtr> aConcealers;
    concealedFeature(aContextOwner, aThisFeature, false, aConcealers, theContext);
    std::list<FeaturePtr>::iterator aConcealer = aConcealers.begin();
    for(; aConcealer != aConcealers.end(); aConcealer++) {
      std::list<ResultPtr> aRefResults;
      ModelAPI_Tools::allResults(*aConcealer, aRefResults);
      std::list<ResultPtr>::iterator aRefIter = aRefResults.begin();
      for(; aRefIter != aRefResults.end(); aRefIter++) {
        ResultBodyPtr aRefBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aRefIter);
        if (!aRefBody.get() || aRefBody->numberOfSubs() != 0) // iterate only leafs
          continue;
        GeomShapePtr aRefShape = aRefBody->shape();
        if (!aRefShape.get() || aRefShape->isNull())
          continue;
        if (aRefShape->impl<TopoDS_Shape>().IsSame(theContShape)) {
          // add the new context result with the same shape
          aResults.push_back(aRefBody);
        }
      }
      if (aResults.empty())
        return true; // feature conceals result, return true, so the context will be removed
    }
    aStaySame = aResults.empty();
  }
  if (myParent && myParent->isMakeCopy()) {
    // check there are copies before the new results, so, make a copy
    std::list<ObjectPtr> aCopyContext;
    std::list<GeomShapePtr> aCopyVals;
    // features between the new and the old: check the "Move" interface to get a copy
    FeaturePtr aRootOwner = theDoc->feature(theContext);
    FeaturePtr anOwner = ModelAPI_Tools::compositeOwner(aRootOwner);
    for(; anOwner.get(); anOwner = ModelAPI_Tools::compositeOwner(anOwner))
      aRootOwner = anOwner;
    FeaturePtr aThisFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
    // iterate all results to find a "Copy" features between the new and one and to add the
    // copy-results also to results if this attribute refers to the copied shape
    int anIndex = kUNDEFINED_FEATURE_INDEX;
    for(FeaturePtr aFeat = theDoc->objects()->nextFeature(aRootOwner, anIndex); aFeat.get() &&
        aFeat != aThisFeature; aFeat = theDoc->objects()->nextFeature(aFeat, anIndex)) {
      std::shared_ptr<ModelAPI_FeatureCopyInterface> aCopier =
        std::dynamic_pointer_cast<ModelAPI_FeatureCopyInterface>(aFeat);
      if (aCopier.get()) {
        GeomShapePtr aValShape(new GeomAPI_Shape);
        aValShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(
          theValShape.IsNull() ? theContShape : theValShape));
        aCopier->getCopies(theContext, aValShape, aCopyContext, aCopyVals);
      }
    }
    // check for the further modifications of the copy contexts and values
    std::list<ObjectPtr>::iterator aCopyContIter = aCopyContext.begin();
    std::list<GeomShapePtr>::iterator aCopyValIter = aCopyVals.begin();
    for(; aCopyContIter != aCopyContext.end(); aCopyContIter++, aCopyValIter++) {
      ResultPtr aNewCont = std::dynamic_pointer_cast<ModelAPI_Result>(*aCopyContIter);
      TopoDS_Shape aNewContShape = aNewCont->shape()->impl<TopoDS_Shape>();
      GeomShapePtr aNewVal = *aCopyValIter;
      TopoDS_Shape aNewValShape;
      if (aNewVal.get() && !aNewVal->isNull())
        aNewValShape = aNewVal->impl<TopoDS_Shape>();
      std::list<ResultPtr> aNewRes;
      TopTools_ListOfShape aNewUpdatedVal;
      if (searchNewContext(theDoc, aNewContShape, aNewCont, aNewValShape,
          theAccessLabel, aNewRes, aNewUpdatedVal)) {
        // append new results instead of the current ones
        std::list<ResultPtr>::iterator aNewIter = aNewRes.begin();
        TopTools_ListIteratorOfListOfShape aNewUpdVal(aNewUpdatedVal);
        for(; aNewIter != aNewRes.end(); aNewIter++, aNewUpdVal.Next()) {
          theResults.push_back(*aNewIter);
          theValShapes.Append(aNewUpdVal.Value());
        }
      } else { // the current result is good
        theResults.push_back(aNewCont);
        theValShapes.Append(aNewValShape);
      }
    }
    if (aStaySame && !theResults.empty()) { // no changes except copy, so, keep the origin as first
      theResults.push_front(theContext);
      theValShapes.Prepend(theValShape);
      return true;
    }
  }
  if (aStaySame)
    return false;

  // iterate all results to find further modifications
  std::list<ResultPtr>::iterator aResIter = aResults.begin();
  for(aResIter = aResults.begin(); aResIter != aResults.end(); aResIter++) {
    if (aResIter->get() != NULL) {
      ResultPtr aNewResObj = *aResIter;
      // compute new values by two contexts: the old and the new
      TopTools_ListOfShape aValShapes;
      computeValues(theContext, aNewResObj, theValShape, aValShapes);

      TopTools_ListIteratorOfListOfShape aNewVal(aValShapes);
      for(; aNewVal.More(); aNewVal.Next()) {
        std::list<ResultPtr> aNewRes;
        TopTools_ListOfShape aNewUpdatedVal;
        TopoDS_Shape aNewValSh = aNewVal.Value();
        TopoDS_Shape aNewContShape = aNewResObj->shape()->impl<TopoDS_Shape>();

        if (theValShape.IsNull() && aNewContShape.IsSame(aNewValSh))
          aNewValSh.Nullify();
        if (searchNewContext(theDoc, aNewContShape, aNewResObj, aNewValSh,
                             theAccessLabel, aNewRes, aNewUpdatedVal))
        {
          // append new results instead of the current ones
          std::list<ResultPtr>::iterator aNewIter = aNewRes.begin();
          TopTools_ListIteratorOfListOfShape aNewUpdVal(aNewUpdatedVal);
          for(; aNewIter != aNewRes.end(); aNewIter++, aNewUpdVal.Next()) {
            theResults.push_back(*aNewIter);
            theValShapes.Append(aNewUpdVal.Value());
          }
        } else { // the current result is good
          theResults.push_back(aNewResObj);
          theValShapes.Append(aNewValSh);
        }
      }
    }
  }
  return true; // theResults must be empty: everything is deleted
}

void Model_AttributeSelection::updateInHistory(bool& theRemove)
{
  static std::shared_ptr<GeomAPI_Shape> anEmptyShape;

  ResultPtr aContext = std::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
  if (!aContext.get() || (aContext->groupName() != ModelAPI_ResultBody::group() &&
      aContext->groupName() != ModelAPI_ResultPart::group())) {
    // but check the case the whole results are allowed: whole features may be selected
    if (myParent && myParent->isWholeResultAllowed()) {
      FeaturePtr aFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(myRef.value());
      if (aFeature.get()) {
        FeaturePtr aThisFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
        std::list<FeaturePtr> aConcealers;
        bool aCopyPossible = myParent && myParent->isMakeCopy();
        concealedFeature(aFeature, aThisFeature, aCopyPossible, aConcealers, ResultPtr());
        if (aConcealers.empty())
          return;
        // if there are copies, but no direct modification, keep the original
        bool aKeepOrigin = false;
        if (aCopyPossible) {
          std::list<FeaturePtr>::iterator aConcealer = aConcealers.begin();
          for(aKeepOrigin = true; aConcealer != aConcealers.end(); aConcealer++)
            if (!std::dynamic_pointer_cast<ModelAPI_FeatureCopyInterface>(*aConcealer).get()) {
              aKeepOrigin = false;
              break;
            }
          if (aKeepOrigin) {
            aConcealers.push_front(aFeature);
          }
        }
        bool aChanged = false;
        std::list<FeaturePtr>::iterator aConcealer = aConcealers.begin();
        for(; aConcealer != aConcealers.end(); aConcealer++)
          if (aChanged) {
            if (aKeepOrigin || !myParent->isInList(*aConcealer, anEmptyShape))
              myParent->append(*aConcealer, anEmptyShape);
          } else {
            if (!myParent->isInList(*aConcealer, anEmptyShape)) {// avoid addition of duplicates
              setValue(*aConcealer, anEmptyShape);
              aChanged = true;
            } else if (aCopyPossible && *aConcealer == aFeature) {// keep origin in case of copy
              aChanged = true;
            }
          }
        if (!aChanged) // remove this
          theRemove = true;
        else if (!aKeepOrigin) // searching further modifications only if current changed
          updateInHistory(theRemove);
      }
    }
    return;// only bodies and parts may be modified later in the history, skip otherwise
  }

  std::shared_ptr<Model_Document> aDoc =
    std::dynamic_pointer_cast<Model_Document>(aContext->document());
  std::shared_ptr<Model_Data> aContData = std::dynamic_pointer_cast<Model_Data>(aContext->data());
  if (!aContData.get() || !aContData->isValid())
    return;
  TDF_Label aContLab = aContData->shapeLab(); // named shape where the selected context is located

  // checking this may be just a reference to another context (same shape), so use that label
  Handle(TNaming_NamedShape) aContNS;
  Handle(TDF_Reference) aRefAttr;
  while(!aContLab.FindAttribute(TNaming_NamedShape::GetID(), aContNS) &&
        aContLab.FindAttribute(TDF_Reference::GetID(), aRefAttr))
    aContLab = aRefAttr->Get();

  if (!aContLab.FindAttribute(TNaming_NamedShape::GetID(), aContNS)) {
    bool aFoundNewContext = true;
    ResultPtr aNewContext = aContext;
    while(aFoundNewContext) {
      aFoundNewContext = false;
      // parts have no shape in result, so, trace references using the Part info
      if (aNewContext->groupName() == ModelAPI_ResultPart::group()) {
        ResultPartPtr aPartContext = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aNewContext);
        if (aPartContext.get()) { // searching for the up to date references to the referenced cont
          const std::set<AttributePtr>& aRefs = aPartContext->data()->refsToMe();
          std::set<AttributePtr>::const_iterator aRef = aRefs.begin();
          for(; aRef != aRefs.end(); aRef++) {
            // to avoid detection of part changes by local selection only
            AttributeSelectionPtr aSel =
              std::dynamic_pointer_cast<ModelAPI_AttributeSelection>(*aRef);
            if (aSel.get() && aSel->value().get() &&
                !aSel->value()->isSame(aSel->context()->shape()))
              continue;

            FeaturePtr aRefFeat = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRef)->owner());

            if (aRefFeat.get() && aRefFeat != owner() && aRefFeat->firstResult().get()) {
              // check the reference is concealed: #2900
              ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();
              if (!aValidators->isConcealed(aRefFeat->getKind(), (*aRef)->id()))
                continue;
              FeaturePtr aThisFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
              if (!aDoc->isLaterByDep(aRefFeat, aThisFeature)) { // found better feature
                aFoundNewContext = true;
                aNewContext = aRefFeat->firstResult();
              }
            }
          }
        }
      }
    }
    if (aNewContext != aContext) {
      setValue(aNewContext, value());
    }
    return;
  }
  FeaturePtr aThisFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
  FeaturePtr aCurrentModifierFeat = aDoc->feature(aContext);
  // iterate the context shape modifications in order to find a feature that is upper in history
  // that this one and is really modifies the referenced result to refer to it
  ResultPtr aModifierResFound;
  TNaming_Iterator aPairIter(aContNS);
  if (!aPairIter.More())
    return;
  TopoDS_Shape aNewCShape = aPairIter.NewShape();
  // trying to update also the sub-shape selected
  GeomShapePtr aSubShape = value();
  if (aSubShape.get() && aSubShape->isEqual(aContext->shape()))
    aSubShape.reset();
  TopoDS_Shape aValShape;
  if (aSubShape.get()) {
    aValShape = aSubShape->impl<TopoDS_Shape>();
  }

  std::list<ResultPtr> aNewContexts;
  TopTools_ListOfShape aValShapes;
  if (searchNewContext(aDoc, aNewCShape, aContext, aValShape, aContLab, aNewContexts, aValShapes))
  {
    std::set<ResultPtr> allContexts(aNewContexts.begin(), aNewContexts.end());
    std::set<ResultPtr> aSkippedContext;

    // if there exist context composite and sub-result(s), leave only sub(s)
    std::set<ResultPtr>::iterator aResIter = allContexts.begin();
    for(; aResIter != allContexts.end(); aResIter++) {
      ResultPtr aParent = ModelAPI_Tools::bodyOwner(*aResIter);
      for(; aParent.get(); aParent = ModelAPI_Tools::bodyOwner(aParent))
        if (allContexts.count(aParent))
          aSkippedContext.insert(aParent);
    }

    GeomAPI_Shape::ShapeType aListShapeType = GeomAPI_Shape::SHAPE;
    if (myParent) {
      if (myParent->selectionType() == "VERTEX" || myParent->selectionType() == "Vertices")
        aListShapeType = GeomAPI_Shape::VERTEX;
      else if (myParent->selectionType() == "EDGE" || myParent->selectionType() == "Edges")
        aListShapeType = GeomAPI_Shape::EDGE;
      else if (myParent->selectionType() == "FACE" || myParent->selectionType() == "Faces")
        aListShapeType = GeomAPI_Shape::FACE;
    }

    // issue #3031: skip topology if there is more convenient shape type presents in the
    // same context as a result of this
    bool isWholeResult = myParent && myParent->isWholeResultAllowed() && !aSubShape.get();
    GeomAPI_Shape::ShapeType allowedType = GeomAPI_Shape::SHAPE;
    if (isWholeResult) {
      std::list<ResultPtr>::iterator aNewCont = aNewContexts.begin();
      TopTools_ListIteratorOfListOfShape aNewValues(aValShapes);
      for(; aNewCont != aNewContexts.end(); aNewCont++, aNewValues.Next()) {
        if (aNewValues.Value().IsNull()) { // only for the whole context
          GeomAPI_Shape::ShapeType aShapeType = (*aNewCont)->shape()->shapeType();
          if (allowedType == GeomAPI_Shape::SHAPE) { // just set this one
            allowedType = aShapeType;
          } else {
            GeomAPI_Shape::ShapeType anAllowed = allowedType;
            if (anAllowed != aShapeType) { // select the best, nearest to the origin
              GeomAPI_Shape::ShapeType anOldShapeType = aContext->shape()->shapeType();
              GeomAPI_Shape::ShapeType aDeltaAllowed =
                (GeomAPI_Shape::ShapeType)(anOldShapeType - anAllowed);
              if (aDeltaAllowed < 0)
                aDeltaAllowed = (GeomAPI_Shape::ShapeType)(-aDeltaAllowed);
              GeomAPI_Shape::ShapeType aDeltaThis =
                (GeomAPI_Shape::ShapeType)(anOldShapeType - aShapeType);
              if (aDeltaThis < 0)
                aDeltaThis = (GeomAPI_Shape::ShapeType)(-aDeltaThis);
              if (aDeltaThis == aDeltaAllowed) { // equal distance to context, select complicated
                if (anOldShapeType < anAllowed)
                  allowedType = aShapeType;
              } else if (aDeltaAllowed > aDeltaThis) { // this wins
                allowedType = aShapeType;
              }
            }
          }
        }
      }
    }

    std::list<ResultPtr>::iterator aNewCont = aNewContexts.begin();
    TopTools_ListIteratorOfListOfShape aNewValues(aValShapes);
    bool aFirst = true; // first is set to this, next are appended to parent
    for(; aNewCont != aNewContexts.end(); aNewCont++, aNewValues.Next()) {
      if (aSkippedContext.count(*aNewCont))
        continue;

      if (isWholeResult && aNewValues.Value().IsNull())
        if (allowedType != GeomAPI_Shape::SHAPE &&
            (*aNewCont)->shape()->shapeType() != allowedType)
          continue; // there is better result exists with the better shape type (issue #3031)

      GeomShapePtr aValueShape;
      if (!aNewValues.Value().IsNull()) {
        aValueShape = std::make_shared<GeomAPI_Shape>();
        aValueShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aNewValues.Value()));
      }
      // Check that list has the same type of shape selection before adding.
      GeomAPI_Shape::ShapeType aShapeShapeType = GeomAPI_Shape::SHAPE;
      if (aValueShape.get()) {
        aShapeShapeType = aValueShape->shapeType();
      } else {
        aShapeShapeType = (*aNewCont)->shape()->shapeType();
      }
      if (aListShapeType != GeomAPI_Shape::SHAPE && aListShapeType != aShapeShapeType) {
        // exception is for whole results selected
        if (!isWholeResult) {
          continue;
        }
      }

      ResultPtr aNewContext = *aNewCont;
      if (aValueShape.get()) { // #2892 if context is higher level result, search this sub in lower
        ResultBodyPtr aBodyContext = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aNewContext);
        if (aBodyContext.get() && aBodyContext->numberOfSubs() != 0) {
          std::list<ResultPtr> aLower;
          ModelAPI_Tools::allSubs(aBodyContext, aLower, true);
          for(std::list<ResultPtr>::iterator aL = aLower.begin(); aL != aLower.end(); aL++) {
            GeomShapePtr aLShape = (*aL)->shape();
            if (aLShape.get() && !aLShape->isNull()) {
              if (aLShape->isSubShape(aValueShape, false)) {
                aNewContext = *aL;
                break;
              }
            }
          }
        }
      }

      if (aFirst) {
        if (!myParent || !myParent->isInList(aNewContext, aValueShape)) { // avoid duplicates
          setValue(aNewContext, aValueShape);
          aFirst = false;
        } else if (aNewContext == aContext && myParent && myParent->isMakeCopy()) {
          // this may be exactly the old one, not modified in case of copy
          aFirst = false;
        }
      } else if (myParent) {
        if (!myParent->isInList(aNewContext, aValueShape)) // avoid addition of duplicates
          myParent->append(aNewContext, aValueShape);
      }
    }
    if (aFirst) { // nothing was added, all results were deleted
      if (myParent) {
        theRemove = true;
      } else {
        static ResultPtr anEmptyContext;
        setValue(anEmptyContext, anEmptyShape); // nullify the selection
        return;
      }
    }
  }
}

void Model_AttributeSelection::setParent(Model_AttributeSelectionList* theParent)
{
  myParent = theParent;
}

std::string Model_AttributeSelection::contextName(const TDF_Label theSelectionLab)
{
  std::shared_ptr<Model_Document> aDoc = myRestoreDocument.get() ? myRestoreDocument :
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  FeaturePtr aFeatureOwner = aDoc->featureByLab(theSelectionLab);
  bool aBaseDocumnetUsed = false;
  if (!aFeatureOwner.get()) { // use module document
    aDoc = std::dynamic_pointer_cast<Model_Document>(ModelAPI_Session::get()->moduleDocument());
    aFeatureOwner = aDoc->featureByLab(theSelectionLab);
    aBaseDocumnetUsed = true;
  }
  if (aFeatureOwner.get()) {
    // if it is sub-element of the sketch, the context name is the name of the sketch
    // searching also for result - real context
    ResultPtr aResult;
    FeaturePtr aComposite = ModelAPI_Tools::compositeOwner(aFeatureOwner);
    if (aComposite.get() && aComposite->results().size() == 1 &&
        aComposite->firstResult()->groupName() == ModelAPI_ResultConstruction::group()) {
      aFeatureOwner = aComposite;
      aResult = aFeatureOwner->firstResult();
    } else {
      aResult = aDoc->resultByLab(theSelectionLab);
    }
    if (aResult.get()) {
      // this is to avoid duplicated names of results problem
      std::string aContextName = aResult->data()->name();
      // myLab corresponds to the current time
      TDF_Label aCurrentLab = selectionLabel();
      while(aCurrentLab.Depth() > 3)
        aCurrentLab = aCurrentLab.Father();

      int aNumInHistoryNames =
        aDoc->numberOfNameInHistory(aResult, aCurrentLab);
      while(aNumInHistoryNames > 1) { // add "_" before name the needed number of times
        aContextName = "_" + aContextName;
        aNumInHistoryNames--;
      }
      if (aBaseDocumnetUsed)
        aContextName = aDoc->kind() + "/" + aContextName;
      return aContextName;
    }
  }
  return ""; // invalid case
}

/// This method restores by the context and value name the context label and
/// sub-label where the value is. Returns true if it is valid.
bool Model_AttributeSelection::restoreContext(std::string theName,
  TDF_Label& theContext, TDF_Label& theValue)
{
  static const GeomShapePtr anEmptyShape; // to store context only
  std::string aName = theName;
  std::shared_ptr<Model_Document> aDoc = myRestoreDocument.get() ? myRestoreDocument :
    std::dynamic_pointer_cast<Model_Document>(owner()->document());

  // remove the sub-value part if exists
  std::string aSubShapeName = aName;
  std::string::size_type n = aName.find('/');
  if (n != std::string::npos) {
    aName = aName.substr(0, n);
  }

  if (aName.empty()) return false;
  bool anUniqueContext = false;
  ResultPtr aCont = aDoc->findByName(aName, aSubShapeName, anUniqueContext);
  if (!aCont.get() || !aCont->shape().get() || aCont->shape()->isNull()) {
    // name in PartSet?
    aDoc = std::dynamic_pointer_cast<Model_Document>(
      ModelAPI_Session::get()->moduleDocument());
    if (theName.find(aDoc->kind()) == 0) { // remove the document identifier from name if exists
      aSubShapeName = theName.substr(aDoc->kind().size() + 1);
      aName = aSubShapeName;
      n = aName.find('/');
      if (n != std::string::npos) {
        aName = aName.substr(0, n);
      }
    }
    aCont = aDoc->findByName(aName, aSubShapeName, anUniqueContext);
    if (!aCont.get() || !aCont->shape().get() || aCont->shape()->isNull())
      return false;
  }

  // searching the sub-shape
  static const ResultPtr anEmpty;
  theValue = aDoc->findNamingName(aSubShapeName, anUniqueContext ? aCont : anEmpty);

  // sketch sub-component shape and name is located in separated feature label, try the sub-name
  if (theValue.IsNull() && aCont->groupName() == ModelAPI_ResultConstruction::group()) {
    std::string::size_type aSlash = aSubShapeName.rfind('/');
    if (aSlash != std::string::npos) {
      std::string aCompName = aSubShapeName.substr(aSlash + 1);
      CompositeFeaturePtr aComposite =
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aDoc->feature(aCont));
      if (aComposite.get() && aComposite->numberOfSubs()) {
        const int aSubNum = aComposite->numberOfSubs();
        for (int a = 0; a < aSubNum && theValue.IsNull(); a++) {
          FeaturePtr aSub = aComposite->subFeature(a);
          const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
          std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
          for (; aRes != aResults.cend() && theValue.IsNull(); aRes++) {
            if ((*aRes)->data()->name() == aCompName) {
              theValue = std::dynamic_pointer_cast<Model_Data>((*aRes)->data())->shapeLab();
              break;
            } else { // any sub-label because the sketch line may be renamed, but not sub-vertices
              TDF_Label aLab = std::dynamic_pointer_cast<Model_Data>((*aRes)->data())->shapeLab();
              TDF_ChildIDIterator aSubNames(aLab, TDataStd_Name::GetID());
              for(; aSubNames.More(); aSubNames.Next()) {
                if (Handle(TDataStd_Name)::DownCast(aSubNames.Value())->Get().
                  IsEqual(aCompName.c_str())) {
                  theValue = aSubNames.Value()->Label();
                  break;
                }
              }
            }
          }
        }
      }
    }
  }

  if (aCont.get()) {
    theContext = std::dynamic_pointer_cast<Model_Data>(aCont->data())->label();
  }
  return true;
}

bool Model_AttributeSelection::isLater(
  const TDF_Label theResult1, const TDF_Label theResult2) const
{
  std::shared_ptr<Model_Document> aDoc = myRestoreDocument.get() ? myRestoreDocument :
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  FeaturePtr aFeat1 = aDoc->featureByLab(theResult1);
  if (!aFeat1.get())
    return false;
  FeaturePtr aFeat2 = aDoc->featureByLab(theResult2);
  if (!aFeat2.get())
    return false;
  return aDoc->isLaterByDep(aFeat1, aFeat2);
}

ResultPtr Model_AttributeSelection::newestContext(
  const ResultPtr theCurrent, const GeomShapePtr theValue)
{
  ResultPtr aResult = theCurrent;
  GeomShapePtr aSelectedShape = theValue.get() ? theValue : theCurrent->shape();
  std::shared_ptr<Model_Document> aDoc =
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  bool aFindNewContext = true;
  while (aFindNewContext && aResult.get()) {
    aFindNewContext = false;
    // try to find the last context to find the up to date shape
    TopoDS_Shape aConShape = aResult->shape()->impl<TopoDS_Shape>();
    if (TNaming_Tool::HasLabel(selectionLabel(), aConShape)) {
      Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aConShape, selectionLabel());
      if (!aNS.IsNull()) {
        aNS = TNaming_Tool::CurrentNamedShape(aNS);
        if (!aNS.IsNull()
            && isLater(selectionLabel(), aNS->Label())
            && isLater(aNS->Label(),
                       std::dynamic_pointer_cast<Model_Data>(aResult->data())->label()))
        {
          TDF_Label aLab = aNS->Label();
          ResultPtr aRes = aDoc->resultByLab(aLab);
          if (aRes.get()) {
            if (aRes->shape()->isSubShape(aSelectedShape)) {
              aResult = aRes;
              aFindNewContext = true;
              continue;
            }
          }
        }
      }
    }

    // TestFillWireVertex.py - sketch constructions for wire may participate too
    //if (aResult->groupName() == ModelAPI_ResultBody::group()) {
      // try to search newer context by the concealment references
      // take references to all results: root one, any sub
    std::list<ResultPtr> allRes;
    ResultPtr aCompContext;
    ResultBodyPtr aCompBody = ModelAPI_Tools::bodyOwner(aResult, true);
    if (aCompBody.get()) {
      ModelAPI_Tools::allSubs(aCompBody, allRes);
      allRes.push_back(aCompBody);
      aCompContext = aCompBody;
    }
    if (allRes.empty())
      allRes.push_back(aResult);

    bool aFoundReferernce = false;
    for (std::list<ResultPtr>::iterator aSub = allRes.begin(); aSub != allRes.end(); aSub++) {
      ResultPtr aResCont = *aSub;
      ResultBodyPtr aResBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResCont);
      if (aResBody.get() && aResBody->numberOfSubs() > 0 && aResBody != aCompContext)
        continue; // only lower and higher level subs are counted
      const std::set<AttributePtr>& aRefs = aResCont->data()->refsToMe();
      std::set<AttributePtr>::const_iterator aRef = aRefs.begin();
      for (; !aFindNewContext && aRef != aRefs.end(); aRef++) {
        if (!aRef->get() || !(*aRef)->owner().get())
          continue;
        // concealed attribute only
        FeaturePtr aRefFeat = std::dynamic_pointer_cast<ModelAPI_Feature>((*aRef)->owner());
        if (!ModelAPI_Session::get()->validators()->isConcealed(
          aRefFeat->getKind(), (*aRef)->id()))
          continue;
        // search the feature result that contains sub-shape selected
        std::list<std::shared_ptr<ModelAPI_Result> > aResults;

        // take all sub-results or one result
        std::list<ResultPtr> aRefFeatResults;
        ModelAPI_Tools::allResults(aRefFeat, aRefFeatResults);
        std::list<ResultPtr>::iterator aRefResIter = aRefFeatResults.begin();
        for (; aRefResIter != aRefFeatResults.end(); aRefResIter++) {
          ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aRefResIter);
          if (aBody.get() && aBody->numberOfSubs() == 0) // add only lower level subs
            aResults.push_back(aBody);
        }
        std::list<std::shared_ptr<ModelAPI_Result> >::iterator aResIter = aResults.begin();

        // searching by sub-shape
        for (; aResIter != aResults.end(); aResIter++) {
          if (!aResIter->get() || !(*aResIter)->data()->isValid() || (*aResIter)->isDisabled())
            continue;
          aFoundReferernce = true;
          GeomShapePtr aShape = (*aResIter)->shape();
          if (aShape.get() && aShape->isSubShape(aSelectedShape, false)) {
            aResult = *aResIter; // found new context (produced from this) with same subshape
            aFindNewContext = true; // continue searching further
            break;
          }
        }
      }
    }
    if (aFoundReferernce && !aFindNewContext &&
        aResult->groupName() != ModelAPI_ResultConstruction::group()) {
      // #19019 : result is concealed by object that contains no such sub-shape
      return theCurrent;
    }
  }
  // if compsolid is context, try to take sub-solid as context: like in GUI and scripts
  ResultBodyPtr aComp = std::dynamic_pointer_cast<ModelAPI_ResultBody>(aResult);
  if (aComp && aComp->numberOfSubs()) {
    std::list<ResultPtr> allSubs;
    ModelAPI_Tools::allSubs(aComp, allSubs);
    std::list<ResultPtr>::iterator aS = allSubs.begin();
    for (; aS != allSubs.end(); aS++) {
      ResultBodyPtr aSub = std::dynamic_pointer_cast<ModelAPI_ResultBody>(*aS);
      if (aSub && aSub->numberOfSubs() == 0 && aSub->shape().get() &&
          aSub->shape()->isSubShape(aSelectedShape)) {
        aResult = aSub;
        break;
      }
    }
  }
  // in case sketch line was selected for wire, but wire was concealed and not such line anymore,
  // so, actually, the sketch element was selected (which is never concealed)
  if (aResult != theCurrent && theCurrent->groupName() == ModelAPI_ResultConstruction::group()) {
    //&& aResult->isConcealed())
    std::list<FeaturePtr> aConcealers;
    FeaturePtr aResFeature = aDoc->feature(aResult);
    FeaturePtr aThisFeature = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
    concealedFeature(aResFeature, aThisFeature, false, aConcealers, aResult, true);
    if (aConcealers.size())
      aResult = theCurrent;
  }
  return aResult;
}

void Model_AttributeSelection::combineGeometrical()
{
  if (myTmpContext.get() || myTmpSubShape.get())
    return;
  TDF_Label aSelLab = selectionLabel();
  if (aSelLab.IsAttribute(kINVALID_SELECTION) || !myRef.isInitialized())
    return;

  if (aSelLab.IsAttribute(kCIRCLE_CENTER) || aSelLab.IsAttribute(kELLIPSE_CENTER1) ||
      aSelLab.IsAttribute(kELLIPSE_CENTER2) || aSelLab.IsAttribute(kSIMPLE_REF_ID))
    return;

  if (aSelLab.IsAttribute(kPART_REF_ID)) {
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(context());
    if (!aPart.get() || !aPart->isActivated())
      return; // postponed naming needed
    Handle(TDataStd_Integer) anIndex;
    if (aSelLab.FindAttribute(TDataStd_Integer::GetID(), anIndex)) {
      if (anIndex->Get()) { // special selection attribute was created, use it
        std::string aNewName;
        aPart->combineGeometrical(anIndex->Get(), aNewName);
        TDataStd_Name::Set(aSelLab, aNewName.c_str());
      }
    }
    return;
  }

  std::shared_ptr<Model_ResultConstruction> aConstr =
    std::dynamic_pointer_cast<Model_ResultConstruction>(context());
  if (aConstr.get())
    return;
  FeaturePtr aFeature = contextFeature();
  if (aFeature.get())
    return;

  Selector_Selector aSelector(aSelLab, baseDocumentLab());
  TopoDS_Shape aContextShape = context()->shape()->impl<TopoDS_Shape>();
  if (aSelector.restore(aContextShape)) {
    aSelector.combineGeometrical(aContextShape);
  }
}

TDF_Label Model_AttributeSelection::baseDocumentLab()
{
  if (ModelAPI_Session::get()->moduleDocument() != owner()->document())
    return std::dynamic_pointer_cast<Model_Document>
      (ModelAPI_Session::get()->moduleDocument())->extConstructionsLabel();
  static TDF_Label anEmpty;
  return anEmpty;
}

void Model_AttributeSelection::reset()
{
  ModelAPI_AttributeSelection::reset();
  myRef.reset();
}
