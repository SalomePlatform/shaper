// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeSelection.h
// Created:     2 Oct 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include "Model_Document.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_CompositeFeature.h>
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
Standard_GUID kCONSTUCTION_SIMPLE_REF_ID("635eacb2-a1d6-4dec-8348-471fae17cb28");

// on this label is stored:
// TNaming_NamedShape - selected shape
// TNaming_Naming - topological selection information (for the body)
// TDataStd_IntPackedMap - indexes of edges in composite element (for construction)
// TDataStd_Integer - type of the selected shape (for construction)
// TDF_Reference - from ReferenceAttribute, the context
#define DDDD 1
void Model_AttributeSelection::setValue(const ResultPtr& theContext,
  const std::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  const std::shared_ptr<GeomAPI_Shape>& anOldShape = value();
  bool isOldContext = theContext == myRef.value();
  bool isOldShape = isOldContext &&
    (theSubShape == anOldShape || (theSubShape && anOldShape && theSubShape->isEqual(anOldShape)));
  if (isOldShape) return; // shape is the same, so context is also unchanged
  // update the referenced object if needed
  if (!isOldContext)
    myRef.setValue(theContext);

  // do noth use naming if selected shape is result shape itself, but not sub-shape
  TDF_Label aSelLab = selectionLabel();
  aSelLab.ForgetAttribute(kSIMPLE_REF_ID);
  aSelLab.ForgetAttribute(kCONSTUCTION_SIMPLE_REF_ID);

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
    if (theContext->shape().get() && theContext->shape()->isEqual(theSubShape)) {
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
    } else {
      selectConstruction(theContext, theSubShape);
    }
  }
  //the attribute initialized state should be changed by sendAttributeUpdated only
  //myIsInitialized = true;

  owner()->data()->sendAttributeUpdated(this);

  std::string aSelName = namingName();
  if(!aSelName.empty())
    TDataStd_Name::Set(selectionLabel(), aSelName.c_str()); //set name
#ifdef DDDD
  //####
  //selectSubShape("FACE",  "Extrusion_1/LateralFace_3");
  //selectSubShape("FACE",  "Extrusion_1/TopFace");
  //selectSubShape("EDGE", "Extrusion_1/TopFace|Extrusion_1/LateralFace_1");
  //selectSubShape("EDGE", "Sketch_1/Edge_6");
#endif
}

