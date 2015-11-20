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
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_ResultPart.h>
#include <ModelAPI_CompositeFeature.h>
#include <ModelAPI_Tools.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_PlanarEdges.h>
#include <Events_Error.h>

#include <TNaming_Selector.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TNaming_Localizer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDataStd_Integer.hxx>
#include <TDataStd_UAttribute.hxx>
#include <TDataStd_Name.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopTools_IndexedMapOfShape.hxx>
#include <TopTools_MapIteratorOfMapOfShape.hxx>
#include <TopTools_ListOfShape.hxx>
#include <NCollection_DataMap.hxx>
#include <TopExp_Explorer.hxx>
#include <TDF_LabelMap.hxx>
#include <BRep_Tool.hxx>
#include <BRep_Builder.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TopExp.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <TopTools_IndexedDataMapOfShapeListOfShape.hxx>
#include <TopTools_ListIteratorOfListOfShape.hxx>
#include <TColStd_MapIteratorOfPackedMapOfInteger.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDataStd_Name.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Iterator.hxx>
#include <TNaming_Iterator.hxx>
#include <BRep_Builder.hxx>
using namespace std;
//#define DEB_NAMING 1
#ifdef DEB_NAMING
#include <BRepTools.hxx>
#endif
/// adeed to the index in the packed map to signalize that the vertex of edge is seleted
/// (multiplied by the index of the edge)
static const int kSTART_VERTEX_DELTA = 1000000;
// identifier that there is simple reference: selection equals to context
Standard_GUID kSIMPLE_REF_ID("635eacb2-a1d6-4dec-8348-471fae17cb29");
// simple reference in the construction
Standard_GUID kCONSTUCTION_SIMPLE_REF_ID("635eacb2-a1d6-4dec-8348-471fae17cb28");
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
    ResultCompSolidPtr anOwner = ModelAPI_Tools::compSolidOwner(theContext);
    if (anOwner.get())
      myRef.setValue(anOwner);
    else
      myRef.setValue(theContext);
  }

  // do noth use naming if selected shape is result shape itself, but not sub-shape
  TDF_Label aSelLab = selectionLabel();
  aSelLab.ForgetAttribute(kSIMPLE_REF_ID);
  aSelLab.ForgetAttribute(kCONSTUCTION_SIMPLE_REF_ID);
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
    if (!theSubShape.get()) {
      // to sub, so the whole result is selected
      aSelLab.ForgetAllAttributes(true);
      TDataStd_UAttribute::Set(aSelLab, kCONSTUCTION_SIMPLE_REF_ID);
      ResultConstructionPtr aConstruction = 
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(theContext);
      if (aConstruction->isInfinite()) {
        // For correct naming selection, put the shape into the naming structure.
        // It seems sub-shapes are not needed: only this shape is (and can be ) selected.
        TNaming_Builder aBuilder(aSelLab);
        aBuilder.Generated(theContext->shape()->impl<TopoDS_Shape>());
        std::shared_ptr<Model_Document> aMyDoc = 
          std::dynamic_pointer_cast<Model_Document>(owner()->document());
        std::string aName = theContext->data()->name();
        aMyDoc->addNamingName(aSelLab, aName);
        TDataStd_Name::Set(aSelLab, aName.c_str());
      } else {  // for sketch the naming is needed in DS
        BRep_Builder aCompoundBuilder;
        TopoDS_Compound aComp;
        aCompoundBuilder.MakeCompound(aComp);
        for(int a = 0; a < aConstruction->facesNum(); a++) {
          TopoDS_Shape aFace = aConstruction->face(a)->impl<TopoDS_Shape>();
          aCompoundBuilder.Add(aComp, aFace);
        }
        std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape);
        aShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aComp));
        selectConstruction(theContext, aShape);
      }
    } else {
      selectConstruction(theContext, theSubShape);
    }
  } else if (theContext->groupName() == ModelAPI_ResultPart::group()) {
    aSelLab.ForgetAllAttributes(true);
    TDataStd_UAttribute::Set(aSelLab, kPART_REF_ID);
    selectPart(theContext, theSubShape);
  }
  //the attribute initialized state should be changed by sendAttributeUpdated only
  //myIsInitialized = true;

  owner()->data()->sendAttributeUpdated(this);

  std::string aSelName = namingName();
  if(!aSelName.empty())
    TDataStd_Name::Set(selectionLabel(), aSelName.c_str()); //set name
}

