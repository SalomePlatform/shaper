// Copyright (C) 2014-2016 CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetLabelValue.cpp
// Created:     30 Nov 2016
// Author:      Natalia ERMOLAEVA

#include "ModuleBase_WidgetLabelValue.h"

#include <Config_WidgetAPI.h>
#include <Config_Keywords.h>
#include <ModuleBase_LabelValue.h>

#include <ModelAPI_AttributeDouble.h>

#include <QLabel>
#include <QVBoxLayout>

ModuleBase_WidgetLabelValue::ModuleBase_WidgetLabelValue(QWidget* theParent,
                                               const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  aLayout->setContentsMargins(0, 0, 0, 0);
  aLayout->setSpacing(0);

  QString aText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  QString aToolTip = QString::fromStdString(theData->widgetTooltip());

  myLabel = new ModuleBase_LabelValue(theParent, aText, aToolTip, aLabelIcon);
  bool isOk;
  double aDefVal = QString::fromStdString(getDefaultValue()).toDouble(&isOk);
  if (isOk) {
    myLabel->setValue(aDefVal);
  }

  aLayout->addWidget(myLabel);
}

ModuleBase_WidgetLabelValue::~ModuleBase_WidgetLabelValue()
{
}

QList<QWidget*> ModuleBase_WidgetLabelValue::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myLabel);
  return aControls;
}

bool ModuleBase_WidgetLabelValue::restoreValueCustom()
{
  DataPtr aData = myFeature->data();
  AttributeDoublePtr anAttribute = aData->real(attributeID());
  double aValue = 0;
  if (anAttribute.get() && anAttribute->isInitialized())
    aValue = anAttribute->value();
  myLabel->setValue(aValue);
  return true;
}

bool ModuleBase_WidgetLabelValue::storeValueCustom()
{
  return true;
}
