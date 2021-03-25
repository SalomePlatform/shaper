// Copyright (C) 2014-2021  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

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

#include <ModuleBase_Tools.h>
#include <QDebug>

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

//#define DEBUG_ERROR_STATE

XGUI_ErrorMgr::XGUI_ErrorMgr(QObject* theParent, ModuleBase_IWorkshop* theWorkshop)
  : ModuleBase_IErrorMgr(theParent),
    myWorkshop(theWorkshop),
    myErrorDialog(0),
    myErrorLabel(0),
    myAcceptToolTip(""),
    myAcceptAllToolTip(""),
    myAcceptStatusTip(""),
    myAcceptAllStatusTip("")
{
}

XGUI_ErrorMgr::~XGUI_ErrorMgr()
{
}

void XGUI_ErrorMgr::updateActions(const FeaturePtr& theFeature)
{
  //update Ok Action and header of property panel if the current operation started for the feature
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                      (workshop()->operationMgr()->currentOperation());
  if (aFOperation && aFOperation->feature() == theFeature) {
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
    updateAcceptActionState(anError);
    updateToolTip(anActiveWidget, aWidgetError);
    myWorkshop->setStatusBarMessage(anError);

#ifdef DEBUG_ERROR_STATE
    QString anInfo = ModuleBase_Tools::objectInfo(theFeature);

    QString aResultInfo = QString("valid = %1, anError = %2, aWidgetError = %3")
                          .arg(anError.isEmpty()).arg(anError).arg(aWidgetError);
    qDebug(QString("XGUI_ErrorMgr::updateActions for %1, result: %2").arg(anInfo)
                  .arg(aResultInfo).toStdString().c_str());
#endif
  }
}

void XGUI_ErrorMgr::updateAcceptAllAction(const FeaturePtr& theFeature)
{
  if (myAcceptAllToolTip.isEmpty() && myAcceptToolTip.isEmpty())
    storeInitialActionValues();

  QString anError = "";
  /// to allow the module have the button always enabled
  bool isActionStateEnabled =
    myWorkshop->module()->isActionEnableStateFixed(XGUI_ActionsMgr::AcceptAll);
  if (!isActionStateEnabled) {
    anError = myWorkshop->module()->getFeatureError(theFeature);
    if (anError.isEmpty()) {
      ModuleBase_ModelWidget* anActiveWidget = activeWidget();
      if (anActiveWidget)
        anError = anActiveWidget->getError();
    }
  }
  XGUI_ActionsMgr* anActionsMgr = workshop()->actionsMgr();
  if (workshop()->isFeatureOfNested(theFeature)) {
    QAction* anAcceptAllAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptAll);
    bool anEnabled = anError.isEmpty();
    anAcceptAllAction->setEnabled(anEnabled);
    anAcceptAllAction->setToolTip(!anEnabled ? anError : myAcceptAllToolTip);
    anAcceptAllAction->setStatusTip(!anEnabled ? anError : myAcceptAllStatusTip);
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

void XGUI_ErrorMgr::storeInitialActionValues()
{
  XGUI_ActionsMgr* anActionsMgr = workshop()->actionsMgr();
  QAction* anAcceptAllAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptAll);
  myAcceptAllToolTip = anAcceptAllAction->toolTip();
  myAcceptAllStatusTip = anAcceptAllAction->statusTip();

  QAction* anOkAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::Accept);
  myAcceptToolTip = anOkAction->toolTip();
  myAcceptStatusTip = anOkAction->toolTip();
}

void XGUI_ErrorMgr::updateAcceptActionState(const QString& theError)
{
  XGUI_ActionsMgr* anActionsMgr = workshop()->actionsMgr();
  QAction* anAcceptAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::Accept);
  QAction* anAcceptPlusAction =
    anActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptPlus);

  if (myAcceptAllToolTip.isEmpty() && myAcceptToolTip.isEmpty())
    storeInitialActionValues();

  bool anEnabled = theError.isEmpty();
  anAcceptAction->setEnabled(anEnabled);
  anAcceptPlusAction->setEnabled(anEnabled);
  anAcceptAction->setToolTip(anEnabled ? myAcceptToolTip : theError);
  anAcceptAction->setStatusTip(anEnabled ? myAcceptStatusTip : theError);
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
    QString aTTip = aWidget->toolTip().section(tr("Errors:") + "\n", 0, 0).trimmed();
    // Add the error message into the tool tip
    if (!theError.isEmpty()) {
      if (!aTTip.isEmpty())
        aTTip.append('\n');
      aTTip += tr("Errors:") + "\n" + theError;
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

