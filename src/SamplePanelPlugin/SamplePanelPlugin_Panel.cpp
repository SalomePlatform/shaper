// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        SamplePanelPlugin_PageGroupBox.h
// Created:     29 Mar 2015
// Author:      Natalia ERMOLAEVA

#include <SamplePanelPlugin_Panel.h>

#include <QGridLayout>
#include <QLabel>
#include <QComboBox>

SamplePanelPlugin_Panel::SamplePanelPlugin_Panel(QWidget* theParent)
  : QWidget(theParent)
{
  QGridLayout* aLayout = new QGridLayout(this);
  aLayout->addWidget(new QLabel("Values:"), 0, 0);

  QComboBox* aComboBox = new QComboBox(this);
  aComboBox->addItem("Value_1");
  aComboBox->addItem("Value_2");
  aComboBox->addItem("Value_3");

  aLayout->addWidget(aComboBox, 0, 1);
}
