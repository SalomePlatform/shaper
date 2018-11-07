// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
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
  if (theTemporarily &&
      (!theContext.get() || theContext->groupName() != ModelAPI_Feature::group())) {
    // just keep the stored without DF update
    myTmpContext = std::dynamic_pointer_cast<ModelAPI_Result>(theContext);
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
    // to keep the reference attribute label
    TDF_Label aRefLab = myRef.myRef->Label();
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
    if (aConstruction->isInfinite()) {
      // For correct naming selection, put the shape into the naming structure.
      // It seems sub-shapes are not needed: only this shape is (and can be) selected.
      /*
      TNaming_Builder aBuilder(aSelLab);
      aBuilder.Generated(aConstruction->shape()->impl<TopoDS_Shape>());
      std::string anInfinitiveName = contextName(aConstruction);
      TDataStd_Name::Set(aSelLab, anInfinitiveName.c_str());
      std::dynamic_pointer_cast<Model_Document>(owner()->document())
        ->addNamingName(aSelLab, anInfinitiveName.c_str());
        */
    }
  } else if (theContext->groupName() == ModelAPI_ResultPart::group()) {
    aSelLab.ForgetAllAttributes(true);
    TDataStd_UAttribute::Set(aSelLab, kPART_REF_ID);
    selectPart(std::dynamic_pointer_cast<ModelAPI_Result>(theContext), theSubShape);
  } else { // check the feature context: parent-Part of this feature should not be used
    FeaturePtr aFeatureContext = std::dynamic_pointer_cast<ModelAPI_Feature>(theContext);
    if (aFeatureContext.get()) {
      if (owner()->document() != aFeatureContext->document()) {
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

// returns the center of the edge: circular or elliptical
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
          Handle(Geom_Circle) aCirc = Handle(Geom_Circle)::DownCast(aCurve);
          if (!aCirc.IsNull()) {
            aBuilder.MakeVertex(aVertex, aCirc->Location(), Precision::Confusion());
          }
        } else { // ellipse
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
  if (!ModelAPI_AttributeSelection::isInitialized() && !myTmpContext.get() && !myTmpSubShape.get())
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
    return myTmpSubShape.get() ? myTmpSubShape : myTmpContext->shape();
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
              int anIndex;
              std::string aType; // to reuse already existing selection the type is not needed
              return aPart->shapeInPart(aNameInPart, aType, anIndex);
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
            return true;
        }
        // for the whole feature, a feature object
        FeaturePtr aFeat = contextFeature();
        if (aFeat.get())
          return true;
      }
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
    myIsGeometricalSelection = myParent->isGeometricalSelection();
  } else {
    myIsGeometricalSelection =
      ModelAPI_Session::get()->validators()->isGeometricalSelection(aFeature->getKind(), id());
  }
}

