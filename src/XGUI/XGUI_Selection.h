// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

// File:        XGUI_Selection.h
// Created:     8 July 2014
// Author:      Vitaly SMETANNIKOV

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
class Handle_SelectMgr_EntityOwner;
class ModuleBase_ViewerPrs;

/**
* \ingroup GUI
* Implementation of \ref ModuleBase_ISelection interface.
*/
class XGUI_EXPORT XGUI_Selection : public ModuleBase_ISelection
{
 public:
   /// Constructor
   /// \param theWorkshop reference to workshop instance
  XGUI_Selection(XGUI_Workshop* theWorkshop);

  /// Returns a list of viewer selected presentations
  /// \return list of presentations
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getSelected(const SelectionPlace& thePlace = Browser) const;

  /// Fills the viewer presentation parameters by the parameters from the owner
  /// \param thePrs a container for selection
  /// \param theOwner a selection owner
  virtual void fillPresentation(std::shared_ptr<ModuleBase_ViewerPrs>& thePrs,
                                const Handle_SelectMgr_EntityOwner& theOwner) const;

  /// Returns a list of viewer highlited presentations
  /// \return list of presentations
  virtual QList<std::shared_ptr<ModuleBase_ViewerPrs>> getHighlighted() const;

  /**
   * Returns list of currently selected objects in object browser
   */
  virtual QObjectPtrList selectedObjects() const;

  virtual void setSelectedObjects( const QObjectPtrList& ) const;

  /**
   * Returns list of currently selected results
   */
  virtual QObjectPtrList selectedPresentations() const;

  //! Returns list of currently selected QModelIndexes
  virtual QModelIndexList selectedIndexes() const;

  //! Returns list of currently selected QModelIndexes
  ObjectPtr getSelectableObject(const Handle(SelectMgr_EntityOwner)& theOwner);

  //! Returns list of currently selected AIS objects
  virtual void selectedAISObjects(AIS_ListOfInteractive& theList) const;

  //! Return a selectable object by the entity owner. It founds AIS object in the viewer
  //! and returns the corresponded object
  /// \param theOwner an entity owner
  /// \return a found object or NULL
  ObjectPtr getSelectableObject(const Handle(SelectMgr_EntityOwner)& theOwner) const;

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

  //! Return the IO from the viewer presentation.
  //! \param thePrs a selected object
  //! \return an interactive object
  virtual Handle(AIS_InteractiveObject) getIO(const std::shared_ptr<ModuleBase_ViewerPrs>& thePrs);

protected:
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
  TopoDS_Shape findAxisShape(Handle(AIS_InteractiveObject) theIO) const;

private:
  XGUI_Workshop* myWorkshop;
};

#endif
