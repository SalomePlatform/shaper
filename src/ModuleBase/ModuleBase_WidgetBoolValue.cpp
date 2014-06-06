// File:        ModuleBase_Widgets.h
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetBoolValue.h>

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

ModuleBase_WidgetBoolValue::ModuleBase_WidgetBoolValue(QWidget* theParent, const Config_WidgetAPI* theData)
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

ModuleBase_WidgetBoolValue::~ModuleBase_WidgetBoolValue()
{
}

QWidget* ModuleBase_WidgetBoolValue::getControl() const 
{ 
  return myCheckBox; 
}

bool ModuleBase_WidgetBoolValue::storeValue(FeaturePtr theFeature) const
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeBoolean> aBool = aData->boolean(myAttributeID);

  if (aBool->value() != myCheckBox->isChecked()) {
    aBool->setValue(myCheckBox->isChecked());
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_FEATURE_UPDATED));
  }
  return true;
}

bool ModuleBase_WidgetBoolValue::restoreValue(FeaturePtr theFeature)
{
  DataPtr aData = theFeature->data();
  boost::shared_ptr<ModelAPI_AttributeBoolean> aRef = aData->boolean(myAttributeID);

  bool isBlocked = myCheckBox->blockSignals(true);
  myCheckBox->setChecked(aRef->value());
  myCheckBox->blockSignals(isBlocked);

  return true;
}

QList<QWidget*> ModuleBase_WidgetBoolValue::getControls() const
{
  QList<QWidget*> aList;
  aList.append(myCheckBox);
  return aList;
}
