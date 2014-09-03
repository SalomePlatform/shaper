// File:        ModuleBase_WidgetChoice.cpp
// Created:     03 Sept 2014
// Author:      Vitaly Smetannikov

#include "ModuleBase_WidgetChoice.h"

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
  aLayout->setContentsMargins(0, 0, 0, 0);

  QString aLabelText = QString::fromStdString(theData->widgetLabel());
  QString aLabelIcon = QString::fromStdString(theData->widgetIcon());
  myLabel = new QLabel(aLabelText, myContainer);
  myLabel->setPixmap(QPixmap(aLabelIcon));
  aLayout->addWidget(myLabel);

  myCombo = new QComboBox(myContainer);
  aLayout->addWidget(myCombo);
  connect(myCombo, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
}

ModuleBase_WidgetChoice::~ModuleBase_WidgetChoice()
{
}
  
bool ModuleBase_WidgetChoice::storeValue() const
{
  return true;
}

bool ModuleBase_WidgetChoice::restoreValue()
{
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
  aControls.append(myLabel);
  aControls.append(myCombo);
  return aControls;
}

void ModuleBase_WidgetChoice::onCurrentIndexChanged(int theIndex)
{
}