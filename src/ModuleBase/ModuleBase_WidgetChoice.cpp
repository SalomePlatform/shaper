// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetChoice.cpp
// Created:     03 Sept 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_WidgetChoice.h"
#include <ModuleBase_Tools.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Data.h>
#include <Config_WidgetAPI.h>

#include <QWidget>
#include <QLayout>
#include <QLabel>
#include <QComboBox>

ModuleBase_WidgetChoice::ModuleBase_WidgetChoice(QWidget* theParent, 
                                                 const Config_WidgetAPI* theData, 
                                                 const std::string& theParentId)
    : ModuleBase_ModelWidget(theParent, theData, theParentId)
{
  myContainer = new QWidget(theParent);
  QHBoxLayout* aLayout = new QHBoxLayout(myContainer);
  ModuleBase_Tools::adjustMargins(aLayout);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, myContainer);
  if (!aLabelIcon.isEmpty())
    myLabel->setPixmap(QPixmap(aLabelIcon));
  aLayout->addWidget(myLabel);

  myCombo = new QComboBox(myContainer);
  aLayout->addWidget(myCombo, 1);
 
  std::string aTypes = theData->getProperty("string_list");
  QStringList aList = QString(aTypes.c_str()).split(' ');
  myCombo->addItems(aList);

  connect(myCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

ModuleBase_WidgetChoice::~ModuleBase_WidgetChoice()
{
}
  
bool ModuleBase_WidgetChoice::storeValueCustom() const
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeInteger> aIntAttr = aData->integer(attributeID());

  aIntAttr->setValue(myCombo->currentIndex());
  updateObject(myFeature);
  return true;
}

bool ModuleBase_WidgetChoice::restoreValue()
{
  DataPtr aData = myFeature->data();
  std::shared_ptr<ModelAPI_AttributeInteger> aIntAttr = aData->integer(attributeID());

  bool isBlocked = myCombo->blockSignals(true);
  myCombo->setCurrentIndex(aIntAttr->value());
  myCombo->blockSignals(isBlocked);
  return true;
}

bool ModuleBase_WidgetChoice::focusTo()
{
  myCombo->setFocus();
  return true;
}

QList<QWidget*> ModuleBase_WidgetChoice::getControls() const
{
  QList<QWidget*> aControls;
  aControls.append(myCombo);
  return aControls;
}

void ModuleBase_WidgetChoice::onCurrentIndexChanged(int theIndex)
{
  emit valuesChanged();
  // Don't transfer focus
  // emit focusOutWidget(this);
}
