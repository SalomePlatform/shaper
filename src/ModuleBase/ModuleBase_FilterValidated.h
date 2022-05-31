// Copyright (C) 2014-2022  CEA/DEN, EDF R&D
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

#ifndef ModuleBase_FilterValidated_H
#define ModuleBase_FilterValidated_H

#include <SelectMgr_Filter.hxx>
#include <SelectMgr_EntityOwner.hxx>

class ModuleBase_IWorkshop;

//#define DEBUG_FILTERS

/**
* \ingroup GUI
* \class ModuleBase_FilterValidated
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select objects which belong to 
* currently active document or to global document
*/
DEFINE_STANDARD_HANDLE(ModuleBase_FilterValidated, SelectMgr_Filter);
class ModuleBase_FilterValidated: public SelectMgr_Filter
{
public:
  /// Constructor
  /// \param theWorkshop instance of workshop interface
  Standard_EXPORT ModuleBase_FilterValidated(ModuleBase_IWorkshop* theWorkshop):
      SelectMgr_Filter(), myWorkshop(theWorkshop) {}

  /**
   * Returns true if the owner is computed from decomposition
   * (it is global selection, not the sub-shapes)
   * of if the selected result is a construction and the result 
   * feature is composite and has sub-elements.
   * \param theOwner the result of selection
   * \return whether the owner is selectable in the viewer
  */
  Standard_EXPORT virtual
    Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTIEXT(ModuleBase_FilterValidated, SelectMgr_Filter)
protected:
  /// Instance of workshop interface
  ModuleBase_IWorkshop* myWorkshop;
};

#endif