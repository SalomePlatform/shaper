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

#include <TNaming_Selector.hxx>
#include <TNaming_NamedShape.hxx>
#include <TNaming_Tool.hxx>
#include <TopoDS_Shape.hxx>
#include <TDataStd_ReferenceList.hxx>
#include <TopTools_MapOfShape.hxx>
#include <TopExp_Explorer.hxx>

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
    if (myRef.myRef->Label().FindAttribute(TNaming_NamedShape::GetID(), aSelection)) {
      TopoDS_Shape aSelShape = aSelection->Get();
      aResult->setImpl(&aSelShape);
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

void Model_AttributeSelection::selectBody(
    const ResultPtr& theContext, const boost::shared_ptr<GeomAPI_Shape>& theSubShape)
{
  // perform the selection
  TNaming_Selector aSel(myRef.myRef->Label());
  TopoDS_Shape aNewShape = theSubShape ? theSubShape->impl<TopoDS_Shape>() : TopoDS_Shape();
  TopoDS_Shape aContext;

  ResultBodyPtr aBody = boost::dynamic_pointer_cast<ModelAPI_ResultBody>(myRef.value());
  if (aBody)
    aContext = aBody->shape()->impl<TopoDS_Shape>();
  else {
    ResultConstructionPtr aConstr = boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(myRef.value());
    if (aConstr)
      aContext = aConstr->shape()->impl<TopoDS_Shape>();
    else
      throw std::invalid_argument("a result with shape is expected");
  }
  Handle(TNaming_NamedShape) aNS = TNaming_Tool::NamedShape(aNewShape, myRef.myRef->Label());
  TDF_Label aLab = aNS->Label();

  aSel.Select(aNewShape, aContext);
}

#include <BRepTools.hxx>

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
  TopTools_MapOfShape allEdges;
  for(TopExp_Explorer anEdgeExp(aSubShape, TopAbs_EDGE); anEdgeExp.More(); anEdgeExp.Next()) {
    BRepTools::Write(anEdgeExp.Current(), "D:\\selected.brep");
    allEdges.Add(anEdgeExp.Current());
  }
  // iterate and store the result ids of sub-elements
  Handle(TDataStd_ReferenceList) aRefs = TDataStd_ReferenceList::Set(aLab);
  const int aSubNum = aComposite->numberOfSubs();
  for(int a = 0; a < aSubNum; a++) {
    FeaturePtr aSub = aComposite->subFeature(a);
    const std::list<boost::shared_ptr<ModelAPI_Result> >& aResults = aSub->results();
    std::list<boost::shared_ptr<ModelAPI_Result> >::const_iterator aRes = aResults.cbegin();
    // there may be many shapes (circle and center): register if at least one is in selection
    for(; aRes != aResults.cend(); aRes++) {
      ResultConstructionPtr aConstr = 
        boost::dynamic_pointer_cast<ModelAPI_ResultConstruction>(*aRes);
      if (aConstr->shape()) {
        const TopoDS_Shape& aResShape = aConstr->shape()->impl<TopoDS_Shape>();
        BRepTools::Write(aResShape, "D:\\sub.brep");
        if (allEdges.Contains(aResShape)) {
          boost::shared_ptr<Model_Data> aSubData = boost::dynamic_pointer_cast<Model_Data>(aSub->data());
          TDF_Label aSubLab = aSubData->label();
          aRefs->Append(aSubLab);
        }
      }
    }
  }
}
