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

#ifndef PartSet_FilterInfinite_H
#define PartSet_FilterInfinite_H

#include <ModuleBase_ViewerFilters.h>

#include <SelectMgr_Filter.hxx>

class ModuleBase_IWorkshop;

/**
* \class PartSet_FilterInfinite
* \ingroup Modules
* A class which filters AIS objects with the true infinite state
*/
DEFINE_STANDARD_HANDLE(PartSet_FilterInfinite, SelectMgr_Filter);
class PartSet_FilterInfinite : public SelectMgr_Filter
{
public:
  /// Constructor
  /// \param theWorkshop a pointer to workshop
  PartSet_FilterInfinite(ModuleBase_IWorkshop* theWorkshop);

  /// Returns True if selected presentation can be selected
  /// \param theOwner an owner of the persentation
  Standard_EXPORT virtual Standard_Boolean
    IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTIEXT(PartSet_FilterInfinite, SelectMgr_Filter)

protected:
  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;
};

#endif