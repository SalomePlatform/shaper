// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_Widgets.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetDoubleValue.h>
#include <ModuleBase_DoubleSpinBox.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeDouble.h>
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

  mySpinBox = new ModuleBase_DoubleSpinBox(this);
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
  connect(mySpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(valuesChanged()));
}

ModuleBase_WidgetDoubleValue::~ModuleBase_WidgetDoubleValue()
{
}

void ModuleBase_WidgetDoubleValue::reset()
{
  if (isComputedDefault()) {
    return;
    //if (myFeature->compute(myAttributeID))
    //  restoreValue();
  }
  else {
    bool isOk;
    double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
    ModuleBase_Tools::setSpinValue(mySpinBox, isOk ? aDefValue : 0.0);
    storeValueCustom();
  }
}

bool ModuleBase_WidgetDoubleValue::storeValueCustom() const
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr aReal = aData->real(attributeID());
  aReal->setValue(mySpinBox->value());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetDoubleValue::restoreValue()
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr aRef = aData->real(attributeID());

  ModuleBase_Tools::setSpinValue(mySpinBox, aRef->value());

  return true;
}

QList<QWidget*> ModuleBase_WidgetDoubleValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(mySpinBox);
  return aList;
}
