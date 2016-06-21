// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetCheckDoubleValue.cpp
// Created:     16 June 2016
// Author:      Clarisse Genrault (CEA)

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Data.h>
#include <ModelAPI_Object.h>

#include <ModuleBase_ParamSpinBox.h>
#include <ModuleBase_Tools.h>
#include <ModuleBase_WidgetCheckDoubleValue.h>
#include <ModuleBase_IconFactory.h>

#include <QFormLayout>
#include <QCheckBox>
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

//#define DEBUG_COMPLETE_WITH_PARAMETERS

ModuleBase_WidgetCheckDoubleValue::ModuleBase_WidgetCheckDoubleValue(QWidget* theParent,
                                                                     const Config_WidgetAPI* theData)
    : ModuleBase_ModelWidget(theParent, theData)
{
  QFormLayout* aControlLay = new QFormLayout(this);
  ModuleBase_Tools::adjustMargins(aControlLay);
  
  myCheckAttributeId = theData->getProperty("id_check");

  QString aCheckText = QString::fromStdString(theData->widgetLabel());
  myCheckBox = new QCheckBox(aCheckText, this);

  bool aAcceptVariables = theData->getBooleanAttribute(DOUBLE_WDG_ACCEPT_EXPRESSIONS, true);

  mySpinBox = new ModuleBase_ParamSpinBox(this);
  mySpinBox->setAcceptVariables(aAcceptVariables);
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
  myCheckBox->setToolTip(aTTip);
  
  myCheckBox->setChecked(false);
  mySpinBox->setEnabled(false);            

  aControlLay->addRow(myCheckBox, mySpinBox);
  connect(mySpinBox, SIGNAL(valueChanged(const QString&)), this, SIGNAL(valuesModified()));
  connect(myCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(valuesChanged()));
}

ModuleBase_WidgetCheckDoubleValue::~ModuleBase_WidgetCheckDoubleValue()
{
}

void ModuleBase_WidgetCheckDoubleValue::activateCustom()
{
  ModuleBase_ModelWidget::activateCustom();
#ifdef DEBUG_COMPLETE_WITH_PARAMETERS
  QStringList aParameters;
  ModuleBase_Tools::getParameters(aParameters);
  mySpinBox->setCompletionList(aParameters);
#endif
}

bool ModuleBase_WidgetCheckDoubleValue::resetCustom()
{
  bool aDone = false;
  if (!isUseReset() || isComputedDefault() || mySpinBox->hasVariable()) {
    aDone = false;
  } else {
    bool isOk;
    double aDefValue = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
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

bool ModuleBase_WidgetCheckDoubleValue::storeValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr aReal = aData->real(attributeID());
  AttributeBooleanPtr aBoolean = aData->boolean(myCheckAttributeId);
  
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
  
  // The spinbox is enabled/disabled only if the checkbox is checked/unchecked
  aBoolean->setValue(myCheckBox->isChecked());
  mySpinBox->setEnabled(myCheckBox->isChecked());
  
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetCheckDoubleValue::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr aRef = aData->real(attributeID());
  AttributeBooleanPtr aBoolean = aData->boolean(myCheckAttributeId);
  std::string aTextRepr = aRef->text();
  if (!aTextRepr.empty()) {
    ModuleBase_Tools::setSpinText(mySpinBox, QString::fromStdString(aTextRepr));
  } else {
    ModuleBase_Tools::setSpinValue(mySpinBox, aRef->value());
  }
  
  // The spinbox is enabled/disabled only if the checkbox is checked/unchecked
  myCheckBox->setChecked(aBoolean->value());
  mySpinBox->setEnabled(aBoolean->value());
  
  return true;
}

void ModuleBase_WidgetCheckDoubleValue::selectContent()
{
  mySpinBox->selectAll();
}

QList<QWidget*> ModuleBase_WidgetCheckDoubleValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(mySpinBox);
  aList.append(myCheckBox);
  return aList;
}

bool ModuleBase_WidgetCheckDoubleValue::processEnter()
{
  bool isModified = getValueState() == ModifiedInPP;
  if (isModified) {
    emit valuesChanged();
    mySpinBox->selectAll();
  }
  return isModified;
}
