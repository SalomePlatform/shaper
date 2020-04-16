// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#ifndef MODULEBASE_INT_SPINBOX_H_
#define MODULEBASE_INT_SPINBOX_H_

#include "ModuleBase.h"

#include <QSpinBox>

class QWidget;
class QKeyEvent;

/**
  * \ingroup GUI
  * Enhanced version of the Qt's int spin box.
  * It allows to store modified state
*/
class MODULEBASE_EXPORT ModuleBase_IntSpinBox : public QSpinBox
{
Q_OBJECT

public:
  /// Constructor
  /// \param theParent a parent widget
  explicit ModuleBase_IntSpinBox(QWidget* theParent = 0);
  virtual ~ModuleBase_IntSpinBox() {};

protected:
  /// Called on key press event
  virtual void keyPressEvent(QKeyEvent* theEvent);
};

#endif
