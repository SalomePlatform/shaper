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

#ifndef PartSet_WidgetEditor_H
#define PartSet_WidgetEditor_H

#include "PartSet.h"

#include <ModuleBase_WidgetEditor.h>

class ModuleBase_IWorkshop;

/**
* \ingroup Modules
* Customization of ModuleBase_WidgetEditor in order to do not show the editor value if the mouse
* cursor is not over the OCC window
*/
class PARTSET_EXPORT PartSet_WidgetEditor : public ModuleBase_WidgetEditor
{
Q_OBJECT
 public:
  /// Constructor
  /// \param theParent the parent object
  /// \param theWorkshop instance of workshop interface
  /// \param theData the widget configuration. The attribute of the model widget is obtained from
  PartSet_WidgetEditor(QWidget* theParent, ModuleBase_IWorkshop* theWorkshop,
                       const Config_WidgetAPI* theData);

  virtual ~PartSet_WidgetEditor() {}

  /// Activates the editor control only in case if the mouse over the OCC window, otherwise
  /// set focus to the usual double value control
  /// \return the state whether the widget can accept the focus
  virtual bool focusTo();

private:
  ModuleBase_IWorkshop* myWorkshop; // the current workshop
};

#endif
