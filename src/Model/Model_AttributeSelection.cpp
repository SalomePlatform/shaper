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
#include <GeomAPI_Shape.h>

#include <TNaming_Selector.hxx>
#include <TNaming_NamedShape.hxx>
#include <TopoDS_Shape.hxx>

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
  aSel.Select(aNewShape, aContext);
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
  myRef.setObject(theObject);
}
