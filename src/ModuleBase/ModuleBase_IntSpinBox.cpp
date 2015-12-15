// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:      ModuleBase_IntSpinBox.cxx
// Author:    Natalia ERMOLAEVA
//
#include "ModuleBase_IntSpinBox.h"

#include <QKeyEvent>

ModuleBase_IntSpinBox::ModuleBase_IntSpinBox(QWidget* theParent)
: QSpinBox(theParent)
{
}

void ModuleBase_IntSpinBox::keyPressEvent(QKeyEvent *theEvent)
{
  switch (theEvent->key()) {
    case Qt::Key_Enter:
    case Qt::Key_Return: {
      // do not react to the Enter key, the property panel processes it
        return;
    }
    break;
    default:
      break;
  }
  QSpinBox::keyPressEvent(theEvent);
}