std::shared_ptr<GeomAPI_Shape> Model_AttributeSelection::value()
{
  std::shared_ptr<GeomAPI_Shape> aResult;
  if (myRef.isInitialized()) {
    TDF_Label aSelLab = selectionLabel();
    if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // it is just reference to shape, not sub-shape
      ResultPtr aContext = context();
      if (!aContext.get()) 
        return aResult; // empty result
      return aContext->shape();
    }
    if (aSelLab.IsAttribute(kCONSTUCTION_SIMPLE_REF_ID)) { // it is just reference to construction, nothing is in value
        return aResult; // empty result
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

bool Model_AttributeSelection::isInitialized()
{
  if (ModelAPI_AttributeSelection::isInitialized()) { // additional checkings if it is initialized
    std::shared_ptr<GeomAPI_Shape> aResult;
    if (myRef.isInitialized()) {
      TDF_Label aSelLab = selectionLabel();
      if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // it is just reference to shape, not sub-shape
        ResultPtr aContext = context();
        if (!aContext.get()) 
          return false;
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
          return aConstr->shape().get();
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
  return std::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
}


void Model_AttributeSelection::setObject(const std::shared_ptr<ModelAPI_Object>& theObject)
{
  ModelAPI_AttributeSelection::setObject(theObject);
  myRef.setObject(theObject);
}

TDF_LabelMap& Model_AttributeSelection::scope()
{
  if (myScope.IsEmpty()) { // create a new scope if not yet done
    // gets all labels with named shapes that are bofore this feature label (before in history)
    TDF_Label aFeatureLab = std::dynamic_pointer_cast<Model_Data>(
      owner()->data())->label().Father();
    int aFeatureID = aFeatureLab.Tag();
    TDF_ChildIterator aFeaturesIter(aFeatureLab.Father());
    for(; aFeaturesIter.More(); aFeaturesIter.Next()) {
      if (aFeaturesIter.Value().Tag() >= aFeatureID) // the left labels are created later
        break;
      TDF_ChildIDIterator aNSIter(aFeaturesIter.Value(), TNaming_NamedShape::GetID(), 1);
      for(; aNSIter.More(); aNSIter.Next()) {
        Handle(TNaming_NamedShape) aNS = Handle(TNaming_NamedShape)::DownCast(aNSIter.Value());
        if (!aNS.IsNull() && aNS->Evolution() != TNaming_SELECTED) {
          myScope.Add(aNS->Label());
        }
      }
    }
  }
  return myScope;
}

bool Model_AttributeSelection::update()
{
  ResultPtr aContext = context();
  if (!aContext.get()) return false;
  TDF_Label aSelLab = selectionLabel();
  if (aSelLab.IsAttribute(kSIMPLE_REF_ID)) { // it is just reference to shape, not sub-shape
    return aContext->shape() && !aContext->shape()->isNull();
  }
  if (aSelLab.IsAttribute(kCONSTUCTION_SIMPLE_REF_ID)) { // it is just reference to construction, not sub-shape
    return aContext->shape() && !aContext->shape()->isNull();
  }

  if (aContext->groupName() == ModelAPI_ResultBody::group()) {
    // body: just a named shape, use selection mechanism from OCCT
    TNaming_Selector aSelector(aSelLab);
    bool aResult = aSelector.Solve(scope()) == Standard_True;
    owner()->data()->sendAttributeUpdated(this);
    return aResult;
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
        return false;
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
        return false;
      }

      if (aShapeType == TopAbs_FACE) { // compound is for the whole sketch selection
        // If this is a wire with plane defined thin it is a sketch-like object
        if (!aConstructionContext->facesNum()) // no faces, update can not work correctly
          return false;
        // if there is no edges indexes, any face can be used: take the first
        std::shared_ptr<GeomAPI_Shape> aNewSelected;
        if (aNoIndexes) {
          aNewSelected = aConstructionContext->face(0);
        } else { // searching for most looks-like initial face by the indexes
          // prepare edges of the current resut for the fast searching
          TColStd_MapOfTransient allCurves;
          const int aSubNum = aComposite->numberOfSubs();
          for(int a = 0; a < aSubNum; a++) {
            if (aSubIds->Contains(aComposite->subFeatureId(a))) {
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
                    allCurves.Add(aCurve);
                  }
                }
              }
            }
          }
          double aBestFound = 0; // best percentage of found edges
          for(int aFaceIndex = 0; aFaceIndex < aConstructionContext->facesNum(); aFaceIndex++) {
            int aFound = 0, aNotFound = 0;
            TopExp_Explorer anEdgesExp(
              aConstructionContext->face(aFaceIndex)->impl<TopoDS_Shape>(), TopAbs_EDGE);
            for(; anEdgesExp.More(); anEdgesExp.Next()) {
              TopoDS_Edge anEdge = TopoDS::Edge(anEdgesExp.Current());
              if (!anEdge.IsNull()) {
                Standard_Real aFirst, aLast;
                Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                if (allCurves.Contains(aCurve)) {
                  aFound++;
                } else {
                  aNotFound++;
                }
              }
            }
            if (aFound + aNotFound != 0) {
              double aPercentage = double(aFound) / double(aFound + aNotFound);
              if (aPercentage > aBestFound) {
                aBestFound = aPercentage;
                aNewSelected = aConstructionContext->face(aFaceIndex);
              }
            }
          }
        }
        if (aNewSelected) { // store this new selection
          selectConstruction(aContext, aNewSelected);
          owner()->data()->sendAttributeUpdated(this);
          return true;
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
                return true;
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
                    return true;
                  } else if (aRes->shape()->isEdge() && aVertexNum > 0) {
                    const TopoDS_Shape& anEdge = aRes->shape()->impl<TopoDS_Shape>();
                    int aVIndex = 1;
                    for(TopExp_Explorer aVExp(anEdge, TopAbs_VERTEX); aVExp.More(); aVExp.Next()) {
                      if (aVIndex == aVertexNum) { // found!
                        std::shared_ptr<GeomAPI_Shape> aVertex(new GeomAPI_Shape);
                        aVertex->setImpl(new TopoDS_Shape(aVExp.Current()));
                        selectConstruction(aContext, aVertex);
                        owner()->data()->sendAttributeUpdated(this);
                        return true;
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
      return true;
    }
  }
  return false; // unknown case
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
  /*
  FeaturePtr aFeatureOwner = std::dynamic_pointer_cast<ModelAPI_Feature>(owner());
  if (aFeatureOwner.get())
    aFeatureOwner->eraseResults();
    */
  if (!aContext.IsNull()) {
    aSel.Select(aNewShape, aContext); 
  }
}

/// registers the name of the shape in the label (theID == 0) of sub label (theID is a tag)
/// if theID is zero, 
static void registerSubShape(TDF_Label theMainLabel, TopoDS_Shape theShape,
  const int theID, const FeaturePtr& theContextFeature, std::shared_ptr<Model_Document> theDoc,
  std::string theAdditionalName,
  Handle(TDataStd_IntPackedMap) theRefs = Handle(TDataStd_IntPackedMap)())
{
  TDF_Label aLab = theID == 0 ? theMainLabel : theMainLabel.FindChild(theID);
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
  } else { // make a compisite name from all sub-elements indexes: "1_2_3_4"
    TColStd_MapIteratorOfPackedMapOfInteger aRef(theRefs->GetMap());
    for(; aRef.More(); aRef.Next()) {
      aName<<"-"<<aRef.Key();
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
              if (aShapeType != TopAbs_EDGE) { // edge do not need the sub-edges on sub-labels
                // add edges to sub-label to support naming for edges selection
                TopExp_Explorer anEdgeExp(aSubShape, TopAbs_EDGE);
                for(; anEdgeExp.More(); anEdgeExp.Next()) {
                  TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
                  Standard_Real aFirst, aLast;
                  Handle(Geom_Curve) aFaceCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                  if (aFaceCurve == aCurve) {
                    registerSubShape(selectionLabel(), anEdge, anID, aContextFeature, aMyDoc, "");
                  }
                }
              } else { // put vertices of the selected edge to sub-labels
                // add edges to sub-label to support naming for edges selection
                TopExp_Explorer anEdgeExp(aSubShape, TopAbs_VERTEX);
                int aTagIndex = anID + kSTART_VERTEX_DELTA;
                for(; anEdgeExp.More(); anEdgeExp.Next(), aTagIndex += kSTART_VERTEX_DELTA) {
                  TopoDS_Vertex aV = TopoDS::Vertex(anEdgeExp.Current());

                  std::stringstream anAdditionalName; 
                  registerSubShape(selectionLabel(), aV, aTagIndex, aContextFeature, aMyDoc,
                    "");
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
  registerSubShape(selectionLabel(), aSubShape, 0, aContextFeature, aMyDoc, "", aRefs);
}

TDF_Label Model_AttributeSelection::selectionLabel()
{
  return myRef.myRef->Label().FindChild(1);
}

#define FIX_BUG1 1
std::string GetShapeName(std::shared_ptr<Model_Document> theDoc, const TopoDS_Shape& theShape, 
  const TDF_Label& theLabel)
{
  std::string aName;
  // check if the subShape is already in DF
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(theShape, theLabel);
  Handle(TDataStd_Name) anAttr;
  if(!aNS.IsNull() && !aNS->IsEmpty()) { // in the document    
    if(aNS->Label().FindAttribute(TDataStd_Name::GetID(), anAttr)) {
      aName = TCollection_AsciiString(anAttr->Get()).ToCString();
      if(!aName.empty()) {	    
        const TDF_Label& aLabel = theDoc->findNamingName(aName);
        /* MPV: the same shape with the same name may be duplicated on different labels (selection of the same construction object)
        if(!aLabel.IsEqual(aNS->Label())) {
        //aName.erase(); //something is wrong, to be checked!!!
        aName += "_SomethingWrong";
        return aName;
        }*/

        static const std::string aPostFix("_");
        TNaming_Iterator anItL(aNS);
        for(int i = 1; anItL.More(); anItL.Next(), i++) {
          if(anItL.NewShape() == theShape) {
            aName += aPostFix;
            aName += TCollection_AsciiString (i).ToCString();
            break;
          }
        }
        /* MPV: the code below does not work because of TNaming_Tool.cxx line 145: aNS->Get() uses maps
        const TopoDS_Shape& aShape = aNS->Get();
        if(aShape.ShapeType() == TopAbs_COMPOUND) {
        std::string aPostFix("_");
        TopoDS_Iterator it(aShape);			
        for (int i = 1;it.More();it.Next(), i++) {
        if(it.Value() == theShape) {
        aPostFix += TCollection_AsciiString (i).ToCString();
        aName    += aPostFix;
        break;
        }
        else continue;			  			
        }
        }
        */
      }	
    }
  }
  return aName;
}

bool isTrivial (const TopTools_ListOfShape& theAncestors, TopTools_IndexedMapOfShape& theSMap)
{
  // a trivial case: F1 & F2,  aNumber = 1, i.e. intersection gives 1 edge.
  TopoDS_Compound aCmp;
  BRep_Builder BB;
  BB.MakeCompound(aCmp);
  TopTools_ListIteratorOfListOfShape it(theAncestors);
  for(;it.More();it.Next()) {
    BB.Add(aCmp, it.Value());
    theSMap.Add(it.Value());
  }
  int aNumber(0);
  TopTools_IndexedDataMapOfShapeListOfShape aMap2;
  TopExp::MapShapesAndAncestors(aCmp, TopAbs_EDGE, TopAbs_FACE, aMap2);
  for (int i = 1; i <= aMap2.Extent(); i++) {
    const TopoDS_Shape& aKey = aMap2.FindKey(i);
    const TopTools_ListOfShape& anAncestors = aMap2.FindFromIndex(i);
    if(anAncestors.Extent() > 1) aNumber++;
  }
  if(aNumber > 1) return false;
  return true;
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
  aName = "Undefined name";
  if(!aCont.get() || aCont->shape()->isNull()) 
    return !theDefaultName.empty() ? theDefaultName : aName;
  if (!aSubSh.get() || aSubSh->isNull()) { // no subshape, so just the whole feature name
    return aCont->data()->name();
  }
  TopoDS_Shape aSubShape = aSubSh->impl<TopoDS_Shape>();
  TopoDS_Shape aContext  = aCont->shape()->impl<TopoDS_Shape>();
#ifdef DEB_NAMING
  if(aSubShape.ShapeType() == TopAbs_COMPOUND) {
  BRepTools::Write(aSubShape, "Selection.brep");
  BRepTools::Write(aContext, "Context.brep");
  }
#endif
  std::shared_ptr<Model_Document> aDoc = 
    std::dynamic_pointer_cast<Model_Document>(aCont->document());

  // check if the subShape is already in DF
  aName = GetShapeName(aDoc, aSubShape, selectionLabel());
  if(aName.empty() ) { // not in the document!
    TopAbs_ShapeEnum aType = aSubShape.ShapeType();
    switch (aType) {
    case TopAbs_FACE:
      // the Face should be in DF. If it is not the case, it is an error ==> to be debugged		
		break;
	  case TopAbs_EDGE:
		  {
		  // name structure: F1 | F2 [| F3 | F4], where F1 & F2 the faces which gives the Edge in trivial case
		  // if it is not atrivial case we use localization by neighbours. F3 & F4 - neighbour faces	
		  if (BRep_Tool::Degenerated(TopoDS::Edge(aSubShape))) {
			  aName = "Degenerated_Edge";
			  break;
		  }    
		  TopTools_IndexedDataMapOfShapeListOfShape aMap;
		  TopExp::MapShapesAndAncestors(aContext, TopAbs_EDGE, TopAbs_FACE, aMap);
		  TopTools_IndexedMapOfShape aSMap; // map for ancestors of the sub-shape
		  bool isTrivialCase(true);
/*		  for (int i = 1; i <= aMap.Extent(); i++) {
			const TopoDS_Shape& aKey = aMap.FindKey(i);
			//if (aKey.IsNotEqual(aSubShape)) continue; // find exactly the selected key
			if (aKey.IsSame(aSubShape)) continue;
            const TopTools_ListOfShape& anAncestors = aMap.FindFromIndex(i);
			// check that it is not a trivial case (F1 & F2: aNumber = 1)
			isTrivialCase = isTrivial(anAncestors, aSMap);			
			break;
		  }*/
		  if(aMap.Contains(aSubShape)) {
			const TopTools_ListOfShape& anAncestors = aMap.FindFromKey(aSubShape);
			// check that it is not a trivial case (F1 & F2: aNumber = 1)
			isTrivialCase = isTrivial(anAncestors, aSMap);		
		  } else 
			  break;
		  TopTools_ListOfShape aListOfNbs;
		  if(!isTrivialCase) { // find Neighbors
			TNaming_Localizer aLocalizer;
			TopTools_MapOfShape aMap3;
			aLocalizer.FindNeighbourg(aContext, aSubShape, aMap3);
			//int n = aMap3.Extent();
			TopTools_MapIteratorOfMapOfShape it(aMap3);
			for(;it.More();it.Next()) {
			  const TopoDS_Shape& aNbShape = it.Key(); // neighbor edge
			  //TopAbs_ShapeEnum aType = aNbShape.ShapeType();
			  const TopTools_ListOfShape& aList  = aMap.FindFromKey(aNbShape);
			  TopTools_ListIteratorOfListOfShape it2(aList);
			  for(;it2.More();it2.Next()) {
				if(aSMap.Contains(it2.Value())) continue; // skip this Face
				aListOfNbs.Append(it2.Value());
			  }
			}
		  }  // else a trivial case
		  
		  // build name of the sub-shape Edge
		  for(int i=1; i <= aSMap.Extent(); i++) {
			const TopoDS_Shape& aFace = aSMap.FindKey(i);
			std::string aFaceName = GetShapeName(aDoc, aFace, selectionLabel());
			if(i == 1)
			  aName = aFaceName;
			else 
			  aName += "|" + aFaceName;
		  }
		  TopTools_ListIteratorOfListOfShape itl(aListOfNbs);
		  for (;itl.More();itl.Next()) {
			std::string aFaceName = GetShapeName(aDoc, itl.Value(), selectionLabel());
			aName += "|" + aFaceName;
		  }		  
		  }
		  break;

    case TopAbs_VERTEX:
      // name structure (Monifold Topology): 
      // 1) F1 | F2 | F3 - intersection of 3 faces defines a vertex - trivial case.
      // 2) F1 | F2 | F3 [|F4 [|Fn]] - redundant definition, but it should be kept as is to obtain safe recomputation
      // 2) F1 | F2      - intersection of 2 faces definses a vertex - applicable for case
      //                   when 1 faces is cylindrical, conical, spherical or revolution and etc.
      // 3) E1 | E2 | E3 - intersection of 3 edges defines a vertex - when we have case of a shell
      //                   or compound of 2 open faces.
      // 4) E1 | E2      - intesection of 2 edges defines a vertex - when we have a case of 
      //                   two independent edges (wire or compound)
      // implemented 2 first cases
      {
        TopTools_IndexedDataMapOfShapeListOfShape aMap;
        TopExp::MapShapesAndAncestors(aContext, TopAbs_VERTEX, TopAbs_FACE, aMap);
        const TopTools_ListOfShape& aList2  = aMap.FindFromKey(aSubShape);
        TopTools_ListOfShape aList;
        TopTools_MapOfShape aFMap;
#ifdef FIX_BUG1
        //int n = aList2.Extent(); //bug! duplication
        // fix is below
        TopTools_ListIteratorOfListOfShape itl2(aList2);
        for (int i = 1;itl2.More();itl2.Next(),i++) {
          if(aFMap.Add(itl2.Value()))
            aList.Append(itl2.Value());
        }
        //n = aList.Extent();
#endif
        int n = aList.Extent();
        if(n < 3) { // open topology case or Compound case => via edges
          TopTools_IndexedDataMapOfShapeListOfShape aMap;
          TopExp::MapShapesAndAncestors(aContext, TopAbs_VERTEX, TopAbs_EDGE, aMap);
          const TopTools_ListOfShape& aList22  = aMap.FindFromKey(aSubShape);
          if(aList22.Extent() >= 2)  { // regular solution
#ifdef FIX_BUG1

            // bug! duplication; fix is below
            aFMap.Clear();
            TopTools_ListOfShape aListE;
            TopTools_ListIteratorOfListOfShape itl2(aList22);
            for (int i = 1;itl2.More();itl2.Next(),i++) {
              if(aFMap.Add(itl2.Value()))
                aListE.Append(itl2.Value());
            }
            n = aListE.Extent();
#endif
            TopTools_ListIteratorOfListOfShape itl(aListE);
            for (int i = 1;itl.More();itl.Next(),i++) {
              const TopoDS_Shape& anEdge = itl.Value();
              std::string anEdgeName = GetShapeName(aDoc, anEdge, selectionLabel());
              if(i == 1)
                aName = anEdgeName;
              else 
                aName += "|" + anEdgeName;
            }
          }//reg
          else { // dangle vertex: if(aList22.Extent() == 1)
            //it should be already in DF
          }
        } 
        else {
          TopTools_ListIteratorOfListOfShape itl(aList);
          for (int i = 1;itl.More();itl.Next(),i++) {
            const TopoDS_Shape& aFace = itl.Value();
            std::string aFaceName = GetShapeName(aDoc, aFace, selectionLabel());
            if(i == 1)
              aName = aFaceName;
            else 
              aName += "|" + aFaceName;
          }
        }
      }
      break;
    }
    // register name			
    // aDoc->addNamingName(selectionLabel(), aName);
    // the selected sub-shape will not be shared and as result it will not require registration
  }
  return aName;
}

TopAbs_ShapeEnum translateType (const std::string& theType)
{
  // map from the textual shape types to OCCT enumeration
  static std::map<std::string, TopAbs_ShapeEnum> MyShapeTypes;
  if (MyShapeTypes.size() == 0) {
    MyShapeTypes["face"] = TopAbs_FACE;
    MyShapeTypes["faces"] = TopAbs_FACE;
    MyShapeTypes["vertex"] = TopAbs_VERTEX;
    MyShapeTypes["vertices"] = TopAbs_VERTEX;
    MyShapeTypes["wire"] = TopAbs_WIRE;
    MyShapeTypes["edge"] = TopAbs_EDGE;
    MyShapeTypes["edges"] = TopAbs_EDGE;
    MyShapeTypes["shell"] = TopAbs_SHELL;
    MyShapeTypes["solid"] = TopAbs_SOLID;
    MyShapeTypes["solids"] = TopAbs_SOLID;
    MyShapeTypes["FACE"] = TopAbs_FACE;
    MyShapeTypes["FACES"] = TopAbs_FACE;
    MyShapeTypes["VERTEX"] = TopAbs_VERTEX;
    MyShapeTypes["VERTICES"] = TopAbs_VERTEX;
    MyShapeTypes["WIRE"] = TopAbs_WIRE;
    MyShapeTypes["EDGE"] = TopAbs_EDGE;
    MyShapeTypes["EDGES"] = TopAbs_EDGE;
    MyShapeTypes["SHELL"] = TopAbs_SHELL;
    MyShapeTypes["SOLID"] = TopAbs_SOLID;
    MyShapeTypes["SOLIDS"] = TopAbs_SOLID;
  }
  if (MyShapeTypes.find(theType) != MyShapeTypes.end())
    return MyShapeTypes[theType];
  Events_Error::send("Shape type defined in XML is not implemented!");
  return TopAbs_SHAPE;
}

const TopoDS_Shape getShapeFromNS(
  const std::string& theSubShapeName, Handle(TNaming_NamedShape) theNS)
{
  TopoDS_Shape aSelection;
  std::string::size_type n = theSubShapeName.rfind('/');			
  if (n == std::string::npos) n = 0;
  std::string aSubString = theSubShapeName.substr(n + 1);
  n = aSubString.rfind('_');
  if (n == std::string::npos) return aSelection;
  aSubString = aSubString.substr(n+1);
  int indx = atoi(aSubString.c_str());

  TNaming_Iterator anItL(theNS);
  for(int i = 1; anItL.More(); anItL.Next(), i++) {
    if (i == indx) {
      return anItL.NewShape();
    }
  }
  return aSelection;	
}

const TopoDS_Shape findFaceByName(
  const std::string& theSubShapeName, std::shared_ptr<Model_Document> theDoc)
{
  TopoDS_Shape aFace;
  std::string::size_type n, nb = theSubShapeName.rfind('/');			
  if (nb == std::string::npos) nb = 0;
  std::string aSubString = theSubShapeName.substr(nb + 1);
  n = aSubString.rfind('_');
  if (n != std::string::npos) {
    std::string aSubStr2 = aSubString.substr(0, n);
    aSubString  = theSubShapeName.substr(0, nb + 1);
    aSubString = aSubString + aSubStr2;	
  } else
    aSubString = theSubShapeName;

  const TDF_Label& aLabel = theDoc->findNamingName(aSubString);
  if(aLabel.IsNull()) return aFace;
  Handle(TNaming_NamedShape) aNS;
  if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
    aFace = getShapeFromNS(theSubShapeName, aNS);
  }
  return aFace;
}

int ParseName(const std::string& theSubShapeName,   std::list<std::string>& theList)
{
  std::string aName = theSubShapeName;
  std::string aLastName;
  int n1(0), n2(0); // n1 - start position, n2 - position of the delimiter
  while ((n2 = aName.find('|', n1)) != std::string::npos) {
    const std::string aName1 = aName.substr(n1, n2 - n1); //name of face
    theList.push_back(aName1);	
    n1 = n2 + 1;
    aLastName = aName.substr(n1);
  }
  if(!aLastName.empty())
    theList.push_back(aLastName);
  return theList.size();
}

const TopoDS_Shape findCommonShape(
  const TopAbs_ShapeEnum theType, const TopTools_ListOfShape& theList)
{
  TopoDS_Shape aShape;
  std::vector<TopTools_MapOfShape> aVec;
  TopTools_MapOfShape aMap1, aMap2, aMap3, aMap4;
  if(theList.Extent() > 1) {
    aVec.push_back(aMap1);
    aVec.push_back(aMap2);
  }
  if(theList.Extent() > 2)
    aVec.push_back(aMap3);
  if(theList.Extent() == 4)
    aVec.push_back(aMap4);

  //fill maps
  TopTools_ListIteratorOfListOfShape it(theList);
  for(int i = 0;it.More();it.Next(),i++) {
    const TopoDS_Shape& aFace = it.Value();		
    if(i < 2) {
      TopExp_Explorer anExp (aFace, theType);
      for(;anExp.More();anExp.Next()) {
        const TopoDS_Shape& anEdge = anExp.Current();
        if (!anEdge.IsNull())
          aVec[i].Add(anExp.Current());
      }
    } else {
      TopExp_Explorer anExp (aFace, TopAbs_VERTEX);
      for(;anExp.More();anExp.Next()) {
        const TopoDS_Shape& aVertex = anExp.Current();
        if (!aVertex.IsNull())
          aVec[i].Add(anExp.Current());
      }
    }
  }
  //trivial case: 2 faces
  TopTools_ListOfShape aList;
  TopTools_MapIteratorOfMapOfShape it2(aVec[0]);
  for(;it2.More();it2.Next()) {
    if(aVec[1].Contains(it2.Key())) {
      aShape = it2.Key();
      if(theList.Extent() == 2)
        break;
      else 
        aList.Append(it2.Key());
    }
  }
  if(aList.Extent() && aVec.size() > 3) {// list of common edges ==> search ny neighbors 
    if(aVec[2].Extent() && aVec[3].Extent()) {
      TopTools_ListIteratorOfListOfShape it(aList);
      for(;it.More();it.Next()) {
        const TopoDS_Shape& aCand = it.Value();
        // not yet completelly implemented, to be rechecked
        TopoDS_Vertex aV1, aV2;
        TopExp::Vertices(TopoDS::Edge(aCand), aV1, aV2);
        int aNum(0);
        if(aVec[2].Contains(aV1)) aNum++;
        else if(aVec[2].Contains(aV2)) aNum++;
        if(aVec[3].Contains(aV1)) aNum++;
        else if(aVec[3].Contains(aV2)) aNum++;
        if(aNum == 2) {
          aShape = aCand;
          break;
        }
      }
    }
  }

  if(aList.Extent() && aVec.size() == 3) {

    TopTools_ListIteratorOfListOfShape it(aList);
    for(;it.More();it.Next()) {
      const TopoDS_Shape& aCand = it.Value();
      if(aVec[2].Contains(aCand)) {
        aShape = aCand;
        break;
      }
    }
  }
  return aShape;
}

std::string getContextName(const std::string& theSubShapeName)
{
  std::string aName;
  std::string::size_type n = theSubShapeName.find('/');			
  if (n == std::string::npos) return aName;
  aName = theSubShapeName.substr(0, n);
  return aName;
}
// type ::= COMP | COMS | SOLD | SHEL | FACE | WIRE | EDGE | VERT
void Model_AttributeSelection::selectSubShape(
  const std::string& theType, const std::string& theSubShapeName)
{
  if(theSubShapeName.empty() || theType.empty()) return;
  TopAbs_ShapeEnum aType = translateType(theType);
  std::shared_ptr<Model_Document> aDoc = 
    std::dynamic_pointer_cast<Model_Document>(owner()->document());
  std::string aContName = getContextName(theSubShapeName);
  if(aContName.empty()) return;
  //ResultPtr aCont = context();
  ResultPtr aCont = aDoc->findByName(aContName);
  if(!aCont.get() || aCont->shape()->isNull()) return;
  TopoDS_Shape aContext  = aCont->shape()->impl<TopoDS_Shape>();
  TopAbs_ShapeEnum aContType = aContext.ShapeType();
  if(aType <= aContType) return; // not applicable


  TopoDS_Shape aSelection;
  switch (aType) 
  {
  case TopAbs_COMPOUND:
    break;
  case TopAbs_COMPSOLID:
    break;
  case TopAbs_SOLID:
    break;
  case TopAbs_SHELL:
    break;
  case TopAbs_FACE:
    {
      const TopoDS_Shape aSelection = findFaceByName(theSubShapeName, aDoc);
      if(!aSelection.IsNull()) {// Select it
        std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected(new GeomAPI_Shape());
        aShapeToBeSelected->setImpl(new TopoDS_Shape(aSelection));
        setValue(aCont, aShapeToBeSelected);
      }
    }
    break;
  case TopAbs_WIRE:
    break;
  case TopAbs_EDGE:
    {  
      TopoDS_Shape aSelection;// = findFaceByName(theSubShapeName, aDoc);
      const TDF_Label& aLabel = aDoc->findNamingName(theSubShapeName);
      if(!aLabel.IsNull()) {
        Handle(TNaming_NamedShape) aNS;
        if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
          aSelection = getShapeFromNS(theSubShapeName, aNS);
        }
      }
      if(aSelection.IsNull()) {
        std::list<std::string> aListofNames;
        int n = ParseName(theSubShapeName, aListofNames);
        if(n > 1 && n < 5) {
          TopTools_ListOfShape aList;
          std::list<std::string>::iterator it =aListofNames.begin();
          for(;it != aListofNames.end();it++){
            const TopoDS_Shape aFace = findFaceByName(*it, aDoc);
            aList.Append(aFace);		
          }
          aSelection = findCommonShape(TopAbs_EDGE, aList);
        }
      }
      if(!aSelection.IsNull()) {// Select it
        std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected(new GeomAPI_Shape());
        aShapeToBeSelected->setImpl(new TopoDS_Shape(aSelection));
        setValue(aCont, aShapeToBeSelected);
      }
    }
    break;
  case TopAbs_VERTEX:
    {
      TopoDS_Shape aSelection;
      const TDF_Label& aLabel = aDoc->findNamingName(theSubShapeName);
      if(!aLabel.IsNull()) {
        Handle(TNaming_NamedShape) aNS;
        if(aLabel.FindAttribute(TNaming_NamedShape::GetID(), aNS)) {
          aSelection = getShapeFromNS(theSubShapeName, aNS);
        }
      }
      if(aSelection.IsNull()) {
        std::list<std::string> aListofNames;
        int n = ParseName(theSubShapeName, aListofNames);
        if(n > 1 && n < 4) { // 2 || 3
          TopTools_ListOfShape aList;
          std::list<std::string>::iterator it = aListofNames.begin();
          for(; it != aListofNames.end(); it++){
            const TopoDS_Shape aFace = findFaceByName(*it, aDoc);
            if(!aFace.IsNull())
              aList.Append(aFace);		
          }
          aSelection = findCommonShape(TopAbs_VERTEX, aList);
        }
      }
      if(!aSelection.IsNull()) {// Select it
        std::shared_ptr<GeomAPI_Shape> aShapeToBeSelected(new GeomAPI_Shape());
        aShapeToBeSelected->setImpl(new TopoDS_Shape(aSelection));
        setValue(aCont, aShapeToBeSelected);
      }
    }
    break;
  default: //TopAbs_SHAPE
    return;
  }

}

int Model_AttributeSelection::Id()
{
  std::shared_ptr<GeomAPI_Shape> aSelection = value();
  std::shared_ptr<GeomAPI_Shape> aContext = context()->shape();
  const TopoDS_Shape& aMainShape = aContext->impl<TopoDS_Shape>();
  const TopoDS_Shape& aSubShape = aSelection->impl<TopoDS_Shape>();
  int anID = 0;
  if (aSelection && !aSelection->isNull() &&
    aContext   && !aContext->isNull())
  {
    TopTools_IndexedMapOfShape aSubShapesMap;
    TopExp::MapShapes(aMainShape, aSubShapesMap);
    anID = aSubShapesMap.FindIndex(aSubShape);
  }
  return anID;
}
