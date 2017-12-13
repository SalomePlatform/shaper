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

#ifndef ModuleBase_ISelectionActivate_H
#define ModuleBase_ISelectionActivate_H

#include "ModuleBase.h"

#include "ModuleBase_IWorkshop.h"

#include <SelectMgr_ListOfFilter.hxx>

class ModuleBase_ModelWidget;


/// \ingroup GUI
/// A class which provides interface of activation/deactivate selection modes
/// and using selection filters.
class ModuleBase_ISelectionActivate
{
public:
  /// Constructor
  ModuleBase_ISelectionActivate(ModuleBase_IWorkshop* theWorkshop) : myWorkshop(theWorkshop) {}

  /// Destructor
  ~ModuleBase_ISelectionActivate() {}

  /// Updates active selection modes in the viewer depending on the application state
  MODULEBASE_EXPORT virtual void updateSelectionModes() = 0;

  /// Updates active selection filters in the viewer depending on the application state
  MODULEBASE_EXPORT virtual void updateSelectionFilters() = 0;

  /// Activates parameter filters in the workshop, deactivate active out of the container
  /// Please find a possibility to use updateSelectionFilters instead of direct call this method.
  /// \param theSelectionFilters a filtes
  MODULEBASE_EXPORT virtual void activateSelectionFilters
    (const SelectMgr_ListOfFilter& theSelectionFilters) = 0;

protected:
  ModuleBase_IWorkshop* myWorkshop; ///< active workshop
};

#endif
