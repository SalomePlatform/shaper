// File:        XGUI_ModuleConnector.cpp
// Created:     3 June 2014
// Author:      Vitaly Smetannikov

#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"
#include "XGUI_ViewerProxy.h"
#include "XGUI_SelectionMgr.h"
#include "XGUI_Selection.h"

XGUI_ModuleConnector::XGUI_ModuleConnector(XGUI_Workshop* theWorkshop)
    : ModuleBase_IWorkshop(theWorkshop),
      myWorkshop(theWorkshop)
{
  XGUI_SelectionMgr* aSelector = myWorkshop->selector();
  connect(aSelector, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
}

XGUI_ModuleConnector::~XGUI_ModuleConnector()
{
}

Handle(AIS_InteractiveContext) XGUI_ModuleConnector::AISContext() const
{
  return myWorkshop->viewer()->AISContext();
}

QList<ObjectPtr> XGUI_ModuleConnector::selectedObjects() const
{
  return myWorkshop->selector()->selection()->selectedObjects();
}

ModuleBase_IModule* XGUI_ModuleConnector::module() const
{
  return myWorkshop->module();
}
