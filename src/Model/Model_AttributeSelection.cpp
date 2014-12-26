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
#include <TNaming_Localizer.hxx>
#include <TopoDS_Shape.hxx>
#include <TopoDS_Compound.hxx>
#include <TDataStd_IntPackedMap.hxx>
#include <TDataStd_Integer.hxx>
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
#include <gp_Pnt.hxx>
#include <Precision.hxx>
#include <TDF_ChildIterator.hxx>
#include <TDF_ChildIDIterator.hxx>
#include <TDataStd_Name.hxx>
#include <TopAbs_ShapeEnum.hxx>
#include <TopoDS_Iterator.hxx>
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

  std::string aSelName = buildSubShapeName(theSubShape, theContext);
  if(!aSelName.empty())
	  TDataStd_Name::Set(selectionLabel(), aSelName.c_str()); //set name

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
  if (!aContext) return false;
  if (aContext->groupName() == ModelAPI_ResultBody::group()) {
    // body: just a named shape, use selection mechanism from OCCT
    TNaming_Selector aSelector(selectionLabel());
    bool aResult = aSelector.Solve(scope()) == Standard_True;
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
		if(!aLabel.IsEqual(aNS->Label())) {
		  //aName.erase(); //something is wrong, to be checked!!!
		  aName += "_SomethingWrong";
		  return aName;
		}
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
std::string Model_AttributeSelection::buildSubShapeName(std::shared_ptr<GeomAPI_Shape> theSubShape, 
	                                                    const ResultPtr& theContext)
{
  std::string aName;
  if(theSubShape->isNull() || theContext->shape()->isNull()) return aName;  
  TopoDS_Shape aSubShape = theSubShape->impl<TopoDS_Shape>();
  TopoDS_Shape aContext  = theContext->shape()->impl<TopoDS_Shape>();
  std::shared_ptr<Model_Document> aDoc = std::dynamic_pointer_cast<Model_Document>(theContext->document());

  // check if the subShape is already in DF
  aName = GetShapeName(aDoc, aSubShape, selectionLabel());
  if(aName.empty() ) { // not in the document!
    TopAbs_ShapeEnum aType = aSubShape.ShapeType();
	switch (aType) {
	  case TopAbs_FACE:
      // the Face should be in DF. If it is not a case, it is an error ==> to be dbugged		
		break;
	  case TopAbs_EDGE:
		  {
		  // name structure: F1 | F2 [| F3 | F4], where F1 & F2 the faces which gives the Edge in trivial case
		  // if it is not atrivial case we use localization by neighbours. F3 & F4 - neighbour faces
		  TopTools_IndexedMapOfShape aSMap; // map for ancestors of the sub-shape
		  TopTools_IndexedDataMapOfShapeListOfShape aMap;
		  TopExp::MapShapesAndAncestors(aContext, TopAbs_EDGE, TopAbs_FACE, aMap);
		  bool isTrivialCase(true);
		  for (int i = 1; i <= aMap.Extent(); i++) {
			const TopoDS_Shape& aKey = aMap.FindKey(i);
			if (aKey.IsNotEqual(aSubShape)) continue; // find exactly the selected key

            const TopTools_ListOfShape& anAncestors = aMap.FindFromIndex(i);
			// check that it is not a trivial case (F1 & F2: aNumber = 1)
			isTrivialCase = isTrivial(anAncestors, aSMap);			
			break;
		  }

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
			//int n = aList2.Extent(); //bug!
			// fix is below
			TopTools_ListIteratorOfListOfShape itl2(aList2);
		    for (int i = 1;itl2.More();itl2.Next(),i++) {
			  if(aFMap.Add(itl2.Value()))
				aList.Append(itl2.Value());
			}
			//n = aList.Extent();
#endif
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
		  break;
	}
    // register name			
    // aDoc->addNamingName(selectionLabel(), aName);
	// the selected sub-shape will not be shared and as result it will not require registration
  }
  return aName;
}
