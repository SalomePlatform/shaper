// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_FilterValidated.cpp
// Created:     17 Mar 2015
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_FilterValidated.h"
#include "ModuleBase_IWorkshop.h"

#include <ModuleBase_IModule.h>
#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_WidgetValidated.h>
#include <ModuleBase_WidgetValidator.h>
#include <ModuleBase_ViewerPrs.h>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_FilterValidated, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_FilterValidated, SelectMgr_Filter);

Standard_Boolean ModuleBase_FilterValidated::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  bool aValid = true;
  ModuleBase_Operation* anOperation = myWorkshop->module()->currentOperation();
  if (anOperation) {
    ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
    ModuleBase_ModelWidget* aCurrentWidget = aPanel->preselectionWidget();
    if (!aCurrentWidget)
      aCurrentWidget = myWorkshop->module()->activeWidget();
    if (aCurrentWidget) {
      ModuleBase_ViewerPrsPtr aPrs(new ModuleBase_ViewerPrs());
      myWorkshop->selection()->fillPresentation(aPrs, theOwner);
      ModuleBase_WidgetValidated* aWidgetValidated = dynamic_cast<ModuleBase_WidgetValidated*>
                                                                             (aCurrentWidget);
      if (aWidgetValidated)
        aValid = !aWidgetValidated || aWidgetValidated->isValidSelection(aPrs);
      else{
        ModuleBase_WidgetValidator* aWidgetValidator = aCurrentWidget->widgetValidator();
        if (aWidgetValidator)
          aValid = aWidgetValidator->isValidSelection(aPrs);
      }
    }
  }

#ifdef DEBUG_FILTERS
  qDebug(QString("ModuleBase_FilterValidated::IsOk = %1").arg(aValid).toStdString().c_str());
#endif
  return aValid;
}

