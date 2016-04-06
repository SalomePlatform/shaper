// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_ModuleConnector.cpp
// Created:     3 June 2014
// Author:      Vitaly Smetannikov

#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Selection.h"
#include "XGUI_OperationMgr.h"
#include "XGUI_Displayer.h"
#include "XGUI_PropertyPanel.h"

#include <ModuleBase_IModule.h>

#include <AIS_Shape.hxx>


XGUI_ModuleConnector::XGUI_ModuleConnector(XGUI_Workshop* theWorkshop)
    : ModuleBase_IWorkshop(theWorkshop),
      myWorkshop(theWorkshop)
{
  XGUI_SelectionMgr* aSelector = myWorkshop->selector();
  connect(aSelector, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
  
  XGUI_OperationMgr* anOperationMgr = myWorkshop->operationMgr();
}

XGUI_ModuleConnector::~XGUI_ModuleConnector()
{
}

ModuleBase_ISelection* XGUI_ModuleConnector::selection() const
{
  return myWorkshop->selector()->selection();
}

ModuleBase_IModule* XGUI_ModuleConnector::module() const
{
  return myWorkshop->module();
}

ModuleBase_IViewer* XGUI_ModuleConnector::viewer() const
{
  return myWorkshop->viewer();
}

ModuleBase_IPropertyPanel* XGUI_ModuleConnector::propertyPanel() const
{
  return myWorkshop->propertyPanel();
}

ModuleBase_Operation* XGUI_ModuleConnector::currentOperation() const
{
  return myWorkshop->operationMgr()->currentOperation();
}


QObjectPtrList XGUI_ModuleConnector::activeObjects(const QObjectPtrList& theObjList) const
{
  QObjectPtrList aActiveOPbjects;
  ModuleBase_IModule* aModule = myWorkshop->module();
  // Activate objects only which can be activated
  foreach (ObjectPtr aObj, theObjList) {
    if (aModule->canActivateSelection(aObj))
      aActiveOPbjects.append(aObj);
  }
  return aActiveOPbjects;
}

void XGUI_ModuleConnector::activateSubShapesSelection(const QIntList& theTypes)
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  aDisp->activateObjects(theTypes, activeObjects(aDisp->displayedObjects()));
}

void XGUI_ModuleConnector::deactivateSubShapesSelection()
{
  // Clear selection modes
  activateModuleSelectionModes();
}

void XGUI_ModuleConnector::activateModuleSelectionModes()
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  myWorkshop->activateObjectsSelection(activeObjects(aDisp->displayedObjects()));
}

AISObjectPtr XGUI_ModuleConnector::findPresentation(const ObjectPtr& theObject) const
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  return aDisp->getAISObject(theObject);
}

ObjectPtr XGUI_ModuleConnector::findPresentedObject(const AISObjectPtr& theAIS) const
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  ObjectPtr anObject = aDisp->getObject(theAIS);
  return anObject;
}

void XGUI_ModuleConnector::setSelected(const QList<ModuleBase_ViewerPrsPtr>& theValues)
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  if (theValues.isEmpty()) {
    myWorkshop->selector()->clearSelection();
  } else {
    aDisp->setSelected(theValues);
  }    
}

bool XGUI_ModuleConnector::canStartOperation(QString theId)
{
  return myWorkshop->operationMgr()->canStartOperation(theId);
}

ModuleBase_Operation* XGUI_ModuleConnector::findStartedOperation(const QString& theId)
{
  return myWorkshop->operationMgr()->findOperation(theId);
}

bool XGUI_ModuleConnector::canStopOperation(ModuleBase_Operation* theOperation)
{
  return myWorkshop->operationMgr()->canStopOperation(theOperation);
}

void XGUI_ModuleConnector::abortOperation(ModuleBase_Operation* theOperation)
{
  myWorkshop->operationMgr()->abortOperation(theOperation);
}

void XGUI_ModuleConnector::updateCommandStatus()
{
  myWorkshop->updateCommandStatus();
}