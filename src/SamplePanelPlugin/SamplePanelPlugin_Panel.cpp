// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_PageGroupBox.h
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <SamplePanelPlugin_Panel.h>
#include <SamplePanelPlugin_Feature.h>

#include <ModelAPI_AttributeInteger.h>
#include <ModelAPI_Events.h>

#include <Events_Loop.h>

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

SamplePanelPlugin_Panel::SamplePanelPlugin_Panel(QWidget* theParent)
: QWidget(theParent), myDefaultValue(0)
{
  QGridLayout* aLayout = new QGridLayout(this);
  aLayout->addWidget(new QLabel("Values:"), 0, 0);

  myComboBox = new QComboBox(this);
  myComboBox->addItem("Value_1");
  myComboBox->addItem("Value_2");
  myComboBox->addItem("Value_3");

  connect(myComboBox, SIGNAL(currentIndexChanged(int)), this, SLOT(onCurrentIndexChanged(int)));
  aLayout->addWidget(myComboBox, 0, 1);
}

void SamplePanelPlugin_Panel::setFeature(const FeaturePtr& theFeature)
{
  myFeature = theFeature;

  AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  AttributeIntegerPtr aValueAttribute =
                        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);
  /// the attribute should be filled with some value, because Apply button of the Property
  /// panel is enabled only if all attributes of the feature are initialized
  /// It is possible to make it in the initializeAttribute method of the feature after
  /// attribute creation
  if (!aValueAttribute->isInitialized()) {
    aValueAttribute->setValue(myDefaultValue);
    /// to update error state of the feature: previous value was that this attribute is not
    /// initialized yet. Apply of Property panel was disabled.
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
  }

  int aValueId = aValueAttribute->value();
  myComboBox->setCurrentIndex(aValueId);
}

void SamplePanelPlugin_Panel::onCurrentIndexChanged(int theIndex)
{
  if (!myFeature.get())
    return;

  AttributePtr anAttribute = myFeature->attribute(SamplePanelPlugin_Feature::VALUE_ID());
  AttributeIntegerPtr aValueAttribute =
                        std::dynamic_pointer_cast<ModelAPI_AttributeInteger>(anAttribute);
  aValueAttribute->setValue(theIndex);
  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_UPDATED));
}
