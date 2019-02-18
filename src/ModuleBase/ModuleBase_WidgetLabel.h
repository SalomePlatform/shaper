// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
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

#ifndef ModuleBase_WidgetLabel_H
#define ModuleBase_WidgetLabel_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class QLabel;

/**
* \ingroup GUI
* Implementation of model widget for a label control
*/
class MODULEBASE_EXPORT ModuleBase_WidgetLabel : public ModuleBase_ModelWidget
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuation. The attribute of the model widget is obtained from
  ModuleBase_WidgetLabel(QWidget* theParent, const Config_WidgetAPI* theData);

  virtual ~ModuleBase_WidgetLabel();

  /// Defines if it is supported to set the value in this widget
  /// It returns false because this is an info widget
  virtual bool canAcceptFocus() const { return false; };

  virtual bool restoreValueCustom();

  virtual QList<QWidget*> getControls() const;

  /// This control doesn't accept focus
  virtual bool focusTo();

protected:
  /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom()
  {
    return true;
  }

  /// A label control
  QLabel* myLabel;
};

#endif