ResultPtr Model_AttributeSelection::context()
{
  if (!ModelAPI_AttributeSelection::isInitialized() && !myTmpContext.get() && !myTmpSubShape.get())
    return ResultPtr();

  if (myTmpContext.get() || myTmpSubShape.get()) {
    return myTmpContext;
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
  if (myTmpContext.get() || myTmpSubShape.get()) {
    return FeaturePtr(); // feature can not be selected temporarily
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
      GeomShapePtr aSubSh(new GeomAPI_Shape);
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

    Selector_Selector aSelector(aSelLab);
    if (ModelAPI_Session::get()->moduleDocument() != owner()->document()) {
      aSelector.setBaseDocument(std::dynamic_pointer_cast<Model_Document>
        (ModelAPI_Session::get()->moduleDocument())->extConstructionsLabel());
    }
    if (aSelector.restore()) { // it is stored in old OCCT format, use TNaming_Selector
      TopoDS_Shape aContextShape = aContext->shape()->impl<TopoDS_Shape>();
      aResult = aSelector.solve(aContextShape);
    }
    aResult = setInvalidIfFalse(aSelLab, aResult);

    TopoDS_Shape aNewShape;
    if (aSelLab.FindAttribute(TNaming_NamedShape::GetID(), aNS))
      aNewShape = aNS->Get();

    if (anOldShape.IsNull() || aNewShape.IsNull() || !anOldShape.IsEqual(aNewShape)) {
      // shape type should not be changed: if shape becomes compound of such shapes, then split
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
    bool aResult = true;
    std::shared_ptr<Model_ResultConstruction> aConstructionContext =
      std::dynamic_pointer_cast<Model_ResultConstruction>(aContext);
    if (!aConstructionContext->isInfinite()) {
      Selector_Selector aSelector(aSelLab);
      if (ModelAPI_Session::get()->moduleDocument() != owner()->document()) {
        aSelector.setBaseDocument(std::dynamic_pointer_cast<Model_Document>
          (ModelAPI_Session::get()->moduleDocument())->extConstructionsLabel());
      }

      aResult = aSelector.restore();
      TopoDS_Shape anOldShape = aSelector.value();
      if (aResult) {
        TopoDS_Shape aContextShape = aContext->shape()->impl<TopoDS_Shape>();
        aResult = aSelector.solve(aContextShape);
      }
      aResult = setInvalidIfFalse(aSelLab, aResult);
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
    Selector_Selector aSel(aSelLab);
    if (ModelAPI_Session::get()->moduleDocument() != owner()->document()) {
      aSel.setBaseDocument(std::dynamic_pointer_cast<Model_Document>
        (ModelAPI_Session::get()->moduleDocument())->extConstructionsLabel());
    }
    try {
      aSelectorOk = aSel.select(aContext, aNewSub, myIsGeometricalSelection);
      if (aSelectorOk) {
        aSel.store();
        aSelectorOk = aSel.solve(aContext);
      }
    } catch(...) {
      aSelectorOk = false;
    }
    Handle(TNaming_NamedShape) aSelectorShape;
    if (aSelectorOk && aSelLab.FindAttribute(TNaming_NamedShape::GetID(), aSelectorShape))
    {
      TopoDS_Shape aShape = aSelectorShape->Get();
      if (aShape.IsNull() || aShape.ShapeType() != aNewSub.ShapeType())
        aSelectorOk = false;
    }
    if (!aSelectorOk) {
      setInvalidIfFalse(aSelLab, false);
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
  std::string aName("");
  if(!this->isInitialized())
    return !theDefaultName.empty() ? theDefaultName : aName;

  CenterType aCenterType = NOT_CENTER;
  std::shared_ptr<GeomAPI_Shape> aSubSh = internalValue(aCenterType);
  ResultPtr aCont = context();

  if (!aCont.get() ||
      (aCont->groupName() == ModelAPI_ResultConstruction::group() && contextFeature().get())) {
    // selection of a full feature
    FeaturePtr aFeatureCont = contextFeature();
    if (aFeatureCont.get()) {
      return kWHOLE_FEATURE + aFeatureCont->name();
    }
    // in case of selection of removed result
    return "";
  }

  TDF_Label aSelLab = selectionLabel();
  if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // whole context, no value
    return contextName(aCont);
  }

  // if it is in result of another part
  if (aCont->groupName() == ModelAPI_ResultPart::group()) {
    ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aCont);
    int anIndex;
    GeomShapePtr aValue = value();
    if (aValue.get())
      return aPart->data()->name() + "/" + aPart->nameInPart(aValue, anIndex);
    else
      return aPart->data()->name();
  }


  // whole infinitive construction
  if (aCont->groupName() == ModelAPI_ResultConstruction::group()) {
    ResultConstructionPtr aConstr = std::dynamic_pointer_cast<Model_ResultConstruction>(aCont);
    if (aConstr->isInfinite()) {
      return contextName(aCont);
    }
  }

  Selector_Selector aSelector(aSelLab);
  if (ModelAPI_Session::get()->moduleDocument() != owner()->document()) {
    aSelector.setBaseDocument(std::dynamic_pointer_cast<Model_Document>
      (ModelAPI_Session::get()->moduleDocument())->extConstructionsLabel());
  }
  std::string aResult;
  if (aSelector.restore())
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
      } else {
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
            } else
              setValue(aPart, aSelected);
            TDataStd_Integer::Set(selectionLabel(), anIndex);
            return;
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
      if (!aRes.get())
        aRes = aDoc->objectByName(ModelAPI_ResultBody::group(), aSubShapeName);
      if (aRes.get()) {
        setValue(aRes, anEmptyShape);
        return;
      }
    }

    Selector_Selector aSelector(selectionLabel());
    if (ModelAPI_Session::get()->moduleDocument() != owner()->document()) {
      aSelector.setBaseDocument(std::dynamic_pointer_cast<Model_Document>
        (ModelAPI_Session::get()->moduleDocument())->extConstructionsLabel());
    }
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
          if (myIsGeometricalSelection) { // store the currently generated name
            selectionLabel().ForgetAllAttributes(true);
            bool aToUnblock = false;
            aToUnblock = !owner()->data()->blockSendAttributeUpdated(true);
            myRef.setValue(aContext);
            aSelector.store();
            owner()->data()->sendAttributeUpdated(this);
            if (aToUnblock)
              owner()->data()->blockSendAttributeUpdated(false);
            return;
          } else { // re-select by context and value
            if (aCenterType != NOT_CENTER) {
              if (!aShapeToBeSelected->isEdge())
                continue;
              std::shared_ptr<GeomAPI_Edge> aSelectedEdge(new GeomAPI_Edge(aShapeToBeSelected));
              setValueCenter(aContext, aSelectedEdge, aCenterType);
            }
            else
              setValue(aContext, aShapeToBeSelected);
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

int Model_AttributeSelection::Id()
{
  int anID = 0;
  std::shared_ptr<GeomAPI_Shape> aSelection = value();
  ResultPtr aContextRes = context();
  // support for compsolids:
  while(ModelAPI_Tools::bodyOwner(aContextRes).get()) {
    aContextRes = ModelAPI_Tools::bodyOwner(aContextRes);
  }
  std::shared_ptr<GeomAPI_Shape> aContext = aContextRes->shape();


  TopoDS_Shape aMainShape = aContext->impl<TopoDS_Shape>();
  const TopoDS_Shape& aSubShape = aSelection->impl<TopoDS_Shape>();
  // searching for the latest main shape
  if (aSelection && !aSelection->isNull() && aContext && !aContext->isNull())
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
  TopoDS_Shape aNewContShape = theNewContext->shape()->impl<TopoDS_Shape>();
  // if a new value is unchanged in the new context, do nothing: value is correct
  TopExp_Explorer aSubExp(aNewContShape, theValShape.ShapeType());
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
              theShapes.Append(aPairIter.NewShape());
            }
          } else if (!aPairIter.OldShape().IsNull()) { // search shape that contains this sub
            TopExp_Explorer anExp(aPairIter.OldShape(), theValShape.ShapeType());
            for(; anExp.More(); anExp.Next()) {
              if (anExp.Current().IsSame(theValShape)) { // found a new container
                if (aPairIter.NewShape().IsNull()) {// value was removed
                  theShapes.Clear();
                  return;
                }
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
    // map of sub-shapes -> number of occurrences of these shapes in containers
    NCollection_DataMap<TopoDS_Shape, TopTools_MapOfShape, TopTools_ShapeMapHasher> aSubs;
    TopTools_DataMapOfShapeShape::Iterator aContIter(aNewToOld);
    for(; aContIter.More(); aContIter.Next()) {
      TopExp_Explorer aSubExp(aContIter.Key(), theValShape.ShapeType());
      for(; aSubExp.More(); aSubExp.Next()) {
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
        theShapes.Append(aSubsIter.Key());
      }
    }
  }
  if (theShapes.IsEmpty()) { // nothing was changed
    theShapes.Append(aWasWholeContext ? TopoDS_Shape() : theValShape);
  }
}

bool Model_AttributeSelection::searchNewContext(std::shared_ptr<Model_Document> theDoc,
  const TopoDS_Shape theContShape, ResultPtr theContext, TopoDS_Shape theValShape,
  TDF_Label theAccessLabel,
  std::list<ResultPtr>& theResults, TopTools_ListOfShape& theValShapes)
{
  std::set<ResultPtr> aResults; // to avoid duplicates, new context, null if deleted
  TopTools_ListOfShape aResContShapes;
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
        aResults.insert(aModifierObj);
        aResContShapes.Append(aModifierObj->shape()->impl<TopoDS_Shape>());
      } else if (aNewNS->Evolution() == TNaming_DELETE) { // a shape was deleted => result is empty
        aResults.insert(ResultPtr());
      } else { // not-processed modification => don't support it
        continue;
      }
    }
  }
  if (aResults.empty())
    return false; // no modifications found, must stay the same
  // iterate all results to find further modifications
  std::set<ResultPtr>::iterator aResIter = aResults.begin();
  for(; aResIter != aResults.end(); aResIter++) {
    if (aResIter->get() != NULL) {
      // compute new values by two contexts: the old and the new
      TopTools_ListOfShape aValShapes;
      computeValues(theContext, *aResIter, theValShape, aValShapes);

      TopTools_ListIteratorOfListOfShape aNewVal(aValShapes);
      for(; aNewVal.More(); aNewVal.Next()) {
        std::list<ResultPtr> aNewRes;
        TopTools_ListOfShape aNewUpdatedVal;
        TopoDS_Shape aNewValSh = aNewVal.Value();
        TopoDS_Shape aNewContShape = (*aResIter)->shape()->impl<TopoDS_Shape>();
        if (theValShape.IsNull() && aNewContShape.IsSame(aNewValSh))
          aNewValSh.Nullify();
        if (searchNewContext(theDoc, aNewContShape, *aResIter, aNewValSh,
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
          theResults.push_back(*aResIter);
          theValShapes.Append(aNewValSh);
        }
      }
    }
  }
  return true; // theResults must be empty: everything is deleted
}

void Model_AttributeSelection::updateInHistory()
{
  ResultPtr aContext = std::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
  // only bodies and parts may be modified later in the history, don't do anything otherwise
  if (!aContext.get() || (aContext->groupName() != ModelAPI_ResultBody::group() &&
      aContext->groupName() != ModelAPI_ResultPart::group()))
    return;
  std::shared_ptr<Model_Document> aDoc =
    std::dynamic_pointer_cast<Model_Document>(aContext->document());
  std::shared_ptr<Model_Data> aContData = std::dynamic_pointer_cast<Model_Data>(aContext->data());
  if (!aContData.get() || !aContData->isValid())
    return;
  TDF_Label aContLab = aContData->shapeLab(); // named shape where the selected context is located
  Handle(TNaming_NamedShape) aContNS;
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
            if (aRefFeat.get() && aRefFeat != owner()) {
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
  bool anIterate = true;
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
    GeomAPI_Shape::ShapeType aListShapeType = GeomAPI_Shape::SHAPE;
    if (myParent) {
      if (myParent->selectionType() == "VERTEX") aListShapeType = GeomAPI_Shape::VERTEX;
      else if (myParent->selectionType() == "EDGE") aListShapeType = GeomAPI_Shape::EDGE;
      else if (myParent->selectionType() == "FACE") aListShapeType = GeomAPI_Shape::FACE;
    }

    std::list<ResultPtr>::iterator aNewCont = aNewContexts.begin();
    TopTools_ListIteratorOfListOfShape aNewValues(aValShapes);
    bool aFirst = true; // first is set to this, next are appended to parent
    for(; aNewCont != aNewContexts.end(); aNewCont++, aNewValues.Next()) {

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
        continue;
      }
      if (aFirst) {
        setValue(*aNewCont, aValueShape);
        aFirst = false;
      } else if (myParent) {
        myParent->append(*aNewCont, aValueShape);
      }
    }
    if (aFirst) { // nothing was added, all results were deleted
      ResultPtr anEmptyContext;
      std::shared_ptr<GeomAPI_Shape> anEmptyShape;
      setValue(anEmptyContext, anEmptyShape); // nullify the selection
      return;
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
      std::string::size_type n = aName.find('/');
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

TDF_Label Model_AttributeSelection::newestContext(const TDF_Label theCurrentContext) {
  std::shared_ptr<Model_Document> aDoc = myRestoreDocument.get() ? myRestoreDocument :
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  ResultPtr aContext = aDoc->resultByLab(theCurrentContext);
  if (aContext.get()) {
    aContext = newestContext(aContext, GeomShapePtr(), true);
    if (aContext.get())
      return std::dynamic_pointer_cast<Model_Data>(aContext->data())->label();
  }
  return theCurrentContext; // nothing is changed
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
  const ResultPtr theCurrent, const GeomShapePtr theValue, const bool theAnyValue)
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
            if (theAnyValue || aRes->shape()->isSubShape(aSelectedShape)) {
              aResult = aRes;
              aFindNewContext = true;
              continue;
            }
          }
        }
      }
    }
    if (theAnyValue) { // only for neighbors for now
      // try to find modification of sub-shapes: the best number of matches
      std::map<ResultPtr, int> aMatches; // result -> number of matches of shapes to find the best
      TDF_Label aResLab = std::dynamic_pointer_cast<Model_Data>(aResult->data())->shapeLab();
      TDF_ChildIDIterator aModifIter(aResLab, TNaming_NamedShape::GetID());
      for(; aModifIter.More(); aModifIter.Next()) {
        Handle(TNaming_NamedShape) aNS = Handle(TNaming_NamedShape)::DownCast(aModifIter.Value());
        if (aNS->Evolution() == TNaming_MODIFY || aNS->Evolution() == TNaming_GENERATED) {
          for(TNaming_Iterator aNSIter(aNS); aNSIter.More(); aNSIter.Next()) {
            TNaming_NewShapeIterator aNewIter(aNSIter.NewShape(), aNS->Label());
            for(; aNewIter.More(); aNewIter.Next()) {
              TDF_Label aLab = aNewIter.Label();
              if (isLater(aLab, aNS->Label()) && isLater(selectionLabel(), aLab)) {
                ResultPtr aRes = aDoc->resultByLab(aLab);
                if (aRes.get()) {
                  if (aMatches.find(aRes) == aMatches.end())
                    aMatches[aRes] = 0;
                  aMatches[aRes]++; // found result, add matches
                }
              }
            }
          }
        }
      }
      // searching for the best result-candidate
      int aBest = 0;
      ResultPtr aBestResult;
      std::map<ResultPtr, int>::iterator aMatchIter = aMatches.begin();
      for(; aMatchIter != aMatches.end(); aMatchIter++) {
        if (aMatchIter->second > aBest) {
          aBest = aMatchIter->second;
          aBestResult = aMatchIter->first;
        }
      }
      if (aBestResult.get()) {
        aResult = aBestResult;
        aFindNewContext = true;
        continue;
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
        for (; aResIter != aResults.end(); aResIter++) {
          if (!aResIter->get() || !(*aResIter)->data()->isValid() || (*aResIter)->isDisabled())
            continue;
          GeomShapePtr aShape = (*aResIter)->shape();
          if (aShape.get() && (theAnyValue || aShape->isSubShape(aSelectedShape, false))) {
            aResult = *aResIter; // found new context (produced from this) with same subshape
            aFindNewContext = true; // continue searching further
            break;
          }
        }
      }
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
        (theAnyValue || aSub->shape()->isSubShape(aSelectedShape))) {
        aResult = aSub;
        break;
      }
    }
  }
  // in case sketch line was selected for wire, but wire was concealed and not such line anymore,
  // so, actually, the sketch element was selected (which is never concealed)
  if (aResult != theCurrent && aResult->isConcealed())
    aResult = theCurrent;
  return aResult;
}
