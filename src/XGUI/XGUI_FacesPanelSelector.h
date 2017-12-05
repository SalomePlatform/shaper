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

#ifndef XGUI_FacesPanelSelector_H
#define XGUI_FacesPanelSelector_H

#include "XGUI.h"

#include <XGUI_ActiveControlSelector.h>

class XGUI_FacesPanel;

/**
* Processing selection by the faces panel.
*/
class XGUI_FacesPanelSelector : public XGUI_ActiveControlSelector
{
  Q_OBJECT

public:
  /// Constructor
  /// \param thePanel the workshop faces panel
  XGUI_EXPORT XGUI_FacesPanelSelector(XGUI_FacesPanel* thePanel);
  /// Destructor
  XGUI_EXPORT virtual ~XGUI_FacesPanelSelector() {};

  /// Returns name of the selector
  XGUI_EXPORT static QString Type() { return "XGUI_FacesPanelSelector"; }

  /// Returns name of the selector
  XGUI_EXPORT virtual QString getType() { return Type(); }

  /// Set empty widget that need to be activated widget if it is not empty
  XGUI_EXPORT virtual void reset();

  /// Sets control active. It should activates/deactivates selection and selection filters.
  /// \param isActive if true, the control becomes active
  XGUI_EXPORT virtual void setActive(const bool& isActive);

  /// Processes current selection of workshop. Reaction to selection change in workshop.
  XGUI_EXPORT virtual void processSelection();

protected:
  XGUI_FacesPanel* myPanel; ///< processed panel
};

#endif
