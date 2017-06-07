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
// See http://www.salome-platform.org/ or
// email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef MODULEBASE_MODELDIALOGWIDGET_H
#define MODULEBASE_MODELDIALOGWIDGET_H

#include "ModuleBase.h"
#include "ModuleBase_ModelWidget.h"

class QDialogButtonBox;

/// \class ModuleBase_ModelDialogWidget
/// \ingroup GUI
/// \brief Widget for dialog.
class MODULEBASE_EXPORT ModuleBase_ModelDialogWidget : public ModuleBase_ModelWidget
{
Q_OBJECT
public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  /// a low-level API for reading xml definitions of widgets
  ModuleBase_ModelDialogWidget(QWidget* theParent, const Config_WidgetAPI* theData) :
                                ModuleBase_ModelWidget(theParent, theData) {}

  /// Set general buttons from dialog
  /// \param theButtons the dialog buttons
  void setDialogButtons(QDialogButtonBox* theButtons) { myOkCancelBtn = theButtons; }

protected:

  /// Contains dialog buttons to enable/disable Ok and Cancel buttons
  QDialogButtonBox* myOkCancelBtn;
};

#endif