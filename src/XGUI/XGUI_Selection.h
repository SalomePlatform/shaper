// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_Selection.h
// Created:     8 July 2014
// Author:      Vitaly SMETANNIKOV

#ifndef XGUI_Selection_H
#define XGUI_Selection_H

#include "XGUI.h"

#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_ISelection.h>

#include <QModelIndexList>
#include <AIS_ListOfInteractive.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

#include <SelectMgr_IndexedMapOfOwner.hxx>

class XGUI_Workshop;

class XGUI_EXPORT XGUI_Selection : public ModuleBase_ISelection
{
 public:
  XGUI_Selection(XGUI_Workshop* theWorkshop);

  /// Returns a list of viewer selected presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  virtual QList<ModuleBase_ViewerPrs> getSelected(int theShapeTypeToSkip = -1) const;

  /// Returns a list of viewer highlited presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  virtual QList<ModuleBase_ViewerPrs> getHighlighted(int theShapeTypeToSkip = -1) const;

  /**
   * Returns list of currently selected objects in object browser
   */
  virtual QObjectPtrList selectedObjects() const;

  /**
   * Returns list of currently selected results
   */
  virtual QObjectPtrList selectedPresentations() const;

  //! Returns list of currently selected QModelIndexes
  virtual QModelIndexList selectedIndexes() const;

  //! Returns list of currently selected AIS objects
  virtual void selectedAISObjects(AIS_ListOfInteractive& theList) const;

  //! Returns list of currently selected shapes
  virtual void selectedShapes(NCollection_List<TopoDS_Shape>& theShapes, 
    std::list<ObjectPtr>& theOwners) const;

  //! Returns list of currently selected owners
  /// \return list of owners
  void selectedOwners(SelectMgr_IndexedMapOfOwner& theSelectedOwners) const;

  //! Returns a list of selection entity owners of the interactive object
  /// It depends on the modes, in which the object is activated in the context
  /// \param theObject an object
  /// \param theOwners a map of entity owners
  void entityOwners(const Handle_AIS_InteractiveObject& theObject,
                    SelectMgr_IndexedMapOfOwner& theOwners) const;

 private:
  XGUI_Workshop* myWorkshop;
};

#endif
