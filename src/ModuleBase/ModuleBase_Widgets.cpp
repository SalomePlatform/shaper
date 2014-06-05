// File:        ModuleBase_Widgets.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_Widgets.h"

#include <ModelAPI_AttributeDouble.h>
#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Data.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <Model_Events.h>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QDoubleSpinBox>
#include <QCheckBox>


ModuleBase_DoubleValueWidget::ModuleBase_DoubleValueWidget(QWidget* theParent, const Config_WidgetAPI* theData)
  : ModuleBase_ModelWidget(theParent)
{
  myContainer = new QWidget(theParent);
  QHBoxLayout* aControlLay = new QHBoxLayout(myContainer);
  aControlLay->setContentsMargins(0, 0, 0, 0);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, myContainer);
  myLabel->setPixmap(QPixmap(aLabelIcon));
  aControlLay->addWidget(myLabel);

  myAttributeID = theData->widgetId();
  mySpinBox = new QDoubleSpinBox(myContainer);
  QString anObjName = QString::fromStdString(myAttributeID);
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
    mySpinBox->setSingleStep(aStepVal);
  }

  aProp = theData->getProperty(DOUBLE_WDG_DFLT);
  double aDefVal = QString::fromStdString(aProp).toDouble(&isOk);
  if (isOk) {
    mySpinBox->setValue(aDefVal);
  }

  QString aTTip = QString::fromStdString(theData->widgetTooltip());
  mySpinBox->setToolTip(aTTip);

  aControlLay->addWidget(mySpinBox);
  aControlLay->setStretch(1, 1);

  connect(mySpinBox, SIGNAL(valueChanged(double)), this, SIGNAL(valuesChanged()));
}

ModuleBase_DoubleValueWidget::~ModuleBase_DoubleValueWidget()
{
}

bool ModuleBase_DoubleValueWidget::storeValue(FeaturePtr theFeature) const
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeDouble> aReal = aData->real(myAttributeID);
  if (aReal->value() != mySpinBox->value()) {
    aReal->setValue(mySpinBox->value());
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  }
  return true;
}

bool ModuleBase_DoubleValueWidget::restoreValue(FeaturePtr theFeature)
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeDouble> aRef = aData->real(myAttributeID);

  bool isBlocked = mySpinBox->blockSignals(true);
  mySpinBox->setValue(aRef->value());
  mySpinBox->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_DoubleValueWidget::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myLabel);
  aList.append(mySpinBox);
  return aList;
}


//////////////////////////////////////////////////////////////////////////////////
ModuleBase_BoolValueWidget::ModuleBase_BoolValueWidget(QWidget* theParent, const Config_WidgetAPI* theData)
  : ModuleBase_ModelWidget(theParent)
{
  myAttributeID = theData->widgetId();
  QString aText = QString::fromStdString(theData->widgetLabel());
  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  QString aDefault = QString::fromStdString(theData->getProperty("default"));

  myCheckBox = new QCheckBox(aText, theParent);
  myCheckBox->setToolTip(aToolTip);
  myCheckBox->setChecked(aDefault == "true");

  connect(myCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(valuesChanged()));
}

ModuleBase_BoolValueWidget::~ModuleBase_BoolValueWidget()
{
}

QWidget* ModuleBase_BoolValueWidget::getControl() const 
{ 
  return myCheckBox; 
}

bool ModuleBase_BoolValueWidget::storeValue(FeaturePtr theFeature) const
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeBoolean> aBool = aData->boolean(myAttributeID);

  if (aBool->value() != myCheckBox->isChecked()) {
    aBool->setValue(myCheckBox->isChecked());
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  }
  return true;
}

bool ModuleBase_BoolValueWidget::restoreValue(FeaturePtr theFeature)
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeBoolean> aRef = aData->boolean(myAttributeID);

  bool isBlocked = myCheckBox->blockSignals(true);
  myCheckBox->setChecked(aRef->value());
  myCheckBox->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_BoolValueWidget::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myCheckBox);
  return aList;
}
