// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_WidgetShapeSelector.cpp
// Created:     27 Nov 2014
// Author:      Vitaly Smetannikov

#include "PartSet_WidgetShapeSelector.h"
#include "PartSet_Module.h"
#include "PartSet_SketcherMgr.h"

#include <ModelAPI_AttributeRefAttr.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <ModuleBase_Definitions.h>
#include <Config_WidgetAPI.h>

#include <PartSet_ExternalObjectsMgr.h>
#include <SketchPlugin_Feature.h>

#include <XGUI_Workshop.h>

#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_Selection.h>
#include <XGUI_Tools.h>

PartSet_WidgetShapeSelector::PartSet_WidgetShapeSelector(QWidget* theParent,
                                                         ModuleBase_IWorkshop* theWorkshop,
                                                         const Config_WidgetAPI* theData)
: ModuleBase_WidgetShapeSelector(theParent, theWorkshop, theData)
{
  myUseSketchPlane = theData->getBooleanAttribute("use_sketch_plane", true);
  myExternalObjectMgr = new PartSet_ExternalObjectsMgr(theData->getProperty("use_external"), true);
}

PartSet_WidgetShapeSelector::~PartSet_WidgetShapeSelector()
{
  delete myExternalObjectMgr;
}

//********************************************************************
bool PartSet_WidgetShapeSelector::activateSelectionAndFilters(bool toActivate)
{
  bool aHasSelectionFilter = ModuleBase_WidgetShapeSelector::activateSelectionAndFilters
                                                                           (toActivate);
  if (!myUseSketchPlane) {
    XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(myWorkshop);
    PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(aWorkshop->module());
    bool isUsePlaneFilterOnly = !toActivate;
    aModule->sketchMgr()->activatePlaneFilter(isUsePlaneFilterOnly);
  }
  return aHasSelectionFilter;
}

//********************************************************************
bool PartSet_WidgetShapeSelector::isValidSelectionCustom(const ModuleBase_ViewerPrsPtr& thePrs)
{
  bool aValid = ModuleBase_WidgetShapeSelector::isValidSelectionCustom(thePrs);
  if (aValid) {
    ObjectPtr anObject = myWorkshop->selection()->getResult(thePrs);
    aValid = myExternalObjectMgr->isValidObject(anObject);
  }
  return aValid;
}

void PartSet_WidgetShapeSelector::getGeomSelection(const ModuleBase_ViewerPrsPtr& thePrs,
                                                   ObjectPtr& theObject,
                                                   GeomShapePtr& theShape)
{
  ModuleBase_WidgetShapeSelector::getGeomSelection(thePrs, theObject, theShape);

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
    if (aShape.get() != NULL && !aShape->isNull())
      theObject = myExternalObjectMgr->externalObject(theObject, aShape, sketch(), myIsInValidate);
  }
}

//********************************************************************
void PartSet_WidgetShapeSelector::restoreAttributeValue(const AttributePtr& theAttribute,
                                                        const bool theValid)
{
  ModuleBase_WidgetShapeSelector::restoreAttributeValue(theAttribute, theValid);
  myExternalObjectMgr->removeExternal(sketch(), myFeature, myWorkshop, true);
}

