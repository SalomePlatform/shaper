// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include <ModuleBase_WidgetValidator.h>
#include <ModuleBase_WidgetSelectorStore.h>
#include <ModuleBase_WidgetValidated.h>

#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IWorkshop.h>

ModuleBase_WidgetValidator::ModuleBase_WidgetValidator(ModuleBase_ModelWidget* theModelWidget,
                                                       ModuleBase_IWorkshop* theWorkshop)
: myModelWidget(theModelWidget), myWorkshop(theWorkshop), myIsInValidate(false)
{
  myAttributeStore = new ModuleBase_WidgetSelectorStore();
}

ModuleBase_WidgetValidator::~ModuleBase_WidgetValidator()
{
  delete myAttributeStore;
}

//********************************************************************
bool ModuleBase_WidgetValidator::isValidSelection(const ModuleBase_ViewerPrsPtr& theValue)
{
  bool aValid = false;
  if (getValidState(theValue, aValid))
    return aValid;

  aValid = myModelWidget->isValidSelectionCustom(theValue);

  storeValidState(theValue, aValid);
  return aValid;
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
    clearValidatedCash();
  }

  return aHasSelectionFilter;
}

void ModuleBase_WidgetValidator::storeAttributeValue(const AttributePtr& theAttribute)
{
  myIsInValidate = true;
  myAttributeStore->storeAttributeValue(theAttribute, myWorkshop);
}

void ModuleBase_WidgetValidator::restoreAttributeValue(const AttributePtr& theAttribute,
                                                       const bool theValid)
{
  myIsInValidate = false;
  myAttributeStore->restoreAttributeValue(theAttribute, myWorkshop);
}

bool ModuleBase_WidgetValidator::isValidAttribute(const AttributePtr& theAttribute) const
{
  return ModuleBase_WidgetValidated::isValidAttribute(theAttribute);
}

bool ModuleBase_WidgetValidator::isFilterActivated() const
{
  bool isActivated = false;

  Handle(SelectMgr_Filter) aSelFilter = myWorkshop->validatorFilter();
  ModuleBase_IViewer* aViewer = myWorkshop->viewer();

  return aViewer->hasSelectionFilter(aSelFilter);
}

void ModuleBase_WidgetValidator::clearValidatedCash()
{
  myValidPrs.clear();
  myInvalidPrs.clear();
}

bool ModuleBase_WidgetValidator::getValidState(const ModuleBase_ViewerPrsPtr& theValue,
                                               bool& theValid)
{
  bool aValidPrs = myValidPrs.contains(theValue);
  bool anInvalidPrs = myInvalidPrs.contains(theValue);

  if (aValidPrs)
    theValid = true;
  else if (anInvalidPrs)
    theValid = false;

  return aValidPrs || anInvalidPrs;
}

//********************************************************************
void ModuleBase_WidgetValidator::storeValidState(const ModuleBase_ViewerPrsPtr& theValue,
                                                 const bool theValid)
{
  bool aValidPrs = myInvalidPrs.contains(theValue);
  bool anInvalidPrs = myInvalidPrs.contains(theValue);

  if (theValid) {
    if (!aValidPrs)
      myValidPrs.append(theValue);
    // the commented code will be useful when the valid state of the presentation
    // will be changable between activate/deactivate. Currently it does not happen.
    //if (anInvalidPrs)
    //  myInvalidPrs.removeOne(theValue);
  }
  else { // !theValid
    if (!anInvalidPrs)
      myInvalidPrs.append(theValue);
    //if (!aValidPrs)
    //  myValidPrs.removeOne(theValue);
  }
#ifdef DEBUG_VALID_STATE
  qDebug(QString("storeValidState: myValidPrs.size() = %1, myInvalidPrs.size() = %2")
                 .arg(myValidPrs.count())
                 .arg(myInvalidPrs.count()).toStdString().c_str());
#endif
}
