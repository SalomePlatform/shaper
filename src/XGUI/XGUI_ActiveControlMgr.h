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

#ifndef XGUI_ActiveControlMgr_H
#define XGUI_ActiveControlMgr_H

#include "XGUI.h"

#include <QList>
#include <QObject>


class XGUI_ActiveControlSelector;
class ModuleBase_IWorkshop;

/**
* Interface of providing only one active control for workshop.
* It has container of selectors, where only one might be active at the moment.
* Selection in 3D view is processed by the active selector.
*/
class XGUI_ActiveControlMgr : public QObject
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theWorkshop the current workshop instance
  XGUI_EXPORT XGUI_ActiveControlMgr(ModuleBase_IWorkshop* theWorkshop);

  XGUI_EXPORT virtual ~XGUI_ActiveControlMgr() {};

  /// Register selector to process activation of control
  void addSelector(XGUI_ActiveControlSelector* theSelector);

  /// Returns selector by type name
  /// \param theType a selector type
  /// \return selector instance
  XGUI_EXPORT XGUI_ActiveControlSelector* getSelector(const QString& theType);

  /// Returns the active selector
  /// \return selector instance
  XGUI_ActiveControlSelector* activeSelector() const { return myActiveSelector; }

  void deactivateSelector(XGUI_ActiveControlSelector* theSelector);

protected slots:
  /// Deactivates active selector and set the sender selector as active
  void onSelectorActivated();
  /// Deactivate the active selector
  void onSelectorDeactivated();
  /// Listens workshop selection and pass it to the active selector
  void onSelectionChanged();

protected:
  void activateSelector(XGUI_ActiveControlSelector* theSelector);

protected:
  ModuleBase_IWorkshop* myWorkshop; ///< the current workshop

  QList<XGUI_ActiveControlSelector*> mySelectors; ///< workshop selectors
  XGUI_ActiveControlSelector* myActiveSelector; ///< active selector

  bool myIsBlocked; ///< blocking flag to avoid cycling signals processing
};

#endif
