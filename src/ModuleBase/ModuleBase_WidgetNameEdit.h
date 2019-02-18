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

#ifndef MODULEBASE_WIDGETNAMEEDIT_H_
#define MODULEBASE_WIDGETNAMEEDIT_H_

#include <ModuleBase.h>
#include <ModuleBase_WidgetLineEdit.h>

class ModuleBase_WidgetNameEdit : public ModuleBase_WidgetLineEdit
{
  Q_OBJECT
public:
  ModuleBase_WidgetNameEdit(QWidget* theParent,
                            const Config_WidgetAPI* theData,
                            const std::string& thePlaceHolder)
      : ModuleBase_WidgetLineEdit(theParent, theData, thePlaceHolder) {}

  /// Returns True if the widget uses feature attribute.
  /// If not then it means that the widget do not need attribute at all.
  virtual bool usesAttribute() const { return false; }

  virtual bool focusTo() { return false; }

protected:
    /// Saves the internal parameters to the given feature
  /// \return True in success
  virtual bool storeValueCustom();

  /// Redefinition of virtual method
  virtual bool restoreValueCustom();
};

#endif