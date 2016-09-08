// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        XGUI_DeflectionDialog.cpp
// Created:     27 Jul 2016
// Author:      Natalia ERMOLAEVA

#include <XGUI_DeflectionDialog.h>

#include <ModelAPI_Tools.h>

#include <ModuleBase_DoubleSpinBox.h>

#include <QLabel>
#include <QButtonGroup>
#include <QGridLayout>
#include <QRadioButton>
#include <QDialogButtonBox>

XGUI_DeflectionDialog::XGUI_DeflectionDialog(QWidget* theParent)
  : QDialog(theParent)
{
  setWindowTitle(tr("Deflection"));
  QGridLayout* aLay = new QGridLayout(this);

  myDeflection = new ModuleBase_DoubleSpinBox(this);
  myDeflection->setDecimals(12);
  myDeflection->setRange(0, 1);
  myDeflection->setSingleStep(0.001);
  aLay->addWidget(myDeflection, 0, 0);

  QDialogButtonBox* aButtons = new QDialogButtonBox(QDialogButtonBox::Ok | QDialogButtonBox::Cancel,
                                                    Qt::Horizontal, this);
  connect(aButtons, SIGNAL(accepted()), this, SLOT(accept()));
  connect(aButtons, SIGNAL(rejected()), this, SLOT(reject()));
  aLay->addWidget(aButtons, 1, 0);
}

void XGUI_DeflectionDialog::setDeflection(const double& theValue)
{
  myDeflection->setValue(theValue);
}

double XGUI_DeflectionDialog::getDeflection() const
{
  return myDeflection->value();
}
