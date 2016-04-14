// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetBoolValue.cpp
// Created:     04 June 2014
// Author:      Vitaly Smetannikov

#include <ModuleBase_WidgetBoolValue.h>
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeBoolean.h>
#include <ModelAPI_Data.h>

#include <Config_Keywords.h>
#include <Config_WidgetAPI.h>

#include <Events_Loop.h>
#include <ModelAPI_Events.h>

#include <QWidget>
#include <QLayout>
#include <QCheckBox>

ModuleBase_WidgetBoolValue::ModuleBase_WidgetBoolValue(QWidget* theParent,
                                                       const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QString aText = QString::fromStdString(theData->widgetLabel());
  QString aToolTip = QString::fromStdString(theData->widgetTooltip());
  bool isChecked = theData->getBooleanAttribute(ATTR_DEFAULT, false);

  myCheckBox = new QCheckBox(aText, this);
  myCheckBox->setToolTip(aToolTip);
  myCheckBox->setChecked(isChecked);

  QVBoxLayout* aMainLayout = new QVBoxLayout(this);
  ModuleBase_Tools::adjustMargins(aMainLayout);
  aMainLayout->addWidget(myCheckBox);
  setLayout(aMainLayout);

  connect(myCheckBox, SIGNAL(toggled(bool)), this, SIGNAL(valuesChanged()));
}

ModuleBase_WidgetBoolValue::~ModuleBase_WidgetBoolValue()
{
}

bool ModuleBase_WidgetBoolValue::storeValueCustom()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeBoolean> aBool = aData->boolean(attributeID());
  aBool->setValue(myCheckBox->isChecked());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetBoolValue::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeBoolean> aRef = aData->boolean(attributeID());

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

void ModuleBase_WidgetBoolValue::setHighlighted(bool)
{
  return;
}
