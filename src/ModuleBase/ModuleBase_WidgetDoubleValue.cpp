// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetDoubleValue.cpp
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

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

//#define APPLY_BY_ENTER_OR_TAB

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
  myLabel->setToolTip(aTTip);

  aControlLay->addRow(myLabel, mySpinBox);
#ifdef APPLY_BY_ENTER_OR_TAB
  // Apply widget value change by enter/tab event.
  connect(mySpinBox, SIGNAL(editingFinished()), this, SIGNAL(valuesChanged()));
#else
  connect(mySpinBox, SIGNAL(valueChanged(const QString&)), this, SIGNAL(valuesChanged()));
#endif
}

ModuleBase_WidgetDoubleValue::~ModuleBase_WidgetDoubleValue()
{
}

bool ModuleBase_WidgetDoubleValue::reset()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault() || mySpinBox->hasVariable()) {
    aDone = false;
  } else {
    bool isOk;
    double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
    // reset the value just if there is a default value definition in the XML definition
    // if the double value can not be found by the default value, do nothing
    if (isOk) {
      ModuleBase_Tools::setSpinValue(mySpinBox, aDefValue);
      storeValue();
      aDone = true;
    }
  }
  return aDone;
}

bool ModuleBase_WidgetDoubleValue::storeValueCustom() const
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr aReal = aData->real(attributeID());
  if (mySpinBox->hasVariable()) {
    // Here is a text of a real value or an expression.
    std::string aText = mySpinBox->text().toStdString();
    aReal->setText(aText);
  } else {
    // it is important to set the empty text value to the attribute before set the value
    // because setValue tries to calculate the attribute value according to the
    // attribute current text
    aReal->setText("");
    aReal->setValue(mySpinBox->value());
  }
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetDoubleValue::restoreValueCustom()
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
