// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_CustomPrs.cpp
// Created:     30 Jun 2015
// Author:      Natalia ERMOLAEVA

#include <PartSet_CustomPrs.h>
#include <PartSet_Module.h>
#include "PartSet_OperationPrs.h"

#include <XGUI_ModuleConnector.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>

#include <Config_PropManager.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Prs3d_PointAspect.hxx>

//#define DO_NOT_VISUALIZE_CUSTOM_PRESENTATION

PartSet_CustomPrs::PartSet_CustomPrs(ModuleBase_IWorkshop* theWorkshop)
  : myWorkshop(theWorkshop), myIsActive(false)
{
  initPresentation(ModuleBase_IModule::CustomizeDependedAndResults);
  initPresentation(ModuleBase_IModule::CustomizeHighlightedObjects);
}

bool PartSet_CustomPrs::isActive()
{
  return myIsActive;
}

bool PartSet_CustomPrs::activate(const FeaturePtr& theFeature, const bool theUpdateViewer)
{
#ifdef DO_NOT_VISUALIZE_CUSTOM_PRESENTATION
  return false;
#endif

  myIsActive = true;


  bool isModified = false;
  getPresentation(ModuleBase_IModule::CustomizeDependedAndResults)->setFeature(theFeature);
  getPresentation(ModuleBase_IModule::CustomizeHighlightedObjects)->setFeature(theFeature);
  if (theFeature.get()) {
    displayPresentation(ModuleBase_IModule::CustomizeDependedAndResults, theUpdateViewer);
    displayPresentation(ModuleBase_IModule::CustomizeHighlightedObjects, theUpdateViewer);
    isModified = true;
  }
  return isModified;
}

bool PartSet_CustomPrs::deactivate(const bool theUpdateViewer)
{
  myIsActive = false;
  bool isModified = false;

  getPresentation(ModuleBase_IModule::CustomizeDependedAndResults)->setFeature(FeaturePtr());
  getPresentation(ModuleBase_IModule::CustomizeHighlightedObjects)->setFeature(FeaturePtr());

  erasePresentation(ModuleBase_IModule::CustomizeDependedAndResults, theUpdateViewer);
  erasePresentation(ModuleBase_IModule::CustomizeHighlightedObjects, theUpdateViewer);
  isModified = true;

  return isModified;
}

bool PartSet_CustomPrs::displayPresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                            const bool theUpdateViewer)
{
  bool isModified = false;
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation(theFlag);
  if (theFlag == ModuleBase_IModule::CustomizeDependedAndResults) {
    PartSet_OperationPrs::getFeatureShapes(anOperationPrs->getFeature(), myWorkshop,
                                           anOperationPrs->featureShapes());
    anOperationPrs->updateShapes();
  }
  else if (theFlag == ModuleBase_IModule::CustomizeHighlightedObjects) {
    PartSet_OperationPrs::getHighlightedShapes(myWorkshop, anOperationPrs->featureShapes());
  }

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull() && !aContext->IsDisplayed(anOperationPrs)) {
    if (anOperationPrs->hasShapes()) {
      PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
      XGUI_Workshop* aWorkshop = workshop();
      aWorkshop->displayer()->displayAIS(myPresentations[theFlag], false/*load object in selection*/,
                                         theUpdateViewer);
      aContext->SetZLayer(anOperationPrs, aModule->getVisualLayerId());
      isModified = true;
    }
  }
  else {
    if (!anOperationPrs->hasShapes()) {
      erasePresentation(theFlag, theUpdateViewer);
      isModified = true;
    }
    else {
      anOperationPrs->Redisplay();
      isModified = true;
      if (theUpdateViewer)
        workshop()->displayer()->updateViewer();
    }
  }
  return isModified;
}

void PartSet_CustomPrs::erasePresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                          const bool theUpdateViewer)
{
  XGUI_Workshop* aWorkshop = workshop();
  aWorkshop->displayer()->eraseAIS(myPresentations[theFlag], theUpdateViewer);
}

void PartSet_CustomPrs::clearPresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag)
{
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation(theFlag);
  if (!anOperationPrs.IsNull())
    anOperationPrs.Nullify();
}

Handle(PartSet_OperationPrs) PartSet_CustomPrs::getPresentation(
                                         const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag)
{
  Handle(PartSet_OperationPrs) aPresentation;

  if (myPresentations.contains(theFlag)) {
    AISObjectPtr anOperationPrs = myPresentations[theFlag];
    if (!anOperationPrs.get())
      initPresentation(theFlag);
    Handle(AIS_InteractiveObject) anAISIO = anOperationPrs->impl<Handle(AIS_InteractiveObject)>();
    aPresentation = Handle(PartSet_OperationPrs)::DownCast(anAISIO);
  }
  return aPresentation;
}

bool PartSet_CustomPrs::redisplay(const ObjectPtr& theObject,
                                  const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                  const bool theUpdateViewer)
{
#ifdef DO_NOT_VISUALIZE_CUSTOM_PRESENTATION
  return false;
#endif
  return displayPresentation(theFlag, theUpdateViewer);
}

void PartSet_CustomPrs::clearPrs()
{
  clearPresentation(ModuleBase_IModule::CustomizeDependedAndResults);
  clearPresentation(ModuleBase_IModule::CustomizeHighlightedObjects);
}

void PartSet_CustomPrs::initPresentation(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag)
{
  AISObjectPtr anOperationPrs = AISObjectPtr(new GeomAPI_AISObject());
  Handle(PartSet_OperationPrs) anAISPrs = new PartSet_OperationPrs(myWorkshop);
  anOperationPrs->setImpl(new Handle(AIS_InteractiveObject)(anAISPrs));
  if (theFlag == ModuleBase_IModule::CustomizeDependedAndResults) {
    Quantity_Color aShapeColor = ModuleBase_Tools::color("Visualization", "operation_parameter_color",
                                                         OPERATION_PARAMETER_COLOR());
    Quantity_Color aResultColor = ModuleBase_Tools::color("Visualization", "operation_result_color",
                                                         OPERATION_RESULT_COLOR());
    anAISPrs->setColors(aShapeColor, aResultColor);

    anOperationPrs->setPointMarker(5, 2.);
    anOperationPrs->setWidth(1);
  }
  else if (theFlag == ModuleBase_IModule::CustomizeHighlightedObjects) {
    Quantity_Color aShapeColor = ModuleBase_Tools::color("Visualization", "operation_highlight_color",
                                                         OPERATION_HIGHLIGHT_COLOR());
    Quantity_Color aResultColor = ModuleBase_Tools::color("Visualization", "operation_result_color",
                                                          OPERATION_RESULT_COLOR());
    anAISPrs->setColors(aShapeColor, aResultColor);
    //in this presentation we show the shapes wireframe similar to their highlight by OCCT
    //so, we need to use the source AIS object width for the presentation width
    anAISPrs->useAISWidth();
  }

  if (anOperationPrs.get())
    myPresentations[theFlag] = anOperationPrs;
}

XGUI_Workshop* PartSet_CustomPrs::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}
