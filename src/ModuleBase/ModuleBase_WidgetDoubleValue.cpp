// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Widgets.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>
#include <ModelAPI_Events.h>

#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_WidgetDoubleValue.h>

#include <QFormLayout>
#include <QLabel>
#include <QList>
#include <QObject>
#include <QPixmap>
#include <QString>

#include <cfloat>

#ifndef DBL_MAX
#define DBL_MAX 1.7976931348623158e+308 
#endif
#ifdef _DEBUG
#include <iostream>
#endif

ModuleBase_WidgetDoubleValue::ModuleBase_WidgetDoubleValue(QWidget* theParent,
                                                           const Config_WidgetAPI* theData,
                                                           const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  QFormLayout* aControlLay = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(aControlLay);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, this);
  if (!aLabelIcon.isEmpty())
    myLabel->setPixmap(QPixmap(aLabelIcon));

  mySpinBox = new ModuleBase_ParamSpinBox(this);
  QString anObjName = QString::fromStdString(attributeID());
  mySpinBox->setObjectName(anObjName);

  bool isOk = false;
  std::string aProp = theData->getProperty(DOUBLE_WDG_MIN);
  double aMinVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setMinimum(aMinVal);
  } else {
    mySpinBox->setMinimum(-DBL_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_MAX);
  double aMaxVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setMaximum(aMaxVal);
  } else {
    mySpinBox->setMaximum(DBL_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_STEP);
  double aStepVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    double aMinStep = pow(10, -1. * (double) mySpinBox->decimals());
    if(aStepVal < aMinStep){
      aStepVal = aMinStep;
    }
    mySpinBox->setSingleStep(aStepVal);
  }

  double aDefVal = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setValue(aDefVal);
  }

  QString aTTip = QString::fromStdString(theData->widgetTooltip());
  mySpinBox->setToolTip(aTTip);

  aControlLay->addRow(myLabel, mySpinBox);
  connect(mySpinBox, SIGNAL(valueChanged(const QString&)), this, SIGNAL(valuesChanged()));
}

ModuleBase_WidgetDoubleValue::~ModuleBase_WidgetDoubleValue()
{
}

void ModuleBase_WidgetDoubleValue::reset()
{
  if (isComputedDefault() || mySpinBox->hasVariable()) {
    return;
    //if (myFeature->compute(myAttributeID))
    //  restoreValue();
  } else {
    bool isOk;
    double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
    // reset the value just if there is a default value definition in the XML definition
    // if the double value can not be found by the default value, do nothing
    if (isOk) {
      ModuleBase_Tools::setSpinValue(mySpinBox, aDefValue);
      storeValueCustom();
    }
  }
}

bool ModuleBase_WidgetDoubleValue::storeValueCustom() const
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr aReal = aData->real(attributeID());
  if (!mySpinBox->hasVariable()) {
    aReal->setValue(mySpinBox->value());
    aReal->setText("");
  } else {
    // Here is a text of a real value or an expression.
    std::string aText = mySpinBox->text().toStdString();
    aReal->setText(aText);
    // Send it to evaluator to convert into the double and store in the attribute
    static Events_ID anId = ModelAPI_AttributeEvalMessage::eventId();
    std::shared_ptr<ModelAPI_AttributeEvalMessage> aMessage =
      std::shared_ptr<ModelAPI_AttributeEvalMessage>(new ModelAPI_AttributeEvalMessage(anId, this));
    aMessage->setAttribute(aData->attribute(attributeID()));
    Events_Loop::loop()->send(aMessage);
  }
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetDoubleValue::restoreValue()
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr aRef = aData->real(attributeID());
  std::string aTextRepr = aRef->text();
  if (!aTextRepr.empty()) {
    ModuleBase_Tools::setSpinText(mySpinBox, QString::fromStdString(aTextRepr));
  } else {
    ModuleBase_Tools::setSpinValue(mySpinBox, aRef->value());
  }
  return true;
}

QList<QWidget*> ModuleBase_WidgetDoubleValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(mySpinBox);
  return aList;
}
