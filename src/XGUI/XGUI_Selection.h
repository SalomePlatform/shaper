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

#ifndef XGUI_Selection_H
#define XGUI_Selection_H

#include "XGUI.h"

#include <ModuleBase_Definitions.h>
#include <ModuleBase_ISelection.h>

#include <QModelIndexList>
#include <AIS_ListOfInteractive.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

#include <SelectMgr_IndexedMapOfOwner.hxx>

class XGUI_Workshop;
class SelectMgr_EntityOwner;
class ModuleBase_ViewerPrs;

/// \ingroup GUI
/// Implementation of \ref ModuleBase_ISelection interface.
class XGUI_EXPORT XGUI_Selection : public ModuleBase_ISelection
{
 public:
   /// Constructor
   /// \param theWorkshop reference to workshop instance
  XGUI_Selection(XGUI_Workshop* theWorkshop);

  /// Returns a list of viewer selected presentations
  /// \return list of presentations
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>>
    getSelected(const SelectionPlace& thePlace = Browser) const;

  /// Fills the viewer presentation parameters by the parameters from the owner
  /// \param thePrs a container for selection
  /// \param theOwner a selection owner
  virtual void fillPresentation(std::shared_ptr<ModuleBase_ViewerPrs>& thePrs,
                                const Handle(SelectMgr_EntityOwner)& theOwner) const;

  /// Returns a list of viewer highlited presentations
  /// \return list of presentations
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getHighlighted() const;

  /// Returns list of currently selected objects in object browser
  virtual QObjectPtrList selectedObjects() const;

  /// Returns list of currently selected results
  virtual QObjectPtrList selectedPresentations() const;

  /// Returns list of currently selected QModelIndexes
  virtual QModelIndexList selectedIndexes() const;

  /// Returns list of currently selected owners
  /// \return list of owners
  void selectedOwners(SelectMgr_IndexedMapOfOwner& theSelectedOwners) const;

  /// Returns a list of selection entity owners of the interactive object
  /// It depends on the modes, in which the object is activated in the context
  /// \param theObject an object
  /// \param theOwners a map of entity owners
  void entityOwners(const Handle(AIS_InteractiveObject)& theObject,
                    SelectMgr_IndexedMapOfOwner& theOwners) const;

  /// Return the IO from the viewer presentation.
  /// \param thePrs a selected object
  /// \return an interactive object
  virtual Handle(AIS_InteractiveObject) getIO(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

protected:
  /// Return a selectable object by the entity owner. It founds AIS object in the viewer
  /// and returns the corresponded object
  /// \param theOwner an entity owner
  /// \return a found object or NULL
  ObjectPtr getSelectableObject(const Handle(SelectMgr_EntityOwner)& theOwner) const;

  /// Fills the list of presentations by objects selected in the viewer.
  /// \param thePresentations an output list of presentation
  void getSelectedInViewer(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& thePresentations) const;

  /// Fills the list of presentations by objects selected in the object browser.
  /// ViewerPrs contains only object parameter not empty.
  /// If the given list of presentations already has a viewer presentation with the same object
  /// as selected in the browser, a new item is not appended to the list of presentations.
  /// \param thePresentations an output list of presentation
  void getSelectedInBrowser(QList<std::shared_ptr<ModuleBase_ViewerPrs>>& thePresentations) const;

  /// Generates a vertex or edge by the give IO if it is an AIS created on trihedron
  /// \param theIO a selected object
  /// \return created shape or empty shape
#ifdef BEFORE_TRIHEDRON_PATCH
  TopoDS_Shape findAxisShape(Handle(AIS_InteractiveObject) theIO) const;
#endif

private:
  XGUI_Workshop* myWorkshop; ///< current workshop
};

#endif
