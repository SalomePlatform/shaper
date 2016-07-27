// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_DeflectionDialog.cpp
// Created:     27 Jul 2016
// Author:      Natalia ERMOLAEVA

#include <XGUI_DeflectionDialog.h>

#include <ModelAPI_Tools.h>

#include <QDoubleSpinBox>

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>
#include <QDialogButtonBox>

XGUI_DeflectionDialog::XGUI_DeflectionDialog(QWidget* theParent)
  : QDialog(theParent)
{
  setWindowTitle("Deflection");
  QGridLayout* aLay = new QGridLayout(this);

  myDeflection = new QDoubleSpinBox(this);
  myDeflection->setRange(0, 1);
  aLay->addWidget(new QLabel("Deviation", this), 0, 0);
  aLay->addWidget(myDeflection, 0, 1);

  QDialogButtonBox* aButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                    Qt::Horizontal, this);
  connect(aButtons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), this, SLOT(reject()));
  aLay->addWidget(aButtons, 1, 0, 1, 2);
}

void XGUI_DeflectionDialog::setDeflection(const double& theValue)
{
  myDeflection->setValue(theValue);
}

double XGUI_DeflectionDialog::getDeflection() const
{
  return myDeflection->value();
}
