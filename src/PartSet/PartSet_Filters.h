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

#ifndef PartSet_Filters_H
#define PartSet_Filters_H

#include <ModuleBase_ViewerFilters.h>

#include <set>
#include <string>

/**
* \class PartSet_GlobalFilter
* \ingroup Modules
* A class which filters groups object in addition to documents (see issue #310)
*/
DEFINE_STANDARD_HANDLE(PartSet_GlobalFilter, ModuleBase_ShapeDocumentFilter);
class PartSet_GlobalFilter: public ModuleBase_ShapeDocumentFilter
{
public:
  /// Constructor
  /// \param theWorkshop a pointer to workshop
  PartSet_GlobalFilter(ModuleBase_IWorkshop* theWorkshop)
    : ModuleBase_ShapeDocumentFilter(theWorkshop) {}

  /// Returns True if selected presentation can be selected
  /// \param theOwner an owner of the persentation
  Standard_EXPORT virtual Standard_Boolean
    IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTIEXT(PartSet_GlobalFilter, ModuleBase_ShapeDocumentFilter)
};

/// \class PartSet_ResultGroupNameFilter
/// \ingroup Modules
/// A class which filters results with groupName() result in filter parameters
DEFINE_STANDARD_HANDLE(PartSet_ResultGroupNameFilter, SelectMgr_Filter);
class PartSet_ResultGroupNameFilter: public SelectMgr_Filter
{
public:
  /// Constructor
  /// \param theWorkshop a pointer to workshop
  PartSet_ResultGroupNameFilter(ModuleBase_IWorkshop* theWorkshop)
    : SelectMgr_Filter(), myWorkshop(theWorkshop) {}

  void setGroupNames(const std::set<std::string>& theGroupNames);

  /// Returns True if selected presentation can be selected
  /// \param theOwner an owner of the persentation
  Standard_EXPORT virtual Standard_Boolean
    IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTIEXT(PartSet_ResultGroupNameFilter, SelectMgr_Filter)

private:
  ModuleBase_IWorkshop* myWorkshop;
  std::set<std::string> myGroupNames;
};

/// \class PartSet_CirclePointFilter
/// \ingroup GUI
/// \brief A filter which provide filtering of selection in 3d viewer.
/// Installing of this filter disables selection of point on circle in sketch
DEFINE_STANDARD_HANDLE(PartSet_CirclePointFilter, SelectMgr_Filter);
class PartSet_CirclePointFilter: public SelectMgr_Filter
{
public:

  /// Constructor
  /// \param theWorkshop instance of workshop interface
  Standard_EXPORT PartSet_CirclePointFilter(ModuleBase_IWorkshop* theWorkshop):
    SelectMgr_Filter(),
    myWorkshop(theWorkshop) {};

  /// Returns True if the given owner is acceptable for selection
  /// \param theOwner the selected owner
  Standard_EXPORT virtual Standard_Boolean
    IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTIEXT(PartSet_CirclePointFilter, SelectMgr_Filter)

private:

  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;
};

#endif