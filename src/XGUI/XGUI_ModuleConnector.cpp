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

XGUI_ModuleConnector::XGUI_ModuleConnector(XGUI_Workshop* theWorkshop)
    : ModuleBase_IWorkshop(theWorkshop),
      myWorkshop(theWorkshop)
{
  XGUI_SelectionMgr* aSelector = myWorkshop->selector();
  connect(aSelector, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
  
  XGUI_OperationMgr* anOperationMgr = myWorkshop->operationMgr();

  connect(anOperationMgr, SIGNAL(operationStarted(ModuleBase_Operation*)), 
    this, SIGNAL(operationStarted(ModuleBase_Operation*)));
  connect(anOperationMgr, SIGNAL(operationStopped(ModuleBase_Operation*)), 
    this, SIGNAL(operationStopped(ModuleBase_Operation*)));
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

ModuleBase_Operation* XGUI_ModuleConnector::currentOperation() const
{
  return myWorkshop->operationMgr()->currentOperation();
}


void XGUI_ModuleConnector::activateSubShapesSelection(const QIntList& theTypes)
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  aDisp->openLocalContext();
  aDisp->activateObjectsOutOfContext();
  aDisp->setSelectionModes(theTypes);
}

void XGUI_ModuleConnector::deactivateSubShapesSelection()
{
  XGUI_Displayer* aDisp = myWorkshop->displayer();
  aDisp->closeLocalContexts(false);
}