std::shared_ptr<GeomAPI_Shape> Model_AttributeSelection::value()
{
  if (myTmpContext.get() || myTmpSubShape.get()) {
    return myTmpSubShape;
  }

  std::shared_ptr<GeomAPI_Shape> aResult;
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
    if (aSelLab.IsAttribute(kCONSTUCTION_SIMPLE_REF_ID)) { // it is just reference to construction, nothing is in value
        return aResult; // empty result
    }
    if (aSelLab.IsAttribute(kPART_REF_ID)) {
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(context());
      if (!aPart.get() || !aPart->isActivated())
        return std::shared_ptr<GeomAPI_Shape>(); // postponed naming needed
      Handle(TDataStd_Integer) anIndex;
      if (selectionLabel().FindAttribute(TDataStd_Integer::GetID(), anIndex)) {
        if (anIndex->Get()) { // special selection attribute was created, use it
          return aPart->selectionValue(anIndex->Get());
        } else { // face with name is already in the data model, so try to take it by name
          Handle(TDataStd_Name) aName;
          if (selectionLabel().FindAttribute(TDataStd_Name::GetID(), aName)) {
            std::string aSubShapeName(TCollection_AsciiString(aName->Get()).ToCString());
            std::size_t aPartEnd = aSubShapeName.find('/');
            if (aPartEnd != string::npos && aPartEnd != aSubShapeName.rfind('/')) {
              string aNameInPart = aSubShapeName.substr(aPartEnd + 1);
              int anIndex;
              std::string aType; // to reuse already existing selection the type is not needed
              return aPart->shapeInPart(aNameInPart, aType, anIndex);
            }
          }
        }
      }
    }

    Handle(TNaming_NamedShape) aSelection;
    if (selectionLabel().FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
      TopoDS_Shape aSelShape = aSelection->Get();
      aResult = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape);
      aResult->setImpl(new TopoDS_Shape(aSelShape));
    } else { // for simple construction element: just shape of this construction element
      ResultConstructionPtr aConstr = 
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(context());
      if (aConstr) {
        return aConstr->shape();
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
  if (ModelAPI_AttributeSelection::isInitialized()) { // additional checkings if it is initialized
    std::shared_ptr<GeomAPI_Shape> aResult;
    if (myRef.isInitialized()) {
      TDF_Label aSelLab = selectionLabel();
      if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // it is just reference to shape, not sub-shape
        ResultPtr aContext = context();
        return aContext.get() != NULL;
      }
      if (aSelLab.IsAttribute(kCONSTUCTION_SIMPLE_REF_ID)) { // it is just reference to construction, nothing is in value
          return true;
      }

      Handle(TNaming_NamedShape) aSelection;
      if (selectionLabel().FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
        return !aSelection->Get().IsNull();
      } else { // for simple construction element: just shape of this construction element
        ResultConstructionPtr aConstr = 
          std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(context());
        if (aConstr.get()) {
          return aConstr->shape().get() != NULL;
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
    // gets all featueres with named shapes that are bofore this feature label (before in history)
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
      if (!isInScope && aComposite.get()) { // try to add sub-elements of composite if this is composite
        if (aComposite->isSub(*aFIter))
          isInScope = true;
      }
      // remove the composite-owner of this feature (sketch in extrusion-cut)
      if (isInScope && (aCompositeOwner == *aFIter || aCompositeOwnerOwner == *aFIter))
        isInScope = false;

      if (isInScope && aFIter->get() && (*aFIter)->data()->isValid()) {
        TDF_Label aFeatureLab = std::dynamic_pointer_cast<Model_Data>(
          (*aFIter)->data())->label().Father();
        TDF_ChildIDIterator aNSIter(aFeatureLab, TNaming_NamedShape::GetID(), 1);
        for(; aNSIter.More(); aNSIter.Next()) {
          Handle(TNaming_NamedShape) aNS = Handle(TNaming_NamedShape)::DownCast(aNSIter.Value());
          if (!aNS.IsNull() && aNS->Evolution() != TNaming_SELECTED) {
            myScope.Add(aNS->Label());
          }
        }
      }
    }
  }
  return myScope;
}

/// produces theEdge orientation relatively to theContext face
int edgeOrientation(const TopoDS_Shape& theContext, TopoDS_Edge& theEdge)
{
  if (theContext.ShapeType() != TopAbs_FACE)
    return 0;
  TopoDS_Face aContext = TopoDS::Face(theContext);
  if (theEdge.Orientation() == TopAbs_FORWARD) 
    return 1;
  if (theEdge.Orientation() == TopAbs_REVERSED) 
    return -1;
  return 0; // unknown
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

bool Model_AttributeSelection::update()
{
  TDF_Label aSelLab = selectionLabel();
  ResultPtr aContext = context();
  if (!aContext.get()) 
    return setInvalidIfFalse(aSelLab, false);
  if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // it is just reference to shape, not sub-shape
    return setInvalidIfFalse(aSelLab, aContext->shape() && !aContext->shape()->isNull());
  }
  if (aSelLab.IsAttribute(kCONSTUCTION_SIMPLE_REF_ID)) { // it is just reference to construction, not sub-shape
    // if there is a sketch, the sketch-naming must be updated
    ResultConstructionPtr aConstruction = 
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    if (!aConstruction->isInfinite()) {
      BRep_Builder aCompoundBuilder;
      TopoDS_Compound aComp;
      aCompoundBuilder.MakeCompound(aComp);
      for(int a = 0; a < aConstruction->facesNum(); a++) {
        TopoDS_Shape aFace = aConstruction->face(a)->impl<TopoDS_Shape>();
        aCompoundBuilder.Add(aComp, aFace);
      }
      std::shared_ptr<GeomAPI_Shape> aShape(new GeomAPI_Shape);
      aShape->setImpl<TopoDS_Shape>(new TopoDS_Shape(aComp));
      selectConstruction(aContext, aShape);
    }
    return setInvalidIfFalse(aSelLab, aContext->shape() && !aContext->shape()->isNull());
  }

  if (aSelLab.IsAttribute(kPART_REF_ID)) { // it is reference to the part object
    std::shared_ptr<GeomAPI_Shape> aNoSelection;
    return setInvalidIfFalse(aSelLab, selectPart(aContext, aNoSelection, true));
  }

  if (aContext->groupName() == ModelAPI_ResultBody::group()) {
    // body: just a named shape, use selection mechanism from OCCT
    TNaming_Selector aSelector(aSelLab);
    bool aResult = aSelector.Solve(scope()) == Standard_True;
    owner()->data()->sendAttributeUpdated(this);
    return setInvalidIfFalse(aSelLab, aResult);
  } else if (aContext->groupName() == ModelAPI_ResultConstruction::group()) {
    // construction: identification by the results indexes, recompute faces and
    // take the face that more close by the indexes
    ResultConstructionPtr aConstructionContext = 
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext);
    FeaturePtr aContextFeature = aContext->document()->feature(aContext);
    // sketch sub-element
    if (aConstructionContext && 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aContextFeature).get())
    {
      TDF_Label aLab = myRef.myRef->Label();
      // getting a type of selected shape
      Handle(TDataStd_Integer) aTypeAttr;
      if (!aLab.FindAttribute(TDataStd_Integer::GetID(), aTypeAttr)) {
        return setInvalidIfFalse(aSelLab, false);
      }
      TopAbs_ShapeEnum aShapeType = (TopAbs_ShapeEnum)(aTypeAttr->Get());
      // selected indexes will be needed in each "if"
      Handle(TDataStd_IntPackedMap) aSubIds;
      std::shared_ptr<GeomAPI_Shape> aNewSelected;
      bool aNoIndexes = 
        !aLab.FindAttribute(TDataStd_IntPackedMap::GetID(), aSubIds) || aSubIds->Extent() == 0;
      // for now working only with composite features
      CompositeFeaturePtr aComposite = 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aContextFeature);
      if (!aComposite.get() || aComposite->numberOfSubs() == 0) {
        return setInvalidIfFalse(aSelLab, false);
      }

      if (aShapeType == TopAbs_FACE) { // compound is for the whole sketch selection
        // If this is a wire with plane defined thin it is a sketch-like object
        if (!aConstructionContext->facesNum()) // no faces, update can not work correctly
          return setInvalidIfFalse(aSelLab, false);
        // if there is no edges indexes, any face can be used: take the first
        std::shared_ptr<GeomAPI_Shape> aNewSelected;
        if (aNoIndexes) {
          aNewSelected = aConstructionContext->face(0);
        } else { // searching for most looks-like initial face by the indexes
          // prepare edges of the current resut for the fast searching
          NCollection_DataMap<Handle(Geom_Curve), int> allCurves; // curves and orientations of edges
          const int aSubNum = aComposite->numberOfSubs();
          for(int a = 0; a < aSubNum; a++) {
            int aSubID = aComposite->subFeatureId(a);
            if (aSubIds->Contains(aSubID)) {
              FeaturePtr aSub = aComposite->subFeature(a);
              const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
              std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes;
              for(aRes = aResults.cbegin(); aRes != aResults.cend(); aRes++) {
                ResultConstructionPtr aConstr = 
                  std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
                if (aConstr->shape() && aConstr->shape()->isEdge()) {
                  const TopoDS_Shape& aResShape = aConstr->shape()->impl<TopoDS_Shape>();
                  TopoDS_Edge anEdge = TopoDS::Edge(aResShape);
                  if (!anEdge.IsNull()) {
                    Standard_Real aFirst, aLast;
                    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                    // searching for orientation information
                    int anOrient = 0;
                    Handle(TDataStd_Integer) anInt;
                    if (aSelLab.FindChild(aSubID).FindAttribute(TDataStd_Integer::GetID(), anInt)){
                      anOrient = anInt->Get();
                    }
                    allCurves.Bind(aCurve, anOrient);
                  }
                }
              }
            }
          }
          int aBestFound = 0; // best number of found edges (not percentage: issue 1019)
          int aBestOrient = 0; // for the equal "BestFound" additional parameter is orientation
          for(int aFaceIndex = 0; aFaceIndex < aConstructionContext->facesNum(); aFaceIndex++) {
            int aFound = 0, aNotFound = 0, aSameOrientation = 0;
            TopoDS_Face aFace = 
              TopoDS::Face(aConstructionContext->face(aFaceIndex)->impl<TopoDS_Shape>());
            TopExp_Explorer anEdgesExp(aFace, TopAbs_EDGE);
            TColStd_MapOfTransient alreadyProcessed; // to avoid counting edges with same curved (841)
            for(; anEdgesExp.More(); anEdgesExp.Next()) {
              TopoDS_Edge anEdge = TopoDS::Edge(anEdgesExp.Current());
              if (!anEdge.IsNull()) {
                Standard_Real aFirst, aLast;
                Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                if (alreadyProcessed.Contains(aCurve))
                  continue;
                alreadyProcessed.Add(aCurve);
                if (allCurves.IsBound(aCurve)) {
                  aFound++;
                  int anOrient = allCurves.Find(aCurve);
                  if (anOrient != 0) {  // extra comparision score is orientation
                    if (edgeOrientation(aFace, anEdge) == anOrient)
                      aSameOrientation++;
                  }
                } else {
                  aNotFound++;
                }
              }
            }
            if (aFound + aNotFound != 0) {
              if (aFound > aBestFound || 
                  (aFound == aBestFound && aSameOrientation > aBestOrient)) {
                aBestFound = aFound;
                aBestOrient = aSameOrientation;
                aNewSelected = aConstructionContext->face(aFaceIndex);
              }
            }
          }
        }
        if (aNewSelected) { // store this new selection
          selectConstruction(aContext, aNewSelected);
          owner()->data()->sendAttributeUpdated(this);
          return setInvalidIfFalse(aSelLab, true);
        } else { // if the selection is not found, put the empty shape: it's better to have disappeared shape, than the old, the lost one
          TNaming_Builder anEmptyBuilder(selectionLabel());
          return setInvalidIfFalse(aSelLab, false);
        }
      } else if (aShapeType == TopAbs_EDGE) {
        // just reselect the edge by the id
        const int aSubNum = aComposite->numberOfSubs();
        for(int a = 0; a < aSubNum; a++) {
          // if aSubIds take any, the first appropriate
          if (aSubIds->IsEmpty() || aSubIds->Contains(aComposite->subFeatureId(a))) {
            // found the appropriate feature
            FeaturePtr aFeature = aComposite->subFeature(a);
            std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter =
              aFeature->results().cbegin();
            for(;aResIter != aFeature->results().cend(); aResIter++) {
              ResultConstructionPtr aRes = 
                std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aResIter);
              if (aRes && aRes->shape() && aRes->shape()->isEdge()) { // found!
                selectConstruction(aContext, aRes->shape());
                owner()->data()->sendAttributeUpdated(this);
                return setInvalidIfFalse(aSelLab, true);
              }
            }
          }
        }
      } else if (aShapeType == TopAbs_VERTEX) {
        // just reselect the vertex by the id of edge
        const int aSubNum = aComposite->numberOfSubs();
        for(int a = 0; a < aSubNum; a++) {
          // if aSubIds take any, the first appropriate
          int aFeatureID = aComposite->subFeatureId(a);
          if (aSubIds->IsEmpty() || aSubIds->Contains(aFeatureID) ||
            aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA) ||
            aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA * 2)) {
              // searching for deltas
              int aVertexNum = 0;
              if (aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA)) aVertexNum = 1;
              else if (aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA * 2)) aVertexNum = 2;
              // found the feature with appropriate edge
              FeaturePtr aFeature = aComposite->subFeature(a);
              std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aResIter =
                aFeature->results().cbegin();
              for(;aResIter != aFeature->results().cend(); aResIter++) {
                ResultConstructionPtr aRes = 
                  std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aResIter);
                if (aRes && aRes->shape()) {
                  if (aRes->shape()->isVertex() && aVertexNum == 0) { // found!
                    selectConstruction(aContext, aRes->shape());
                    owner()->data()->sendAttributeUpdated(this);
                    return setInvalidIfFalse(aSelLab, true);
                  } else if (aRes->shape()->isEdge() && aVertexNum > 0) {
                    const TopoDS_Shape& anEdge = aRes->shape()->impl<TopoDS_Shape>();
                    int aVIndex = 1;
                    for(TopExp_Explorer aVExp(anEdge, TopAbs_VERTEX); aVExp.More(); aVExp.Next()) {
                      if (aVIndex == aVertexNum) { // found!
                        std::shared_ptr<GeomAPI_Shape> aVertex(new GeomAPI_Shape);
                        aVertex->setImpl(new TopoDS_Shape(aVExp.Current()));
                        selectConstruction(aContext, aVertex);
                        owner()->data()->sendAttributeUpdated(this);
                        return setInvalidIfFalse(aSelLab, true);
                      }
                      aVIndex++;
                    }
                  }
                }
              }
          }
        }
      }
    } else { // simple construction element: the selected is that needed
      selectConstruction(aContext, aContext->shape());
      owner()->data()->sendAttributeUpdated(this);
      return setInvalidIfFalse(aSelLab, true);
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

  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(myRef.value());
  if (aBody) {
    aContext = aBody->shape()->impl<TopoDS_Shape>();
  } else {
    ResultPtr aResult = 
      std::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
    if (aResult) {
      aContext = aResult->shape()->impl<TopoDS_Shape>();
    } else {
      Events_Error::send("A result with shape is expected");
      return;
    }
  }
  TopoDS_Shape aNewShape = theSubShape ? theSubShape->impl<TopoDS_Shape>() : aContext;
  /// fix for issue 411: result modified shapes must not participate in this selection mechanism
  FeaturePtr aFeatureOwner = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
  if (aFeatureOwner.get())
    aFeatureOwner->eraseResults();
  if (!aContext.IsNull()) {
    aSel.Select(aNewShape, aContext); 
  }
}

