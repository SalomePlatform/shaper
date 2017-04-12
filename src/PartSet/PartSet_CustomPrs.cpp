// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_CustomPrs.cpp
// Created:     30 Jun 2015
// Author:      Natalia ERMOLAEVA

#include <PartSet_CustomPrs.h>
#include <PartSet_Module.h>
#include "PartSet_OperationPrs.h"
#include "PartSet_OverconstraintListener.h"

#include <XGUI_ModuleConnector.h>
#include <XGUI_Workshop.h>
#include <XGUI_Displayer.h>

#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_Tools.h>

#include <Config_PropManager.h>
#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <AIS_InteractiveContext.hxx>
#include <AIS_InteractiveObject.hxx>
#include <Prs3d_PointAspect.hxx>

//#define DO_NOT_VISUALIZE_CUSTOM_PRESENTATION

PartSet_CustomPrs::PartSet_CustomPrs(ModuleBase_IWorkshop* theWorkshop)
  : myWorkshop(theWorkshop), myFeature(FeaturePtr()), myPresentationIsEmpty(false)
{
  Events_Loop* aLoop = Events_Loop::loop();
  aLoop->registerListener(this, Events_Loop::eventByName(EVENT_EMPTY_OPERATION_PRESENTATION));

  initPresentation(ModuleBase_IModule::CustomizeArguments);
  initPresentation(ModuleBase_IModule::CustomizeResults);
  initPresentation(ModuleBase_IModule::CustomizeHighlightedObjects);

  myIsActive[ModuleBase_IModule::CustomizeArguments] = false;
  myIsActive[ModuleBase_IModule::CustomizeResults] = false;
  myIsActive[ModuleBase_IModule::CustomizeHighlightedObjects] = false;
}

bool PartSet_CustomPrs::isActive(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag)
{
  return myIsActive[theFlag];
}

bool PartSet_CustomPrs::activate(const FeaturePtr& theFeature,
                                 const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                 const bool theUpdateViewer)
{
#ifdef DO_NOT_VISUALIZE_CUSTOM_PRESENTATION
  return false;
#endif

  myIsActive[theFlag] = true;
  myFeature = theFeature;

  bool isModified = false;
  if (theFeature.get()) {
    displayPresentation(theFlag, theUpdateViewer);
    isModified = true;
  }
  return isModified;
}

bool PartSet_CustomPrs::deactivate(const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                   const bool theUpdateViewer)
{
  myIsActive[theFlag] = false;
  erasePresentation(theFlag, theUpdateViewer);
  return true;
}

bool PartSet_CustomPrs::displayPresentation(
                                  const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                  const bool theUpdateViewer)
{
  bool isModified = false;

  // update the AIS objects content
  AISObjectPtr aPresentation = getPresentation(theFlag, true);
  Handle(AIS_InteractiveObject) anAISIO = aPresentation->impl<Handle(AIS_InteractiveObject)>();
  Handle(PartSet_OperationPrs) anOperationPrs = Handle(PartSet_OperationPrs)::DownCast(anAISIO);

  // do nothing if the feature can not be displayed [is moved from presentation, to be checked]
  if (!myFeature.get())
    return isModified;

  QMap<ObjectPtr, QList<GeomShapePtr> > aFeatureShapes;
  switch (theFlag) {
    case ModuleBase_IModule::CustomizeArguments:
      PartSet_OperationPrs::getFeatureShapes(myFeature, myWorkshop, aFeatureShapes);
      break;
    case ModuleBase_IModule::CustomizeResults:
      PartSet_OperationPrs::getResultShapes(myFeature, myWorkshop, aFeatureShapes);
      PartSet_OperationPrs::getPresentationShapes(myFeature, myWorkshop, aFeatureShapes, false);
      break;
    case ModuleBase_IModule::CustomizeHighlightedObjects:
      PartSet_OperationPrs::getHighlightedShapes(myWorkshop, aFeatureShapes);
      break;
    default:
      return isModified;
  }
  NCollection_DataMap<TopoDS_Shape, Handle_AIS_InteractiveObject>& aShapeMap =
                                                                 anOperationPrs->shapesMap();
  PartSet_OperationPrs::fillShapeList(aFeatureShapes, myWorkshop, aShapeMap);

  myPresentationIsEmpty = false;
  // redisplay AIS objects
  bool aRedisplayed = false;
  Handle(AIS_InteractiveContext) aContext = myWorkshop->viewer()->AISContext();
  if (!aContext.IsNull() && !aContext->IsDisplayed(anOperationPrs)) {
    // when the feature can not be visualized in the module, the operation preview should not
    // be visualized also
    if (anOperationPrs->hasShapes() && myWorkshop->module()->canDisplayObject(myFeature)) {
      // set color here because it can be changed in preferences
      Quantity_Color aShapeColor = getShapeColor(theFlag);
      anOperationPrs->setShapeColor(aShapeColor);

      PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
      XGUI_Workshop* aWorkshop = workshop();
      aRedisplayed = aWorkshop->displayer()->displayAIS(myPresentations[theFlag],
                                         false/*load object in selection*/, false);
      aContext->SetZLayer(anOperationPrs, aModule->getVisualLayerId());
      isModified = true;
    }
  }
  else {
    // when the feature can not be visualized in the module, the operation preview should not
    // be visualized also
    if (!anOperationPrs->hasShapes() || !myWorkshop->module()->canDisplayObject(myFeature)) {
      aRedisplayed = erasePresentation(theFlag, false);
      isModified = true;
    }
    else {
      anOperationPrs->Redisplay();
      isModified = true;
      aRedisplayed = true;
    }
  }
  if (myPresentationIsEmpty) {
    aRedisplayed = erasePresentation(theFlag, false);
  }
  if (aRedisplayed && theUpdateViewer)
    workshop()->displayer()->updateViewer();

  return isModified;
}

