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

bool PartSet_WidgetMultiSelector::setSelection(QList<ModuleBase_ViewerPrs>& theValues,
                                               const bool theToValidate)
{
  bool aSucceed = ModuleBase_WidgetMultiSelector::setSelection(theValues, theToValidate);
  if (aSucceed) {
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
  return aSucceed;
}

//********************************************************************
bool PartSet_WidgetMultiSelector::isValidSelectionCustom(const ModuleBase_ViewerPrs& thePrs)
{
  bool aValid = ModuleBase_WidgetMultiSelector::isValidSelectionCustom(thePrs);
  if (aValid) {
    ObjectPtr anObject = myWorkshop->selection()->getResult(thePrs);
    aValid = myExternalObjectMgr->isValidObject(anObject);
  }
  return aValid;
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

void PartSet_WidgetMultiSelector::getGeomSelection(const ModuleBase_ViewerPrs& thePrs,
                                                   ObjectPtr& theObject,
                                                   GeomShapePtr& theShape)
{
  ModuleBase_WidgetMultiSelector::getGeomSelection(thePrs, theObject, theShape);

  FeaturePtr aSelectedFeature = ModelAPI_Feature::feature(theObject);
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aSelectedFeature);
  // there is no a sketch feature is selected, but the shape exists, try to create an exernal object
  // TODO: unite with the same functionality in PartSet_WidgetShapeSelector
  if (aSPFeature.get() == NULL && myExternalObjectMgr->useExternal()) {
    GeomShapePtr aShape = theShape;
    if (!aShape.get()) {
      ResultPtr aResult = myWorkshop->selection()->getResult(thePrs);
      if (aResult.get())
        aShape = aResult->shape();
    }
    if (aShape.get() != NULL && !aShape->isNull()) {
      if (myIsInVaildate)
        theObject = myExternalObjectMgr->externalObjectValidated(theObject, aShape, sketch());
      else
        theObject = myExternalObjectMgr->externalObject(theObject, aShape, sketch());
    }
  }
}