/// registers the name of the shape in the label (theID == 0) of sub label (theID is a tag)
/// if theID is zero, 
/// theOrientation is additional information about the positioning of edge relatively to face
///    it is stored in the integer attribute of the edge sub-label: 
///    -1 is out, 1 is in, 0 is not needed
static void registerSubShape(TDF_Label theMainLabel, TopoDS_Shape theShape,
  const int theID, const FeaturePtr& theContextFeature, std::shared_ptr<Model_Document> theDoc,
  std::string theAdditionalName, std::map<int, int>& theOrientations,
  Handle(TDataStd_IntPackedMap) theRefs = Handle(TDataStd_IntPackedMap)(),
  const int theOrientation = 0)
{
  TDF_Label aLab = theID == 0 ? theMainLabel : theMainLabel.FindChild(theID);
  if (theOrientation != 0) { // store the orientation of edge relatively to face if needed
    TDataStd_Integer::Set(aLab, theOrientation);
  }
  TNaming_Builder aBuilder(aLab);
  aBuilder.Generated(theShape);
  std::stringstream aName;
  aName<<theContextFeature->name()<<"/";
  if (!theAdditionalName.empty())
    aName<<theAdditionalName<<"/";
  if (theShape.ShapeType() == TopAbs_FACE) aName<<"Face";
  else if (theShape.ShapeType() == TopAbs_EDGE) aName<<"Edge";
  else if (theShape.ShapeType() == TopAbs_VERTEX) aName<<"Vertex";

  if (theRefs.IsNull()) {
    aName<<theID;
    if (theOrientation == 1)
      aName<<"f";
    else if (theOrientation == -1)
      aName<<"r";
  } else { // make a compisite name from all sub-elements indexes: "1_2_3_4"
    TColStd_MapIteratorOfPackedMapOfInteger aRef(theRefs->GetMap());
    for(; aRef.More(); aRef.Next()) {
      aName<<"-"<<aRef.Key();
      if (theOrientations.find(aRef.Key()) != theOrientations.end()) {
        if (theOrientations[aRef.Key()] == 1)
          aName<<"f";
        else if (theOrientations[aRef.Key()] == -1)
          aName<<"r";
      }
    }
  }

  theDoc->addNamingName(aLab, aName.str());
  TDataStd_Name::Set(aLab, aName.str().c_str());
}

