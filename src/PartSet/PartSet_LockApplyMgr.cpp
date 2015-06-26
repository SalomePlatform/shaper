// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_LockApplyMgr.cpp
// Created:     25 Jun 2015
// Author:      Natalia Ermolaeva

#include "PartSet_LockApplyMgr.h"
#include "PartSet_Module.h"

#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewer.h>

#include <XGUI_Workshop.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_OperationMgr.h>

PartSet_LockApplyMgr::PartSet_LockApplyMgr(QObject* theParent,
                                           ModuleBase_IWorkshop* theWorkshop)
: QObject(theParent), myWorkshop(theWorkshop)
{
}

void PartSet_LockApplyMgr::activate()
{
  XGUI_ViewerProxy* aViewer = dynamic_cast<XGUI_ViewerProxy*>(myWorkshop->viewer());
  connect(aViewer, SIGNAL(enterViewPort()), this, SLOT(onLockValidating()));
  connect(aViewer, SIGNAL(leaveViewPort()), this, SLOT(onUnlockValidating()));

  PartSet_Module* aModule = dynamic_cast<PartSet_Module*>(myWorkshop->module());
  if (aModule->isMouseOverWindow())
    onLockValidating();
}

void PartSet_LockApplyMgr::deactivate()
{
  XGUI_ViewerProxy* aViewer = dynamic_cast<XGUI_ViewerProxy*>(myWorkshop->viewer());
  disconnect(aViewer, SIGNAL(enterViewPort()), this, SLOT(onLockValidating()));
  disconnect(aViewer, SIGNAL(leaveViewPort()), this, SLOT(onUnlockValidating()));

  onUnlockValidating();
}

void PartSet_LockApplyMgr::valuesChanged()
{
  operationMgr()->setLockValidating(false);
}

void PartSet_LockApplyMgr::onLockValidating()
{
  XGUI_OperationMgr* anOperationMgr = operationMgr();

  anOperationMgr->setLockValidating(true);
  // the Ok button should be disabled in the property panel by moving the mouse point in the viewer
  // this leads that the user does not try to click Ok and it avoids an incorrect situation that the
  // line is moved to the cursor to the Ok button
  //anOperationMgr->setApplyEnabled(false);
}

void PartSet_LockApplyMgr::onUnlockValidating()
{
  XGUI_OperationMgr* anOperationMgr = operationMgr();

  // it is important to restore the validity state in the property panel after leaving the
  // view port. Unlock the validating.
  if (anOperationMgr->isValidationLocked()) {
    anOperationMgr->setLockValidating(false);
    //anOperationMgr->onValidateOperation();
  }
}

XGUI_OperationMgr* PartSet_LockApplyMgr::operationMgr() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();

  return aWorkshop->operationMgr();
}
