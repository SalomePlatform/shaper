// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetIntValue.cpp
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetIntValue.h>
#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_ParamIntSpinBox.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <QWidget>
#include <QFormLayout>
#include <QLabel>
#include <QEvent>
#include <QTimer>

#include <math.h>

#ifndef INT_MAX
#define INT_MAX   2147483647 
#endif

#ifdef _DEBUG
#include <iostream>
#endif

ModuleBase_WidgetIntValue::ModuleBase_WidgetIntValue(QWidget* theParent,
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

  mySpinBox = new ModuleBase_ParamIntSpinBox(this);
  QString anObjName = QString::fromStdString(attributeID());
  mySpinBox->setObjectName(anObjName);

  bool isOk = false;
  std::string aProp = theData->getProperty(DOUBLE_WDG_MIN);
  int aMinVal = QString::fromStdString(aProp).toInt(&isOk);
  if (isOk) {
    mySpinBox->setMinimum(aMinVal);
  } else {
    mySpinBox->setMinimum(-INT_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_MAX);
  int aMaxVal = QString::fromStdString(aProp).toInt(&isOk);
  if (isOk) {
    mySpinBox->setMaximum(aMaxVal);
  } else {
    mySpinBox->setMaximum(INT_MAX);
  }

  aProp = theData->getProperty(DOUBLE_WDG_STEP);
  int aStepVal = QString::fromStdString(aProp).toInt(&isOk);
  if (isOk) {
    mySpinBox->setSingleStep(aStepVal);
  }

  int aDefVal = QString::fromStdString(getDefaultValue()).toInt(&isOk);
  if (isOk) {
    mySpinBox->setValue(aDefVal);
  }

  QString aTTip = QString::fromStdString(theData->widgetTooltip());
  mySpinBox->setToolTip(aTTip);
  myLabel->setToolTip(aTTip);

  aControlLay->addRow(myLabel, mySpinBox);
  connect(mySpinBox, SIGNAL(valueChanged(int)), this, SIGNAL(valuesModified()));
}

ModuleBase_WidgetIntValue::~ModuleBase_WidgetIntValue()
{
}

bool ModuleBase_WidgetIntValue::resetCustom()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault() || mySpinBox->hasVariable()) {
    aDone = false;
  } else {
    bool isOk;
    int aDefValue = QString::fromStdString(getDefaultValue()).toInt(&isOk);
    // reset the value just if there is a default value definition in the XML definition
    // if the value can not be found by the default value, do nothing
    if (isOk) {
      ModuleBase_Tools::setSpinValue(mySpinBox, aDefValue);
      storeValue();
      aDone = true;
    }
  }
  return aDone;
}

bool ModuleBase_WidgetIntValue::storeValueCustom() const
{
  DataPtr aData = myFeature->data();
  AttributeIntegerPtr anAttribute = aData->integer(attributeID());
  if (mySpinBox->hasVariable()) {
    // Here is a text of a real value or an expression.
    std::string aText = mySpinBox->text().toStdString();
    anAttribute->setText(aText);
  } else {
    // it is important to set the empty text value to the attribute before set the value
    // because setValue tries to calculate the attribute value according to the
    // attribute current text
    anAttribute->setText("");
    anAttribute->setValue(mySpinBox->value());
  }
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetIntValue::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeIntegerPtr anAttribute = aData->integer(attributeID());
  std::string aTextRepr = anAttribute->text();
  if (!aTextRepr.empty()) {
    ModuleBase_Tools::setSpinText(mySpinBox, QString::fromStdString(aTextRepr));
  } else {
    ModuleBase_Tools::setSpinValue(mySpinBox, anAttribute->value());
  }
  return true;
}

void ModuleBase_WidgetIntValue::selectContent()
{
  mySpinBox->selectAll();
}

QList<QWidget*> ModuleBase_WidgetIntValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(mySpinBox);
  return aList;
}

bool ModuleBase_WidgetIntValue::processEnter()
{
  bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    emit valuesChanged();
    mySpinBox->selectAll();
  }
  return isModified;
}
