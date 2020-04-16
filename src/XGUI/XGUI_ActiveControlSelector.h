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

#ifndef XGUI_ActiveControlSelector_H
#define XGUI_ActiveControlSelector_H

#include "XGUI.h"

#include <QObject>

/**
* Interface to have an active control and process selection by the control.
* Activation of control may set selection modes and selection filters of the control.
*/
class XGUI_ActiveControlSelector : public QObject
{
  Q_OBJECT

public:
  /// Constructor
  /// \param theWorkshop the current workshop instance
  XGUI_EXPORT XGUI_ActiveControlSelector() {};
  /// Destructor
  XGUI_EXPORT virtual ~XGUI_ActiveControlSelector() {};

  /// Returns name of the selector
  XGUI_EXPORT virtual QString getType() = 0;

  /// Clear need to be activated widget if it exists
  XGUI_EXPORT virtual void reset() {}

  /// Sets enable/disable state of the selector. If disable, it will not react to selection
  /// \param theEnabled if true, selector is enabled
  XGUI_EXPORT void setEnable(const bool& theEnabled) { myIsEnabled = theEnabled; }

  /// Returns whether the selector is enabled or not
  /// \return boolean result
  XGUI_EXPORT bool isEnabled() const { return myIsEnabled; }

  /// Sets control active. It should activates/deactivates selection and selection filters.
  /// \param isActive if true, the control becomes active
  XGUI_EXPORT virtual void setActive(const bool& isActive) = 0;

  /// Returns whether the selector should be activated as soon as possible (by deactivatate other)
  /// \return boolean result
  XGUI_EXPORT virtual bool needToBeActivated() const { return false; }

  /// Processes current selection of workshop. Reaction to selection change in workshop.
  XGUI_EXPORT virtual void processSelection() = 0;

signals:
  /// control is activated
  void activated();
  /// control is deactivated
  void deactivated();

protected:
  bool myIsEnabled; ///< enable state of the selector
};

#endif
