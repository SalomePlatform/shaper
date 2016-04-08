// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <ModuleBase_WidgetValidator.h>

#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IWorkshop.h>

ModuleBase_WidgetValidator::ModuleBase_WidgetValidator(ModuleBase_ModelWidget* theModelWidget,
                                                       ModuleBase_IWorkshop* theWorkshop)
: myModelWidget(theModelWidget), myWorkshop(theWorkshop)
{
}

ModuleBase_WidgetValidator::~ModuleBase_WidgetValidator()
{
}

//********************************************************************
bool ModuleBase_WidgetValidator::isValidSelection(const ModuleBase_ViewerPrsPtr& theValue)
{
  return myModelWidget->isValidSelectionCustom(theValue);
}

bool ModuleBase_WidgetValidator::isFilterActivated() const
{
  bool isActivated = false;

  Handle(SelectMgr_Filter) aSelFilter = myWorkshop->validatorFilter();
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();

  return aViewer->hasSelectionFilter(aSelFilter);
}

bool ModuleBase_WidgetValidator::activateFilters(const bool toActivate)
{
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();

  Handle(SelectMgr_Filter) aSelFilter = myWorkshop->validatorFilter();
  bool aHasSelectionFilter = aViewer->hasSelectionFilter(aSelFilter);

  if (toActivate)
    aViewer->addSelectionFilter(aSelFilter);
  else {
    aViewer->removeSelectionFilter(aSelFilter);
    //clearValidatedCash();
  }

  return aHasSelectionFilter;
}
