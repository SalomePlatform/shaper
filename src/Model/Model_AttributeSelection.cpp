// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        Model_AttributeSelection.h
// Created:     2 Oct 2014
// Author:      Mikhail PONIKAROV

#include "Model_AttributeSelection.h"
#include "Model_Application.h"
#include "Model_Events.h"
#include "Model_Data.h"
#include <ModelAPI_Feature.h>
#include <ModelAPI_ResultBody.h>
#include <ModelAPI_ResultConstruction.h>
#include <ModelAPI_CompositeFeature.h>
#include <GeomAPI_Shape.h>
#include <GeomAPI_PlanarEdges.h>
#include <GeomAlgoAPI_SketchBuilder.h>
#include <Events_Error.h>

#include <TNaming_Selector.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TNaming_Builder.hxx>
#include <TopoDS_Shape.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDataStd_Integer.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TDF_LabelMap.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TColStd_MapOfTransient.hxx>
#include <gp_Pnt.hxx>
#include <Precision.hxx>

using namespace std;
/// adeed to the index in the packed map to signalize that the vertex of edge is seleted
/// (multiplied by the index of the edge)
static const int kSTART_VERTEX_DELTA = 1000000;

// on this label is stored:
// TNaming_NamedShape - selected shape
// TNaming_Naming - topological selection information (for the body)
// TDataStd_IntPackedMap - indexes of edges in composite element (for construction)
// TDataStd_Integer - type of the selected shape (for construction)
// TDF_Reference - from ReferenceAttribute, the context

void Model_AttributeSelection::setValue(const ResultPtr& theContext,
  const std::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  const std::shared_ptr<GeomAPI_Shape>& anOldShape = value();
  bool isOldShape = 
    (theSubShape == anOldShape || (theSubShape && anOldShape && theSubShape->isEqual(anOldShape)));
  if (isOldShape) return; // shape is the same, so context is also unchanged
  // update the referenced object if needed
  bool isOldContext = theContext == myRef.value();
  if (!isOldContext)
    myRef.setValue(theContext);

  if (theContext->groupName() == ModelAPI_ResultBody::group())
    selectBody(theContext, theSubShape);
  else if (theContext->groupName() == ModelAPI_ResultConstruction::group())
    selectConstruction(theContext, theSubShape);

  myIsInitialized = true;
  owner()->data()->sendAttributeUpdated(this);
}

