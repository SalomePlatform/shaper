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

#include <GeomValidators_Tools.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_AttributeSelectionList.h>

#include <Config_PropManager.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Prs3d_PointAspect.hxx>

#define OPERATION_PARAMETER_COLOR "255, 255, 0"

PartSet_CustomPrs::PartSet_CustomPrs(ModuleBase_IWorkshop* theWorkshop)
  : myWorkshop(theWorkshop)
{
  myOperationPrs = AISObjectPtr(new GeomAPI_AISObject());
  myOperationPrs->setImpl(new Handle(AIS_InteractiveObject)(new PartSet_OperationPrs(theWorkshop)));

  std::vector<int> aColor = Config_PropManager::color("Visualization", "operation_parameter_color",
                                                      OPERATION_PARAMETER_COLOR);
  myOperationPrs->setColor(aColor[0], aColor[1], aColor[2]);

  myOperationPrs->setPointMarker(5, 2.);
  myOperationPrs->setWidth(1);
}

bool PartSet_CustomPrs::isActive() const
{
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();

  return aContext->IsDisplayed(anOperationPrs);
}

void PartSet_CustomPrs::activate(const FeaturePtr& theFeature)
{
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();

  if (anOperationPrs->canActivate(theFeature)) {
    anOperationPrs->setFeature(theFeature);
    if (theFeature.get())
      displayPresentation();
  }
}

void PartSet_CustomPrs::deactivate()
{
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();
  anOperationPrs->setFeature(FeaturePtr());

  erasePresentation();
}


void PartSet_CustomPrs::displayPresentation()
{
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();

  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext->IsDisplayed(anOperationPrs)) {
    PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
    aContext->Display(anOperationPrs);
    aContext->SetZLayer(anOperationPrs, aModule->getVisualLayerId());
  }
  else
    anOperationPrs->Redisplay();
}

void PartSet_CustomPrs::erasePresentation()
{
  Handle(AIS_InteractiveObject) anOperationPrs = myOperationPrs->impl<Handle(AIS_InteractiveObject)>();
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext->IsDisplayed(anOperationPrs))
    aContext->Remove(anOperationPrs);
}

Handle(PartSet_OperationPrs) PartSet_CustomPrs::getPresentation() const
{
  Handle(AIS_InteractiveObject) anAISIO = myOperationPrs->impl<Handle(AIS_InteractiveObject)>();
  return Handle(PartSet_OperationPrs)::DownCast(anAISIO);
}

bool PartSet_CustomPrs::customize(const ObjectPtr& theObject)
{
  // the presentation should be recomputed if the previous AIS depend on the result
  // [it should be hiddend] or the new AIS depend on it [it should be visualized]
  Handle(PartSet_OperationPrs) anOperationPrs = getPresentation();
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (aContext->IsDisplayed(anOperationPrs)) {
    bool aChanged = anOperationPrs->dependOn(theObject);

    anOperationPrs->updateShapes();
    aChanged = aChanged || anOperationPrs->dependOn(theObject);

    //if (aChanged)
    anOperationPrs->Redisplay();
  }
  return false;
}

bool PartSet_CustomPrs::customisePresentation(ResultPtr theResult, AISObjectPtr thePrs,
                                              std::shared_ptr<GeomAPI_ICustomPrs> theCustomPrs)
{
  return false;
}
