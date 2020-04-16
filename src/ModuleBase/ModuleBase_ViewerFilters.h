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

#ifndef ModuleBase_ViewerFilters_H
#define ModuleBase_ViewerFilters_H

#include <QStringList>

#include <SelectMgr_Filter.hxx>
#include <SelectMgr_EntityOwner.hxx>

#include <GeomAPI_Pln.h>


class ModuleBase_IWorkshop;


/**
* \class ModuleBase_ShapeDocumentFilter
* \ingroup GUI
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select objects which belong to 
* currently active document or to global document
*/
DEFINE_STANDARD_HANDLE(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter);
class ModuleBase_ShapeDocumentFilter: public SelectMgr_Filter
{
public:
  /// Constructor
  /// \param theWorkshop instance of workshop interface
  Standard_EXPORT
    ModuleBase_ShapeDocumentFilter(ModuleBase_IWorkshop* theWorkshop): SelectMgr_Filter(),
    myWorkshop(theWorkshop) {}

  /// Returns True if the given owner is acceptable for selection
  /// \param theOwner the selected owner
  Standard_EXPORT virtual
    Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  /// Add an object type name to list of non selectable objects
  /// \param theType - a name of an object type
  Standard_EXPORT void addNonSelectableType(const QString& theType)
  {
    if (!myNonSelectableTypes.contains(theType))
      myNonSelectableTypes.append(theType);
  }

  /// Removes an object type name from list of non selectable objects
  /// \param theType - a name of an object type
  Standard_EXPORT void removeNonSelectableType(const QString& theType)
  {
    if (myNonSelectableTypes.contains(theType))
      myNonSelectableTypes.removeAll(theType);
  }

  /// Returns list of non-selectable an object type names
  Standard_EXPORT QStringList nonSelectableTypes() const
  {
    return myNonSelectableTypes;
  }

  DEFINE_STANDARD_RTTIEXT(ModuleBase_ShapeDocumentFilter, SelectMgr_Filter)

protected:
  /// Reference to workshop
  ModuleBase_IWorkshop* myWorkshop;

  QStringList myNonSelectableTypes;
};

/**
* \class ModuleBase_ShapeInPlaneFilter
* \ingroup GUI
* A filter which provides filtering of selection in 3d viewer.
* Installing of this filter lets to select of Vertexes and Edges which belongs to the given plane
*/
DEFINE_STANDARD_HANDLE(ModuleBase_ShapeInPlaneFilter, SelectMgr_Filter);
class ModuleBase_ShapeInPlaneFilter: public SelectMgr_Filter
{
public:
  /// Constructor
  Standard_EXPORT ModuleBase_ShapeInPlaneFilter(): SelectMgr_Filter() {}

  /// Set working plane
  /// \param thePlane a plane object
  void setPlane(const std::shared_ptr<GeomAPI_Pln>& thePlane) { myPlane = thePlane; }

  /// Returns current working plane
  std::shared_ptr<GeomAPI_Pln> plane() const { return myPlane; }

  /// Returns True if the given owner is acceptable for selection
  /// \param theOwner the selected owner
  Standard_EXPORT virtual
    Standard_Boolean IsOk(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  DEFINE_STANDARD_RTTIEXT(ModuleBase_ShapeInPlaneFilter, SelectMgr_Filter)
private:
  /// Working plane
  std::shared_ptr<GeomAPI_Pln> myPlane;
};

#endif