void Model_AttributeSelection::selectConstruction(
  const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  std::shared_ptr<Model_Document> aMyDoc = 
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  FeaturePtr aContextFeature = theContext->document()->feature(theContext);
  CompositeFeaturePtr aComposite = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aContextFeature);
  const TopoDS_Shape& aSubShape = theSubShape->impl<TopoDS_Shape>();
  if (!aComposite || aComposite->numberOfSubs() == 0) {
    // saving of context is enough: result construction contains exactly the needed shape
    TNaming_Builder aBuilder(selectionLabel());
    aBuilder.Generated(aSubShape);
    aMyDoc->addNamingName(selectionLabel(), theContext->data()->name());
    TDataStd_Name::Set(selectionLabel(), theContext->data()->name().c_str());
    return;
  }
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(owner()->data());
  TDF_Label aLab = myRef.myRef->Label();
  // identify the reuslts of sub-object of the composite by edges
  // save type of the selected shape in integer attribute
  TopAbs_ShapeEnum aShapeType = aSubShape.ShapeType();
  TDataStd_Integer::Set(aLab, (int)aShapeType);
  gp_Pnt aVertexPos;
  TColStd_MapOfTransient allCurves;
  if (aShapeType == TopAbs_VERTEX) { // compare positions
    aVertexPos = BRep_Tool::Pnt(TopoDS::Vertex(aSubShape));
  } else { 
    for(TopExp_Explorer anEdgeExp(aSubShape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next()) {
      TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
      Standard_Real aFirst, aLast;
      Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
      allCurves.Add(aCurve);
    }
  }
  // iterate and store the result ids of sub-elements and sub-elements to sub-labels
  Handle(TDataStd_IntPackedMap) aRefs = TDataStd_IntPackedMap::Set(aLab);
  std::map<int, int> anOrientations; //map from edges IDs to orientations of these edges in face
  aRefs->Clear();
  const int aSubNum = aComposite->numberOfSubs();
  for(int a = 0; a < aSubNum; a++) {
    FeaturePtr aSub = aComposite->subFeature(a);
    const std::list<std::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
    std::list<std::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
    // there may be many shapes (circle and center): register if at least one is in selection
    for(; aRes != aResults.cend(); aRes++) {
      ResultConstructionPtr aConstr = 
        std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
      if (!aConstr->shape()) {
        continue;
      }
      if (aShapeType == TopAbs_VERTEX) {
        if (aConstr->shape()->isVertex()) { // compare vertices positions
          const TopoDS_Shape& aVertex = aConstr->shape()->impl<TopoDS_Shape>();
          gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aVertex));
          if (aPnt.IsEqual(aVertexPos, Precision::Confusion())) {
            aRefs->Add(aComposite->subFeatureId(a));
          }
        } else { // get first or last vertex of the edge: last is stored with negative sign
          const TopoDS_Shape& anEdge = aConstr->shape()->impl<TopoDS_Shape>();
          int aDelta = kSTART_VERTEX_DELTA;
          for(TopExp_Explorer aVExp(anEdge, TopAbs_VERTEX); aVExp.More(); aVExp.Next()) {
            gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(aVExp.Current()));
            if (aPnt.IsEqual(aVertexPos, Precision::Confusion())) {
              aRefs->Add(aDelta + aComposite->subFeatureId(a));
              break;
            }
            aDelta += kSTART_VERTEX_DELTA;
          }
        }
      } else {
        if (aConstr->shape()->isEdge()) {
          const TopoDS_Shape& aResShape = aConstr->shape()->impl<TopoDS_Shape>();
          TopoDS_Edge anEdge = TopoDS::Edge(aResShape);
          if (!anEdge.IsNull()) {
            Standard_Real aFirst, aLast;
            Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
            if (allCurves.Contains(aCurve)) {
              int anID = aComposite->subFeatureId(a);
              aRefs->Add(anID);
              if (aShapeType != TopAbs_EDGE) { // face needs the sub-edges on sub-labels
                // add edges to sub-label to support naming for edges selection
                TopExp_Explorer anEdgeExp(aSubShape, TopAbs_EDGE);
                for(; anEdgeExp.More(); anEdgeExp.Next()) {
                  TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
                  Standard_Real aFirst, aLast;
                  Handle(Geom_Curve) aFaceCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                  if (aFaceCurve == aCurve) {
                    int anOrient = edgeOrientation(aSubShape, anEdge);
                    anOrientations[anID] = anOrient;
                    registerSubShape(
                      selectionLabel(), anEdge, anID, aContextFeature, aMyDoc, "", anOrientations,
                      Handle(TDataStd_IntPackedMap)(), anOrient);
                  }
                }
              } else { // put vertices of the selected edge to sub-labels
                // add edges to sub-label to support naming for edges selection
                TopExp_Explorer anEdgeExp(aSubShape, TopAbs_VERTEX);
                int aTagIndex = anID + kSTART_VERTEX_DELTA;
                for(; anEdgeExp.More(); anEdgeExp.Next(), aTagIndex += kSTART_VERTEX_DELTA) {
                  TopoDS_Vertex aV = TopoDS::Vertex(anEdgeExp.Current());

                  std::stringstream anAdditionalName; 
                  registerSubShape(
                    selectionLabel(), aV, aTagIndex, aContextFeature, aMyDoc, "", anOrientations);
                }
              }
            }
          }
        }
      }
    }
  }
  // store the selected as primitive
  TNaming_Builder aBuilder(selectionLabel());
  aBuilder.Generated(aSubShape);
    registerSubShape(
      selectionLabel(), aSubShape, 0, aContextFeature, aMyDoc, "", anOrientations, aRefs); 
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
    if (selectionLabel().FindAttribute(TDataStd_Integer::GetID(), anIndex)) { // by internal selection
      if (anIndex->Get() > 0) {
        // update the selection by index
        return aPart->updateInPart(anIndex->Get());
      } else {
        return true; // nothing to do, referencing just by name
      }
    }
    return true; // nothing to do, referencing just by name
  }
  // store the shape (in case part is not loaded it should be usefull
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
  Handle(TDataStd_Name) anAtt;
  if(selectionLabel().FindAttribute(TDataStd_Name::GetID(), anAtt)) {
    aName = TCollection_AsciiString(anAtt->Get()).ToCString();
    return aName;
  }

  std::shared_ptr<GeomAPI_Shape> aSubSh = value();
  ResultPtr aCont = context();

  Model_SelectionNaming aSelNaming(selectionLabel());
  return aSelNaming.namingName(aCont, aSubSh, theDefaultName);
}

// type ::= COMP | COMS | SOLD | SHEL | FACE | WIRE | EDGE | VERT
void Model_AttributeSelection::selectSubShape(
  const std::string& theType, const std::string& theSubShapeName)
{
  if(theSubShapeName.empty() || theType.empty()) return;

  // check this is Part-name: 2 delimiters in the name
  std::size_t aPartEnd = theSubShapeName.find('/');
  if (aPartEnd != string::npos && aPartEnd != theSubShapeName.rfind('/')) {
    std::string aPartName = theSubShapeName.substr(0, aPartEnd);
    ObjectPtr aFound = owner()->document()->objectByName(ModelAPI_ResultPart::group(), aPartName);
    if (aFound.get()) { // found such part, so asking it for the name
      ResultPartPtr aPart = std::dynamic_pointer_cast<ModelAPI_ResultPart>(aFound);
      string aNameInPart = theSubShapeName.substr(aPartEnd + 1);
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
    setValue(aCont, aShapeToBeSelected);
  }
}

int Model_AttributeSelection::Id()
{
  int anID = 0;
  std::shared_ptr<GeomAPI_Shape> aSelection = value();
  std::shared_ptr<GeomAPI_Shape> aContext = context()->shape();
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