std::shared_ptr<GeomAPI_Shape> Model_AttributeSelection::value()
{
  std::shared_ptr<GeomAPI_Shape> aResult;
  if (myIsInitialized) {
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

Model_AttributeSelection::Model_AttributeSelection(TDF_Label& theLabel)
  : myRef(theLabel)
{
  myIsInitialized = myRef.isInitialized();
}

ResultPtr Model_AttributeSelection::context() {
  return std::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
}


void Model_AttributeSelection::setObject(const std::shared_ptr<ModelAPI_Object>& theObject)
{
  ModelAPI_AttributeSelection::setObject(theObject);
  myRef.setObject(theObject);
}

bool Model_AttributeSelection::update()
{
  ResultPtr aContext = context();
  if (!aContext) return false;
  if (aContext->groupName() == ModelAPI_ResultBody::group()) {
    // body: just a named shape, use selection mechanism from OCCT
    TNaming_Selector aSelector(selectionLabel());
    TDF_LabelMap aScope; // empty means the whole document
    bool aResult = aSelector.Solve(aScope) == Standard_True;
    owner()->data()->sendAttributeUpdated(this);
    return aResult;
  } else if (aContext->groupName() == ModelAPI_ResultConstruction::group()) {
    // construction: identification by the results indexes, recompute faces and
    // take the face that more close by the indexes
    std::shared_ptr<GeomAPI_PlanarEdges> aWirePtr = 
      std::dynamic_pointer_cast<GeomAPI_PlanarEdges>(
      std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext)->shape());
    if (aWirePtr && aWirePtr->hasPlane()) { // sketch sub-element
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
      FeaturePtr aContextFeature = aContext->document()->feature(aContext);
      CompositeFeaturePtr aComposite = 
        std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aContextFeature);
      if (!aComposite || aComposite->numberOfSubs() == 0) {
        return false;
      }

      if (aShapeType == TopAbs_FACE) {
        // If this is a wire with plane defined thin it is a sketch-like object
        std::list<std::shared_ptr<GeomAPI_Shape> > aFaces;
        GeomAlgoAPI_SketchBuilder::createFaces(aWirePtr->origin(), aWirePtr->dirX(),
          aWirePtr->dirY(), aWirePtr->norm(), aWirePtr, aFaces);
        if (aFaces.empty()) // no faces, update can not work correctly
          return false;
        // if there is no edges indexes, any face can be used: take the first
        std::shared_ptr<GeomAPI_Shape> aNewSelected;
        if (aNoIndexes) {
          aNewSelected = *(aFaces.begin());
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
          // iterate new result faces and searching for these edges
          std::list<std::shared_ptr<GeomAPI_Shape> >::iterator aFacesIter = aFaces.begin();
          double aBestFound = 0; // best percentage of found edges
          for(; aFacesIter != aFaces.end(); aFacesIter++) {
            int aFound = 0, aNotFound = 0;
            TopExp_Explorer anEdgesExp((*aFacesIter)->impl<TopoDS_Shape>(), TopAbs_EDGE);
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
                aNewSelected = *aFacesIter;
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
          if (aSubIds->IsEmpty() || aSubIds->Contains(aFeatureID)) {
            // searching for deltas
            int aVertexNum = 0;
            if (aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA)) aVertexNum = 1;
            else if (aSubIds->Contains(aFeatureID + kSTART_VERTEX_DELTA)) aVertexNum = 2;
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
  TopoDS_Shape aNewShape = theSubShape ? theSubShape->impl<TopoDS_Shape>() : TopoDS_Shape();
  TopoDS_Shape aContext;

  ResultBodyPtr aBody = std::dynamic_pointer_cast<ModelAPI_ResultBody>(myRef.value());
  if (aBody)
    aContext = aBody->shape()->impl<TopoDS_Shape>();
  else {
    ResultConstructionPtr aConstr = std::dynamic_pointer_cast<ModelAPI_ResultConstruction>(myRef.value());
    if (aConstr) {
      aContext = aConstr->shape()->impl<TopoDS_Shape>();
    } else {
      Events_Error::send("A result with shape is expected");
      return;
    }
  }
  aSel.Select(aNewShape, aContext);
}

void Model_AttributeSelection::selectConstruction(
    const ResultPtr& theContext, const std::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  FeaturePtr aContextFeature = theContext->document()->feature(theContext);
  CompositeFeaturePtr aComposite = 
    std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aContextFeature);
  if (!aComposite || aComposite->numberOfSubs() == 0) {
    return; // saving of context is enough: result construction contains exactly the needed shape
  }
  std::shared_ptr<Model_Data> aData = std::dynamic_pointer_cast<Model_Data>(owner()->data());
  TDF_Label aLab = myRef.myRef->Label();
  // identify the reuslts of sub-object of the composite by edges
  const TopoDS_Shape& aSubShape = theSubShape->impl<TopoDS_Shape>();
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
  // iterate and store the result ids of sub-elements
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
              aRefs->Add(aComposite->subFeatureId(a));
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
              // add edges to sub-label to support naming for edges selection
              for(TopExp_Explorer anEdgeExp(aSubShape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next()) {
                TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
                Standard_Real aFirst, aLast;
                Handle(Geom_Curve) aFaceCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
                if (aFaceCurve == aCurve) {
                  TNaming_Builder anEdgeBuilder(selectionLabel().FindChild(anID));
                  anEdgeBuilder.Generated(anEdge);
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
}

TDF_Label Model_AttributeSelection::selectionLabel()
{
  return myRef.myRef->Label().FindChild(1);
}
