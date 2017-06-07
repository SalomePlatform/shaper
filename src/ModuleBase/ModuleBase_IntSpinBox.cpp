// Copyright (C) 2014-2017  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
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