bool PartSet_CustomPrs::erasePresentation(
                          const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                          const bool theUpdateViewer)
{
  bool isErased = false;
  XGUI_Workshop* aWorkshop = workshop();
  if (myPresentations.contains(theFlag))
    isErased = aWorkshop->displayer()->eraseAIS(myPresentations[theFlag], theUpdateViewer);
  return isErased;
}

void PartSet_CustomPrs::clearPresentation(
  const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag)
{
  AISObjectPtr aPresentation = getPresentation(theFlag, false);
  if (aPresentation.get()) {
    Handle(AIS_InteractiveObject) anAISIO = aPresentation->impl<Handle(AIS_InteractiveObject)>();
    Handle(PartSet_OperationPrs) anOperationPrs = Handle(PartSet_OperationPrs)::DownCast(anAISIO);

    anOperationPrs->shapesMap().Clear();
    if (!anOperationPrs.IsNull())
      anOperationPrs.Nullify();
    myPresentations.remove(theFlag);
  }
}

AISObjectPtr PartSet_CustomPrs::getPresentation(
                                       const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                       const bool theToCreate)
{
  Handle(PartSet_OperationPrs) aPresentation;

  AISObjectPtr anOperationPrs;
  if (myPresentations.contains(theFlag))
    anOperationPrs = myPresentations[theFlag];

  if (!anOperationPrs.get() && theToCreate) {
    initPresentation(theFlag);
    anOperationPrs = myPresentations[theFlag];
  }

  return anOperationPrs;
}

bool PartSet_CustomPrs::redisplay(const ObjectPtr& theObject,
                                  const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag,
                                  const bool theUpdateViewer)
{
#ifdef DO_NOT_VISUALIZE_CUSTOM_PRESENTATION
  return false;
#endif
  bool aRedisplayed = false;
  if (myIsActive[theFlag])
    aRedisplayed = displayPresentation(theFlag, theUpdateViewer);

  return aRedisplayed;
}

void PartSet_CustomPrs::clearPrs()
{
  clearPresentation(ModuleBase_IModule::CustomizeArguments);
  clearPresentation(ModuleBase_IModule::CustomizeResults);
  clearPresentation(ModuleBase_IModule::CustomizeHighlightedObjects);
}

void PartSet_CustomPrs::processEvent(const std::shared_ptr<Events_Message>& theMessage)
{
  if (theMessage->eventID() == Events_Loop::eventByName(EVENT_EMPTY_OPERATION_PRESENTATION))
    myPresentationIsEmpty = true; /// store state to analize it after display/erase is finished
}

void PartSet_CustomPrs::initPresentation(
  const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag)
{
  AISObjectPtr anOperationPrs = AISObjectPtr(new GeomAPI_AISObject());
  Handle(PartSet_OperationPrs) anAISPrs = new PartSet_OperationPrs(myWorkshop);
  anOperationPrs->setImpl(new Handle(AIS_InteractiveObject)(anAISPrs));
  if (theFlag == ModuleBase_IModule::CustomizeArguments ||
      theFlag == ModuleBase_IModule::CustomizeResults) {
    anOperationPrs->setPointMarker(5, 2.);
    anOperationPrs->setWidth(1);
  }
  else if (theFlag == ModuleBase_IModule::CustomizeHighlightedObjects)
    anAISPrs->useAISWidth();

  if (anOperationPrs.get())
    myPresentations[theFlag] = anOperationPrs;
}

Quantity_Color PartSet_CustomPrs::getShapeColor(
                                  const ModuleBase_IModule::ModuleBase_CustomizeFlag& theFlag)
{
  Quantity_Color aColor;
  switch(theFlag) {
    case ModuleBase_IModule::CustomizeArguments:
      aColor = ModuleBase_Tools::color("Visualization", "operation_parameter_color");
    break;
    case ModuleBase_IModule::CustomizeResults:
      aColor = ModuleBase_Tools::color("Visualization", "operation_result_color");
    break;
    case ModuleBase_IModule::CustomizeHighlightedObjects:
      aColor = ModuleBase_Tools::color("Visualization", "operation_highlight_color");
    break;
    default:
    break;
  }
  return aColor;
}

XGUI_Workshop* PartSet_CustomPrs::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}
