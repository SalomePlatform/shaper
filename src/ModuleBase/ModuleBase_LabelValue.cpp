// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        ModuleBase_IWorkshop.cpp
// Created:     30 Nov 2016
// Author:      Natalia ERMOLAEVA

#include <ModuleBase_LabelValue.h>
#include <ModuleBase_IconFactory.h>
#include <ModuleBase_Tools.h>

#include <QHBoxLayout>
#include <QLabel>
#include <QLocale>

ModuleBase_LabelValue::ModuleBase_LabelValue(QWidget* theParent, const QString& theText,
                                             const QString& theToolTip, const QString& theIcon,
                                             int thePrecision)
: QWidget(theParent), myPrecision(thePrecision), myValue(0)
{
  QHBoxLayout* aLayout = new QHBoxLayout(this);
  aLayout->setContentsMargins(2, 0, 0, 0);
  aLayout->setSpacing(0);

  myLabel = new QLabel(QString("%1 : ").arg(theText), this);
  if (!theIcon.isEmpty()) {
    myLabel->setPixmap(ModuleBase_IconFactory::loadPixmap(theIcon));
    aLayout->setSpacing(4);
  }
  myLabel->setToolTip(!theToolTip.isEmpty() ? theToolTip : theText);
  aLayout->addWidget(myLabel);

  myLabelValue = new QLabel("", this);
  aLayout->addWidget(myLabelValue, 1);

  setLocale(ModuleBase_Tools::doubleLocale());
  aLayout->addStretch(1);
}

ModuleBase_LabelValue::~ModuleBase_LabelValue()
{
}

void ModuleBase_LabelValue::setValue(const double theValue)
{
  myValue = theValue;

  QString aStrValue = locale().toString(theValue, myPrecision >= 0 ? 'f' : 'g', qAbs(myPrecision));
  myLabelValue->setText(aStrValue);
  myLabelValue->setToolTip(aStrValue);
}
