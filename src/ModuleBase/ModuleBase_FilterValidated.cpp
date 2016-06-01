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
#include <ModuleBase_Tools.h>

IMPLEMENT_STANDARD_HANDLE(ModuleBase_FilterValidated, SelectMgr_Filter);
IMPLEMENT_STANDARD_RTTIEXT(ModuleBase_FilterValidated, SelectMgr_Filter);

//#define DEBUG_CURRENT_SELECTION

Standard_Boolean ModuleBase_FilterValidated::IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const
{
  bool aValid = true;
  ModuleBase_Operation* anOperation = myWorkshop->module()->currentOperation();
  if (anOperation) {
#ifdef DEBUG_CURRENT_SELECTION
    QString aPrefix = "ModuleBase_FilterValidated::IsOk: ";
    QList<ModuleBase_ViewerPrsPtr> aSelected = myWorkshop->selection()->getSelected(ModuleBase_ISelection::Viewer);
    QList<ModuleBase_ViewerPrsPtr>::const_iterator anIt = aSelected.begin(), aLast = aSelected.end();
    QStringList anInfo;
    ModuleBase_ViewerPrsPtr aPrs;
    for (; anIt != aLast; anIt++) {
      aPrs = (*anIt);
      if (!aPrs.get())
        continue;

      //GeomShapePtr aShape = aPrs->shape();
      ObjectPtr anObject = aPrs->object();
      QString anObjectInfo = anObject.get() ? ModuleBase_Tools::objectInfo(anObject) : "";
      anInfo.append(anObjectInfo);
    }
    QString anInfoStr = anInfo.join(";\t");
    qDebug(QString("%1: %2, %3").arg(aPrefix).arg(anInfo.size()).arg(anInfoStr).toStdString().c_str());
#endif

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

