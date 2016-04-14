// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_PageGroupBox.h
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <SamplePanelPlugin_ModelWidget.h>
#include <SamplePanelPlugin_Feature.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Events.h>

#include <Config_WidgetAPI.h>
#include <Events_Loop.h>

#include <QVBoxLayout>
#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

SamplePanelPlugin_ModelWidget::SamplePanelPlugin_ModelWidget(QWidget* theParent,
                                                const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData), myDefaultValue(0)
{
  QVBoxLayout* aLay = new QVBoxLayout(this);
  aLay->setContentsMargins(0, 0, 0, 0);

  myMainWidget = new QWidget(this);
  aLay->addWidget(myMainWidget);

  QGridLayout* aLayout = new QGridLayout(myMainWidget);
  aLayout->addWidget(new QLabel("Values:"), 0, 0);

  myComboBox = new QComboBox(myMainWidget);
  myComboBox->addItem("Value_1");
  myComboBox->addItem("Value_2");
  myComboBox->addItem("Value_3");

  connect(myComboBox, SIGNAL(currentIndexChanged(int)), this, SIGNAL(valuesChanged()));
  aLayout->addWidget(myComboBox, 0, 1);
}

QList<QWidget*> SamplePanelPlugin_ModelWidget::getControls() const
{
  QList<QWidget*> aControls;
  // this control will accept focus and will be highlighted in the Property Panel
  aControls.push_back(myComboBox);
  return aControls;
}

bool SamplePanelPlugin_ModelWidget::storeValueCustom()
{
  AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  AttributeIntegerPtr aValueAttribute =
                        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);
  aValueAttribute->setValue(myComboBox->currentIndex());
  updateObject(myFeature);
  return true;
}

bool SamplePanelPlugin_ModelWidget::restoreValueCustom()
{
  AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  AttributeIntegerPtr aValueAttribute =
                        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);

  bool isBlocked = myComboBox->blockSignals(true);
  myComboBox->setCurrentIndex(aValueAttribute->value());
  myComboBox->blockSignals(isBlocked);

  return true;
}

