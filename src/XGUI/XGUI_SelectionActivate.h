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

#ifndef XGUI_ISelectionActivate_H
#define XGUI_ISelectionActivate_H

#include "XGUI.h"

#include "ModuleBase_Definitions.h"
#include "ModuleBase_ISelectionActivate.h"

#include "SelectMgr_ListOfFilter.hxx"

#include <memory>

class AIS_InteractiveContext;
class AIS_InteractiveObject;
class ModelAPI_Object;

class XGUI_Displayer;

/// \ingroup GUI
/// A class which implements activation/deactivate selection modes and using selection filters.
class XGUI_SelectionActivate : public ModuleBase_ISelectionActivate
{
public:
  /// Types of the activation place
  enum SelectionPlace { Workshop, PropertyPanel, FacesPanel };

public:
  /// Constructor
  XGUI_EXPORT XGUI_SelectionActivate(ModuleBase_IWorkshop* theWorkshop);

  /// Destructor
  ~XGUI_SelectionActivate() {}

  /// Returns place of activation modes that now is active
  /// \return place
  SelectionPlace activeSelectionPlace() const;

  /// Updates active selection modes in the viewer depending on the application state
  XGUI_EXPORT virtual void updateSelectionModes();

  /// Updates active selection filters in the viewer depending on the application state
  XGUI_EXPORT virtual void updateSelectionFilters();

  /// Activates parameter filters in the workshop, deactivate active out of the container
  /// Please find a possibility to use updateSelectionFilters instead of direct call this method.
  /// \param theSelectionFilters a filtes
  XGUI_EXPORT virtual void activateSelectionFilters
    (const SelectMgr_ListOfFilter& theSelectionFilters);

  /// Returns list of currently active selection modes
  /// Selection modes will be returned according to TopAbs_ShapeEnum
  XGUI_EXPORT QIntList activeSelectionModes() const;

  /// Returns true if the given object can be selected
  /// \param theObject object to check
  XGUI_EXPORT bool isActive(ObjectPtr theObject) const;

  /// Activates in local context displayed outside of the context.
  /// \param theModes - modes on which it has to be activated (can be empty)
  /// \param theObjList - list of objects which has to be activated.
  /// \param theUpdateViewer an update viewer flag
  XGUI_EXPORT void activateObjects(const QIntList& theModes, const QObjectPtrList& theObjList,
                                   const bool theUpdateViewer = true);

  /// Fill container of current selection modes
  /// \param theModes selection modes
  XGUI_EXPORT void setSelectionModes(const QIntList& theModes);

  /// Activate object in the selection modes
  /// \param theIO an object
  /// \param theUpdateViewer an update viewer flag
  XGUI_EXPORT void activateOnDisplay(const Handle(AIS_InteractiveObject)& theIO,
                                     const bool theUpdateViewer);

  /// Activate interactive object
  /// \param theIO an interactive object
  /// \param theMode activation mode
  /// \param theUpdateViewer update viewer flag
  XGUI_EXPORT void activateAIS(const Handle(AIS_InteractiveObject)& theIO, const int theMode,
                               const bool theUpdateViewer) const;

  /// Activate interactive object. It is necessary to call ClearOutdatedSelection
  /// after deactivation
  /// \param theIO an interactive object
  /// \param theMode a mode to deactivate. When theMode=-1 then all modes will be deactivated
  XGUI_EXPORT void deactivateAIS(const Handle(AIS_InteractiveObject)& theIO,
                                 const int theMode = -1) const;

  /// Activates the interactive object in the local context.
  /// \param theIO an interactive object
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  /// \return a flag is object activated or not
  XGUI_EXPORT bool activate(const Handle(AIS_InteractiveObject)& theIO,
                            const bool theUpdateViewer) const;

  /// Deactivates the given object (not allow selection)
  /// \param theObject object to deactivate
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  XGUI_EXPORT void deactivate(const std::shared_ptr<ModelAPI_Object>& theObject,
                              const bool theUpdateViewer);

  /// Returns true if the trihedron should be activated in current selection modes
  bool isTrihedronActive() const { return myIsTrihedronActive; }

  /// Set trihedron active (used in selection) or non active
  XGUI_EXPORT void activateTrihedron(bool theIsActive);

  /// Find a trihedron in a list of displayed presentations and deactivate it.
  /// \param theUpdateViewer the parameter whether the viewer should be update immediatelly
  XGUI_EXPORT void deactivateTrihedron(const bool theUpdateViewer) const;

  /// Get selection modes of trihedron and deactivate it in it.
  XGUI_EXPORT void deactivateTrihedronInSelectionModes();

protected:
  /// Returns selection modes of the widget
  /// \param theWidget model widget
  /// \param theModes selection modes
  void getSelectionModes(ModuleBase_ModelWidget* theWidget, QIntList& theModes);

  /// Returns selection filters of the widget
  /// \param theWidget model widget
  /// \param theSelectionFilters selection filters
  void getSelectionFilters(ModuleBase_ModelWidget* theWidget,
                           SelectMgr_ListOfFilter& theSelectionFilters);

  /// Returns Trihedron object if it is displayed
  Handle(AIS_InteractiveObject) getTrihedron() const;

  /// Returns context of the 3D viewer
  /// \return context instance
  Handle(AIS_InteractiveContext) AISContext() const;

  /// Returns displayer
  /// \return displayer
  XGUI_Displayer* getDisplayer() const;

  /// Returns AIS object displayed in 3D viewer for the given model object
  /// \param theObject source object
  /// \returns interactive object
  AISObjectPtr getDisplayedAISObject(std::shared_ptr<ModelAPI_Object> theObject) const;

  /// Converts shape type (TopAbs_ShapeEnum) to selection mode
  /// \param theShapeType a shape type from TopAbs_ShapeEnum
  static int getSelectionMode(int theShapeType);

protected:
  QIntList myActiveSelectionModes; ///< Current activated selection modes
  bool myIsTrihedronActive; ///< Flag: use trihedgon for selection or not
};

#endif
