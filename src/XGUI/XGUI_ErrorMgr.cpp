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

}

XGUI_ErrorMgr::~XGUI_ErrorMgr()
{

}

void XGUI_ErrorMgr::updateActions(const FeaturePtr& theFeature)
{
  QString anError = myWorkshop->module()->getFeatureError(theFeature);

  //update Ok Action and header of property panel if the current operation started for the feature
  XGUI_ActionsMgr* anActionsMgr = workshop()->actionsMgr();
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                      (workshop()->operationMgr()->currentOperation());
  if (aFOperation && aFOperation->feature() == theFeature) {
    QAction* anOkAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::Accept);
    updateActionState(anOkAction, theFeature);
  }
}

void XGUI_ErrorMgr::updateAcceptAllAction(const FeaturePtr& theFeature)
{
  QString anError = myWorkshop->module()->getFeatureError(theFeature);

  XGUI_ActionsMgr* anActionsMgr = workshop()->actionsMgr();
  if (workshop()->isFeatureOfNested(theFeature)) {
    QAction* anAcceptAllAction = anActionsMgr->operationStateAction(XGUI_ActionsMgr::AcceptAll, NULL);
    bool anEnabled = anError.isEmpty();
    anAcceptAllAction->setEnabled(anEnabled);
    anAcceptAllAction->setToolTip(anError);
  }
}

bool XGUI_ErrorMgr::canProcessClick(QAction* theAction, const FeaturePtr& theFeature)
{
  QString anError = myWorkshop->module()->getFeatureError(theFeature);
  bool isActionEnabled = anError.isEmpty();
  if (!isActionEnabled && !anError.isEmpty()) {
    if (!myErrorDialog) {
      myErrorDialog = new QDialog(QApplication::desktop(), Qt::Popup);
      QHBoxLayout* aLay = new QHBoxLayout(myErrorDialog);
      aLay->setContentsMargins(0, 0, 0, 0);

      QFrame* aMarginWidget = new QFrame(myErrorDialog);
      aMarginWidget->setFrameStyle(QFrame::Panel | QFrame::Raised);

      aLay->addWidget(aMarginWidget);
      QHBoxLayout* aMarginLay = new QHBoxLayout(aMarginWidget);
      aMarginLay->setContentsMargins(4, 4, 4, 4);

      myErrorLabel = new QLabel(aMarginWidget);
      aMarginLay->addWidget(myErrorLabel);
    }
    myErrorLabel->setText(anError);
    myErrorDialog->move(QCursor::pos());
    myErrorDialog->show();
  }
  return isActionEnabled;
}

void XGUI_ErrorMgr::updateActionState(QAction* theAction, const FeaturePtr& theFeature/*,
                                      const bool theEnabled*/)
{
  QString anError = myWorkshop->module()->getFeatureError(theFeature);
  bool anEnabled = anError.isEmpty();

  bool isActionEnabled = theAction->data() != INVALID_VALUE;
  if (anEnabled  != isActionEnabled) {
    // update enable state of the button
    theAction->setIcon(anEnabled ? QIcon(":pictures/button_ok.png"): QIcon(":pictures/button_ok_error.png"));
    if (anEnabled)
      theAction->setData("");
    else
      theAction->setData(INVALID_VALUE);
  }
  // some operations have no property panel, so it is important to check that it is not null
  if (myPropertyPanel) {
    // update controls error information
    QWidget* aWidget = myPropertyPanel->headerWidget();
    if (aWidget)
      aWidget->setToolTip(anError);
  }
}

/*const char* toString(ModelAPI_ExecState theExecState) 
{
#define TO_STRING(__NAME__) case __NAME__: return #__NAME__;
  switch (theExecState) {
  TO_STRING(ModelAPI_StateDone)
  TO_STRING(ModelAPI_StateMustBeUpdated)
  TO_STRING(ModelAPI_StateExecFailed)
  TO_STRING(ModelAPI_StateInvalidArgument)
  TO_STRING(ModelAPI_StateNothing)
  default: return "Unknown ExecState.";
  }
#undef TO_STRING
}*/

/*void XGUI_ErrorMgr::onValidationStateChanged()
{
  XGUI_OperationMgr* anOperationMgr = dynamic_cast<XGUI_OperationMgr*>(sender());
  if (!anOperationMgr)
    return;
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                  (anOperationMgr->currentOperation());
  if (!myPropertyPanel || !aFOperation)
    return;

  FeaturePtr aFeature = aFOperation->feature();
  QString anError = getFeatureError(aFeature);

  QWidget* aWidget = myPropertyPanel->headerWidget();
  if (aWidget) {
    aWidget->setToolTip(anError);
    aWidget->setStyleSheet(anError.isEmpty() ? "" : "background-color:pink;");
  }
}*/

/*QString XGUI_ErrorMgr::getFeatureError(const FeaturePtr& theFeature) const
{
  QString anError;
  // get feature
  if (!theFeature.get() || !theFeature->data()->isValid())
    return anError;

  // set error indication
  anError = QString::fromStdString(theFeature->error());
  if (anError.isEmpty()) {
    bool isDone = ( theFeature->data()->execState() == ModelAPI_StateDone
                 || theFeature->data()->execState() == ModelAPI_StateMustBeUpdated );
    if (!isDone)
      anError = toString(theFeature->data()->execState());
  }

  return anError;
}*/

void XGUI_ErrorMgr::onWidgetChanged()
{
  static ModelAPI_ValidatorsFactory* aValidators = ModelAPI_Session::get()->validators();

  ModuleBase_ModelWidget* aModelWidget = dynamic_cast<ModuleBase_ModelWidget*>(sender());
  if (!aModelWidget || !aModelWidget->feature().get())
    return;

  std::string anAttributeID = aModelWidget->attributeID();
  AttributePtr anAttribute = aModelWidget->feature()->attribute(anAttributeID);
  if (!anAttribute.get())
    return;

  std::string aValidatorID;
  std::string anErrorMsg;
  if (!aValidators->validate(anAttribute, aValidatorID, anErrorMsg)) {
    if (anErrorMsg.empty())
      anErrorMsg = "unknown error.";
    anErrorMsg = anAttributeID + " - " + aValidatorID + ": " + anErrorMsg;
  }

  QString anError = QString::fromStdString(anErrorMsg);
  QList<QWidget*> aWidgetList = aModelWidget->getControls();
  foreach(QWidget* aWidget, aWidgetList) {
    QLabel* aLabel = qobject_cast<QLabel*>(aWidget);
    // We won't set the effect to QLabels - it looks ugly
    if (aLabel) continue;

    // Get the original tool tip of the widget
    QString aTTip = aWidget->toolTip().section("Errors:\n", 0, 0).trimmed();
    // Add the error message into the tool tip
    if (!anError.isEmpty()) {
      if (!aTTip.isEmpty())
        aTTip.append('\n');
      aTTip += "Errors:\n" + anError;
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
