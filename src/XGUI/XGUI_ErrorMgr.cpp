// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_ErrorMgr.cpp
// Created:     22 July 2015
// Author:      Sergey POKHODENKO

#include "XGUI_ErrorMgr.h"

#include "XGUI_OperationMgr.h"

#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_OperationFeature.h>

#include <ModelAPI_Attribute.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_Validator.h>

#include <QLabel>

XGUI_ErrorMgr::XGUI_ErrorMgr(QObject* theParent /*= 0*/)
  : ModuleBase_IErrorMgr(theParent)
{

}

XGUI_ErrorMgr::~XGUI_ErrorMgr()
{

}

const char* toString(ModelAPI_ExecState theExecState) 
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
}

void XGUI_ErrorMgr::onValidationStateChanged()
{
  XGUI_OperationMgr* anOperationMgr = dynamic_cast<XGUI_OperationMgr*>(sender());
  if (!anOperationMgr)
    return;
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                  (anOperationMgr->currentOperation());
  if (!myPropertyPanel || !aFOperation)
    return;

  // get feature
  FeaturePtr aFeature = aFOperation->feature();
  if (!aFeature.get() || !aFeature->data()->isValid())
    return;

  // set error indication
  QString anError = QString::fromStdString(aFeature->error());
  if (anError.isEmpty()) {
    bool isDone = ( aFeature->data()->execState() == ModelAPI_StateDone
                 || aFeature->data()->execState() == ModelAPI_StateMustBeUpdated );
    if (!isDone)
      anError = toString(aFeature->data()->execState());
  }

  QWidget* aWidget = myPropertyPanel->headerWidget();
  if (aWidget) {
    aWidget->setToolTip(anError);
    aWidget->setStyleSheet(anError.isEmpty() ? "" : "background-color:pink;");
  }
}

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
    aWidget->setStyleSheet(anError.isEmpty() ? "" : "background-color:pink;");
  }
}
