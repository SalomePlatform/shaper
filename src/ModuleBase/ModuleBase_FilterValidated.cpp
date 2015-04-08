// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterValidated.h
// Created:     17 Mar 2015
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_FilterValidated.h"
#include "ModuleBase_IWorkshop.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_WidgetValidated.h>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_FilterValidated, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_FilterValidated, SelectMgr_Filter);

Standard_Boolean ModuleBase_FilterValidated::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  ModuleBase_Operation* anOperation = myWorkshop->module()->currentOperation();
  if (!anOperation)
    return true;

  ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
  ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
  ModuleBase_WidgetValidated* aWidgetValidated = dynamic_cast<ModuleBase_WidgetValidated*>
                                                                          (anActiveWidget);
  return !aWidgetValidated || aWidgetValidated->isValid(theOwner);
}

