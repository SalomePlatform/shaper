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

#ifndef XGUI_PropertyPanelSelector_H
#define XGUI_PropertyPanelSelector_H

#include "XGUI.h"

#include <XGUI_ActiveControlSelector.h>

class ModuleBase_ModelWidget;
class XGUI_PropertyPanel;

/**
* Processing selection by the property panel.
*/
class XGUI_PropertyPanelSelector : public XGUI_ActiveControlSelector
{
  Q_OBJECT

public:
  /// Constructor
  /// \param thePanel the workshop property panel
  XGUI_EXPORT XGUI_PropertyPanelSelector(XGUI_PropertyPanel* thePanel);
  /// Destructor
  XGUI_EXPORT virtual ~XGUI_PropertyPanelSelector() {};

  /// Returns name of the selector
  XGUI_EXPORT static QString Type() { return "XGUI_PropertyPanelSelector"; }

  /// Returns name of the selector
  XGUI_EXPORT virtual QString getType() { return Type(); }

  /// Clear need to be activated widget if it exists
  XGUI_EXPORT virtual void reset();

  /// Sets control active. It should activates/deactivates selection and selection filters.
  /// \param isActive if true, the control becomes active
  XGUI_EXPORT virtual void setActive(const bool& isActive);

  /// Returns whether the selector should be activated as soon as possible (by deactivatate other)
  /// \return boolean result
  XGUI_EXPORT virtual bool needToBeActiated() const;

  /// Processes current selection of workshop. Reaction to selection change in workshop.
  XGUI_EXPORT virtual void processSelection();

protected:
  XGUI_PropertyPanel* myPanel; ///< processed panel
  ModuleBase_ModelWidget* myWidgetToBeActivated; ///< used as need to be activated back

};

#endif
