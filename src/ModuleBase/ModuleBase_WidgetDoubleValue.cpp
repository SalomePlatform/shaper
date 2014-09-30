// File:        ModuleBase_Widgets.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetDoubleValue.h>
#include <ModuleBase_DoubleSpinBox.h>

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_Data.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QEvent>
#include <QKeyEvent>
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
  myContainer = new QWidget(theParent);
  QHBoxLayout* aControlLay = new QHBoxLayout(myContainer);
  aControlLay->setContentsMargins(0, 0, 0, 0);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, myContainer);
  if (!aLabelIcon.isEmpty())
    myLabel->setPixmap(QPixmap(aLabelIcon));
  aControlLay->addWidget(myLabel);

  mySpinBox = new ModuleBase_DoubleSpinBox(myContainer);
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

  aProp = theData->getProperty(DOUBLE_WDG_DEFAULT);
  double aDefVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setValue(aDefVal);
  } else if (aProp == DOUBLE_WDG_DEFAULT_COMPUTED){
    myIsComputedDefault = true;
  }

  QString aTTip = QString::fromStdString(theData->widgetTooltip());
  mySpinBox->setToolTip(aTTip);

  aControlLay->addWidget(mySpinBox);
  aControlLay->setStretch(1, 1);

  connect(mySpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(valuesChanged()));
}

ModuleBase_WidgetDoubleValue::~ModuleBase_WidgetDoubleValue()
{
}

bool ModuleBase_WidgetDoubleValue::storeValue() const
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

  bool isBlocked = mySpinBox->blockSignals(true);
  mySpinBox->setValue(aRef->value());
  mySpinBox->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_WidgetDoubleValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myLabel);
  aList.append(mySpinBox);
  return aList;
}
