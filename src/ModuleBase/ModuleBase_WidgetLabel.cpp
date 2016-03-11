// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_WidgetLabel.cpp
// Created:     03 Dec 2014
// Author:      Vitaly SMETANNIKOV

#include "ModuleBase_WidgetLabel.h"

#include <Config_WidgetAPI.h>
#include <ModuleBase_Tools.h>

#include <QLabel>
#include <QVBoxLayout>


ModuleBase_WidgetLabel::ModuleBase_WidgetLabel(QWidget* theParent,
                                               const Config_WidgetAPI* theData)
: ModuleBase_ModelWidget(theParent, theData)
{
  QString aText = QString::fromStdString(theData->getProperty("title"));
  myLabel = new QLabel(aText, theParent);
  myLabel->setWordWrap(true);
  myLabel->setIndent(5);
  myLabel->setAlignment(Qt::AlignLeft | Qt::AlignTop);
  QVBoxLayout* aLayout = new QVBoxLayout(this);
  ModuleBase_Tools::zeroMargins(aLayout);
  aLayout->addWidget(myLabel);
  setLayout(aLayout);
}

ModuleBase_WidgetLabel::~ModuleBase_WidgetLabel()
{
}

QList<QWidget*> ModuleBase_WidgetLabel::getControls() const
{
  return QList<QWidget*>();
}

