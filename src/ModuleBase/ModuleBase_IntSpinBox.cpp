// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_IntSpinBox.cxx
// Author:    Natalia ERMOLAEVA
//
#include "ModuleBase_IntSpinBox.h"

#include <QKeyEvent>

ModuleBase_IntSpinBox::ModuleBase_IntSpinBox(QWidget* theParent)
: QSpinBox(theParent),
  myIsModified(false)
{
  connect(this, SIGNAL(valueChanged(const QString&)), this, SLOT(onValueChanged(const QString&)));
}

void ModuleBase_IntSpinBox::onValueChanged(const QString& theValue)
{
  myIsModified = true;
}

bool ModuleBase_IntSpinBox::isModified() const
{
  return myIsModified;
}

void ModuleBase_IntSpinBox::clearModified()
{
  myIsModified = false;
}
