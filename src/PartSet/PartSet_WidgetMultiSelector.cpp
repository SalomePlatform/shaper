// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetMultiSelector.cpp
// Created:     15 Apr 2015
// Author:      Natalia Ermolaeva

#include "PartSet_WidgetMultiSelector.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_Definitions.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_ISelection.h>

#include <Config_WidgetAPI.h>

#include <PartSet_Tools.h>
#include <PartSet_ExternalObjectsMgr.h>
#include <SketchPlugin_Feature.h>

#include <SketchPlugin_ConstraintRigid.h>

#include <XGUI_Workshop.h>

#include <QComboBox>

PartSet_WidgetMultiSelector::PartSet_WidgetMultiSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData,
                                                         const std::string& theParentId)
: ModuleBase_WidgetMultiSelector(theParent, theWorkshop, theData, theParentId)
{
  myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"), false);
}

PartSet_WidgetMultiSelector::~PartSet_WidgetMultiSelector()
{
  delete myExternalObjectMgr;
}

//********************************************************************
void PartSet_WidgetMultiSelector::onSelectionChanged()
{
  ModuleBase_WidgetMultiSelector::onSelectionChanged();
  // TODO(nds): unite with externalObject(), remove parameters
  //myFeature->execute();

  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));

  QObjectPtrList aListOfAttributeObjects;
  for (int i = 0; i < aSelectionListAttr->size(); i++) {
    AttributeSelectionPtr anAttr = aSelectionListAttr->value(i);
    aListOfAttributeObjects.append(anAttr->context());
  }

  myExternalObjectMgr->removeUnusedExternalObjects(aListOfAttributeObjects, sketch(), myFeature);
}

//********************************************************************
void PartSet_WidgetMultiSelector::storeAttributeValue()
{
  myIsInVaildate = true;
  ModuleBase_WidgetMultiSelector::storeAttributeValue();

}

//********************************************************************
void PartSet_WidgetMultiSelector::restoreAttributeValue(const bool theValid)
{
  myIsInVaildate = false;
  ModuleBase_WidgetMultiSelector::restoreAttributeValue(theValid);

  myExternalObjectMgr->removeExternalValidated(sketch(), myFeature);
}

//********************************************************************
bool PartSet_WidgetMultiSelector::setSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  //TopoDS_Shape aShape = thePrs.shape();
  //if (!acceptSubShape(aShape))
  //  return false;

  ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
  /*if (myFeature) {
    // We can not select a result of our feature
    const std::list<ResultPtr>& aResList = myFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    bool isSkipSelf = false;
    for (aIt = aResList.cbegin(); aIt != aResList.cend(); ++aIt) {
      if ((*aIt) == aResult) {
        isSkipSelf = true;
        break;
      }
    }
    if(isSkipSelf)
      return false;
  }*/

  /*GeomShapePtr aGShape = GeomShapePtr();
  const TopoDS_Shape& aTDSShape = thePrs.shape();
  // if only result is selected, an empty shape is set to the model
  if (aTDSShape.IsNull()) {
    //aSelectionListAttr->append(aResult, GeomShapePtr());
  }
  else {
    aGShape = std::shared_ptr<GeomAPI_Shape>(new GeomAPI_Shape());
    //GeomShapePtr aShape(new GeomAPI_Shape());
    aGShape->setImpl(new TopoDS_Shape(aTDSShape));
    // We can not select a result of our feature
    if (aGShape->isEqual(aResult->shape())) {
      //aSelectionListAttr->append(aResult, GeomShapePtr());
      aGShape = GeomShapePtr();
    }
    else {
      //aSelectionListAttr->append(aResult, aShape);
    }
  }*/

  GeomShapePtr aGShape = myWorkshop->selection()->getShape(thePrs);
  setObject(aResult, aGShape);
  return true;
}

//********************************************************************
bool PartSet_WidgetMultiSelector::setObject(const ObjectPtr& theSelectedObject,
                                            const GeomShapePtr& theShape)
{
  ObjectPtr aSelectedObject = theSelectedObject;
  GeomShapePtr aShape = theShape;

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(aSelectedObject);
  //if (aSelectedFeature == myFeature)  // In order to avoid selection of the same object
  //  return false;

  // Do check using of external feature
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);

  // Do check that we can use external feature
  if ((aSPFeature.get() != NULL) && aSPFeature->isExternal() && (!myExternalObjectMgr->useExternal()))
    return false;

  if (aSPFeature.get() == NULL && aShape.get() != NULL && !aShape->isNull() && myExternalObjectMgr->useExternal()) {
    if (myIsInVaildate)
      aSelectedObject = myExternalObjectMgr->externalObjectValidated(theSelectedObject, theShape, sketch());
    else
      aSelectedObject = myExternalObjectMgr->externalObject(theSelectedObject, theShape, sketch());
  }


  ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(aSelectedObject);

  DataPtr aData = myFeature->data();
  AttributeSelectionListPtr aSelectionListAttr = 
    std::dynamic_pointer_cast<ModelAPI_AttributeSelectionList>(aData->attribute(attributeID()));

  aSelectionListAttr->append(aResult, aShape);

  return true;
}
