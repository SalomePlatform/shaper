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

#include <Config_PropManager.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Prs3d_PointAspect.hxx>

//#define DO_NOT_VISUALIZE_CUSTOM_PRESENTATION

#define OPERATION_PARAMETER_COLOR "255, 255, 0"

PartSet_CustomPrs::PartSet_CustomPrs(ModuleBase_IWorkshop* theWorkshop)
  : myWorkshop(theWorkshop), myIsActive(false)
{
  initPrs();
}

bool PartSet_CustomPrs::isActive()
{
  return myIsActive;
  /*Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();

  return !aContext.IsNull() && aContext->IsDisplayed(anOperationPrs);*/
}

bool PartSet_CustomPrs::activate(const FeaturePtr& theFeature, const bool theUpdateViewer)
{
#ifdef DO_NOT_VISUALIZE_CUSTOM_PRESENTATION
  return false;
#endif

  bool isModified = false;
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();

  myIsActive = true;
  anOperationPrs->setFeature(theFeature);
  if (theFeature.get()) {
    displayPresentation(theUpdateViewer);
    isModified = true;
  }
  return isModified;
}

bool PartSet_CustomPrs::deactivate(const bool theUpdateViewer)
{
  myIsActive = false;
  bool isModified = false;

  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();
  anOperationPrs->setFeature(FeaturePtr());

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull() && aContext->IsDisplayed(anOperationPrs)) {
    erasePresentation(theUpdateViewer);
    isModified = true;
  }

  return isModified;
}

bool PartSet_CustomPrs::displayPresentation(const bool theUpdateViewer)
{
  bool isModified = false;
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();
  anOperationPrs->updateShapes();

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull() && !aContext->IsDisplayed(anOperationPrs)) {
    if (anOperationPrs->hasShapes()) {
      PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
      XGUI_Workshop* aWorkshop = workshop();
      aWorkshop->displayer()->displayAIS(myOperationPrs, false/*load object in selection*/, theUpdateViewer);
      aContext->SetZLayer(anOperationPrs, aModule->getVisualLayerId());
      isModified = true;
    }
  }
  else {
    if (!anOperationPrs->hasShapes()) {
      erasePresentation(theUpdateViewer);
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

void PartSet_CustomPrs::erasePresentation(const bool theUpdateViewer)
{
  XGUI_Workshop* aWorkshop = workshop();
  aWorkshop->displayer()->eraseAIS(myOperationPrs, theUpdateViewer);
}

Handle(PartSet_OperationPrs) PartSet_CustomPrs::getPresentation()
{
  if (!myOperationPrs.get())
    initPrs();
  Handle(AIS_InteractiveObject) anAISIO = myOperationPrs->impl<Handle(AIS_InteractiveObject)>();
  return Handle(PartSet_OperationPrs)::DownCast(anAISIO);
}

bool PartSet_CustomPrs::redisplay(const ObjectPtr& theObject,
                                  const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                  const bool theUpdateViewer)
{
#ifdef DO_NOT_VISUALIZE_CUSTOM_PRESENTATION
  return false;
#endif
  return displayPresentation(theUpdateViewer);
}

void PartSet_CustomPrs::clearPrs()
{
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();
  if (!anOperationPrs.IsNull())
    anOperationPrs.Nullify();

  myOperationPrs.reset();
}

void PartSet_CustomPrs::initPrs()
{
  myOperationPrs = AISObjectPtr(new GeomAPI_AISObject());
  myOperationPrs->setImpl(new Handle(AIS_InteractiveObject)(new PartSet_OperationPrs(myWorkshop)));

  std::vector<int> aColor = Config_PropManager::color("Visualization", "operation_parameter_color",
                                                      OPERATION_PARAMETER_COLOR);
  myOperationPrs->setColor(aColor[0], aColor[1], aColor[2]);

  myOperationPrs->setPointMarker(5, 2.);
  myOperationPrs->setWidth(1);
}

XGUI_Workshop* PartSet_CustomPrs::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}
