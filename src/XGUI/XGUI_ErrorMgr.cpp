// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_ErrorMgr.cpp
// Created:     22 July 2015
// Author:      Sergey POKHODENKO

#include "XGUI_ErrorMgr.h"

#include "XGUI_OperationMgr.h"
#include "XGUI_ModuleConnector.h"
#include "XGUI_Workshop.h"
#include "XGUI_ActionsMgr.h"

#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IModule.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_OperationFeature.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <QLabel>
#include <QAction>
#include <QApplication>
#include <QDesktopWidget>
#include <QDialog>
#include <QCursor>
#include <QHBoxLayout>
#include <QLabel>

const QString INVALID_VALUE = "invalid_action";


XGUI_ErrorMgr::XGUI_ErrorMgr(QObject* theParent, ModuleBase_IWorkshop* theWorkshop)
  : ModuleBase_IErrorMgr(theParent),
    myErrorDialog(0),
    myErrorLabel(0),
    myWorkshop(theWorkshop)
{
  ModuleBase_ModelWidget* anActiveWidget = activeWidget();
  XGUI_ActionsMgr* anActionsMgr = workshop()->actionsMgr();
  QAction* anAcceptAllAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptAll, NULL);
  myAcceptAllToolTip = anAcceptAllAction->toolTip();

  QAction* anOkAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::Accept);
  myAcceptToolTip = anOkAction->toolTip();
}

XGUI_ErrorMgr::~XGUI_ErrorMgr()
{

}

void XGUI_ErrorMgr::updateActions(const FeaturePtr& theFeature)
{
  //update Ok Action and header of property panel if the current operation started for the feature
  XGUI_ActionsMgr* anActionsMgr = workshop()->actionsMgr();
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                      (workshop()->operationMgr()->currentOperation());
  if (aFOperation && aFOperation->feature() == theFeature) {
    QAction* anOkAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::Accept);
    
    ModuleBase_ModelWidget* anActiveWidget = activeWidget();
    bool isApplyEnabledByActiveWidget = false;
    if (anActiveWidget)
      isApplyEnabledByActiveWidget = anActiveWidget->getValueState() ==
                                     ModuleBase_ModelWidget::ModifiedInPP;
    QString anError = "";
    QString aWidgetError = "";
    if (!isApplyEnabledByActiveWidget) {
      anError = myWorkshop->module()->getFeatureError(theFeature);
      if (anActiveWidget)
        aWidgetError = anActiveWidget->getError();
      if (anError.isEmpty())
        anError = aWidgetError;
    }
    updateActionState(anOkAction, anError);
    updateToolTip(anActiveWidget, aWidgetError);
  }
}

void XGUI_ErrorMgr::updateAcceptAllAction(const FeaturePtr& theFeature)
{
  QString anError = myWorkshop->module()->getFeatureError(theFeature);
  if (anError.isEmpty()) {
    ModuleBase_ModelWidget* anActiveWidget = activeWidget();
    if (anActiveWidget)
      anError = anActiveWidget->getError();
  }
  XGUI_ActionsMgr* anActionsMgr = workshop()->actionsMgr();
  if (workshop()->isFeatureOfNested(theFeature)) {
    QAction* anAcceptAllAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptAll, NULL);
    bool anEnabled = anError.isEmpty();
    anAcceptAllAction->setEnabled(anEnabled);
    anAcceptAllAction->setToolTip(!anEnabled ? anError : myAcceptAllToolTip);
  }
}

bool XGUI_ErrorMgr::isApplyEnabled() const
{
  bool isEnabled = false;
  XGUI_ActionsMgr* anActionsMgr = workshop()->actionsMgr();
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                      (workshop()->operationMgr()->currentOperation());
  if (aFOperation) {
    QAction* anOkAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::Accept);
    isEnabled = anOkAction && anOkAction->isEnabled();
  }
  return isEnabled;
}

void XGUI_ErrorMgr::updateActionState(QAction* theAction, const QString& theError)
{
  bool anEnabled = theError.isEmpty();

  theAction->setEnabled(anEnabled);
  theAction->setToolTip(anEnabled ? myAcceptToolTip : theError);
  // some operations have no property panel, so it is important to check that it is not null
  if (myPropertyPanel) {
    // update controls error information
    QWidget* aWidget = myPropertyPanel->headerWidget();
    if (aWidget)
      aWidget->setToolTip(theError);
  }
}
void XGUI_ErrorMgr::onWidgetChanged()
{
  ModuleBase_ModelWidget* aModelWidget = dynamic_cast<ModuleBase_ModelWidget*>(sender());
  if (!aModelWidget || !aModelWidget->feature().get())
    return;

  QString aWidgetError = aModelWidget->getError();
  updateToolTip(aModelWidget, aWidgetError);
}

void XGUI_ErrorMgr::updateToolTip(ModuleBase_ModelWidget* theWidget,
                                  const QString& theError)
{
  if (!theWidget)
    return;

  QList<QWidget*> aWidgetList = theWidget->getControls();
  foreach(QWidget* aWidget, aWidgetList) {
    QLabel* aLabel = qobject_cast<QLabel*>(aWidget);
    // We won't set the effect to QLabels - it looks ugly
    if (aLabel) continue;

    // Get the original tool tip of the widget
    QString aTTip = aWidget->toolTip().section("Errors:\n", 0, 0).trimmed();
    // Add the error message into the tool tip
    if (!theError.isEmpty()) {
      if (!aTTip.isEmpty())
        aTTip.append('\n');
      aTTip += "Errors:\n" + theError;
    }
    aWidget->setToolTip(aTTip);
    //aWidget->setStyleSheet(anError.isEmpty() ? "" : "background-color:pink;");
  }
}

XGUI_Workshop* XGUI_ErrorMgr::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}

ModuleBase_ModelWidget* XGUI_ErrorMgr::activeWidget() const
{
  ModuleBase_ModelWidget* anActiveWidget = 0;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                      (workshop()->operationMgr()->currentOperation());
  if (aFOperation) {
    ModuleBase_IPropertyPanel* aPropertyPanel = aFOperation->propertyPanel();
    if (aPropertyPanel) {
      anActiveWidget = aPropertyPanel->activeWidget();
    }
  }
  return anActiveWidget;
}

