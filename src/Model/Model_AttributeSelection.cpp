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
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>
#include <TDF_LabelMap.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS_Edge.hxx>
#include <TopoDS.hxx>
#include <TColStd_MapOfTransient.hxx>

using namespace std;

void Model_AttributeSelection::setValue(const ResultPtr& theContext,
  const boost::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  const boost::shared_ptr<GeomAPI_Shape>& anOldShape = value();
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

boost::shared_ptr<GeomAPI_Shape> Model_AttributeSelection::value()
{
  boost::shared_ptr<GeomAPI_Shape> aResult;
  if (myIsInitialized) {
    Handle(TNaming_NamedShape) aSelection;
    if (selectionLabel().FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
      TopoDS_Shape aSelShape = aSelection->Get();
      aResult = boost::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape);
      aResult->setImpl(new TopoDS_Shape(aSelShape));
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
  return boost::dynamic_pointer_cast<ModelAPI_Result>(myRef.value());
}


void Model_AttributeSelection::setObject(const boost::shared_ptr<ModelAPI_Object>& theObject)
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
    return aSelector.Solve(aScope) == Standard_True;
   
  } else if (aContext->groupName() == ModelAPI_ResultConstruction::group()) {
    // construction: identification by the results indexes, recompute faces and
    // take the face that more close by the indexes
    boost::shared_ptr<GeomAPI_PlanarEdges> aWirePtr = boost::dynamic_pointer_cast<GeomAPI_PlanarEdges>(
      boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(aContext)->shape());
    if (aWirePtr && aWirePtr->hasPlane()) {
        // If this is a wire with plane defined thin it is a sketch-like object
      std::list<boost::shared_ptr<GeomAPI_Shape> > aFaces;
      GeomAlgoAPI_SketchBuilder::createFaces(aWirePtr->origin(), aWirePtr->dirX(),
        aWirePtr->dirY(), aWirePtr->norm(), aWirePtr, aFaces);
      if (aFaces.empty()) // no faces, update can not work correctly
        return false;
      // if there is no edges indexes, any face can be used: take the first
      boost::shared_ptr<Model_Data> aData = 
        boost::dynamic_pointer_cast<Model_Data>(owner()->data());
      TDF_Label aLab = aData->label();
      Handle(TDataStd_IntPackedMap) aSubIds;
      boost::shared_ptr<GeomAPI_Shape> aNewSelected;
      if (!aLab.FindAttribute(TDataStd_IntPackedMap::GetID(), aSubIds) || aSubIds->Extent() == 0) {
        aNewSelected = *(aFaces.begin());
      } else { // searching for most looks-like initial face by the indexes
        // prepare edges of the current resut for the fast searching
        TColStd_MapOfTransient allCurves;
        FeaturePtr aContextFeature = owner()->document()->feature(aContext);
        CompositeFeaturePtr aComposite = 
          boost::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aContextFeature);
        if (!aComposite) // must be composite at least for the current implementation
          return false;
        const int aSubNum = aComposite->numberOfSubs();
        for(int a = 0; a < aSubNum; a++) {
          if (aSubIds->Contains(aComposite->subFeatureId(a))) {
            FeaturePtr aSub = aComposite->subFeature(a);
            const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
            std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
            for(; aRes != aResults.cend(); aRes++) {
              ResultConstructionPtr aConstr = 
                boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
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
        std::list<boost::shared_ptr<GeomAPI_Shape> >::iterator aFacesIter = aFaces.begin();
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
        return true;
      }
    }
  }
  return false; // unknown case
}


void Model_AttributeSelection::selectBody(
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  // perform the selection
  TNaming_Selector aSel(selectionLabel());
  TopoDS_Shape aNewShape = theSubShape ? theSubShape->impl<TopoDS_Shape>() : TopoDS_Shape();
  TopoDS_Shape aContext;

  ResultBodyPtr aBody = boost::dynamic_pointer_cast<ModelAPI_ResultBody>(myRef.value());
  if (aBody)
    aContext = aBody->shape()->impl<TopoDS_Shape>();
  else {
    ResultConstructionPtr aConstr = boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(myRef.value());
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
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  FeaturePtr aContextFeature = owner()->document()->feature(theContext);
  CompositeFeaturePtr aComposite = 
    boost::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aContextFeature);
  if (!aComposite || aComposite->numberOfSubs() == 0) {
    return; // saving of context is enough: result construction contains exactly the needed shape
  }
  boost::shared_ptr<Model_Data> aData = boost::dynamic_pointer_cast<Model_Data>(owner()->data());
  TDF_Label aLab = aData->label();
  // identify the reuslts of sub-object of the composite by edges
  const TopoDS_Shape& aSubShape = theSubShape->impl<TopoDS_Shape>();
  TColStd_MapOfTransient allCurves;
  for(TopExp_Explorer anEdgeExp(aSubShape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next()) {
    TopoDS_Edge anEdge = TopoDS::Edge(anEdgeExp.Current());
    Standard_Real aFirst, aLast;
    Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
    allCurves.Add(aCurve);
  }
  // iterate and store the result ids of sub-elements
  Handle(TDataStd_IntPackedMap) aRefs = TDataStd_IntPackedMap::Set(aLab);
  const int aSubNum = aComposite->numberOfSubs();
  for(int a = 0; a < aSubNum; a++) {
    FeaturePtr aSub = aComposite->subFeature(a);
    const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
    std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
    // there may be many shapes (circle and center): register if at least one is in selection
    for(; aRes != aResults.cend(); aRes++) {
      ResultConstructionPtr aConstr = 
        boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
      if (aConstr->shape() && aConstr->shape()->isEdge()) {
        const TopoDS_Shape& aResShape = aConstr->shape()->impl<TopoDS_Shape>();
        TopoDS_Edge anEdge = TopoDS::Edge(aResShape);
        if (!anEdge.IsNull()) {
          Standard_Real aFirst, aLast;
          Handle(Geom_Curve) aCurve = BRep_Tool::Curve(anEdge, aFirst, aLast);
          if (allCurves.Contains(aCurve)) {
            boost::shared_ptr<Model_Data> aSubData = boost::dynamic_pointer_cast<Model_Data>(aSub->data());
            TDF_Label aSubLab = aSubData->label();
            aRefs->Add(aComposite->subFeatureId(a));
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
