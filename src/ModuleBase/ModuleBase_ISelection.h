// File:        ModuleBase_ISelection.h
// Created:     2 June 2014
// Author:      Vitaly Smetannikov

#ifndef ModuleBase_ISelection_H
#define ModuleBase_ISelection_H

#include "ModuleBase.h"
#include "ModuleBase_Definitions.h"
#include "ModuleBase_ViewerPrs.h"

#include <QModelIndexList>
#include <AIS_ListOfInteractive.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>

#include <QList>

class ModuleBase_ISelection
{
 public:

  /// Returns a list of viewer selected presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  virtual QList<ModuleBase_ViewerPrs> getSelected(int theShapeTypeToSkip = -1) const = 0;

  /// Returns a list of viewer highlited presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  virtual QList<ModuleBase_ViewerPrs> getHighlighted(int theShapeTypeToSkip = -1) const = 0;

  /**
   * Returns list of features currently selected in 3d viewer
   */
  virtual QList<ObjectPtr> selectedObjects() const = 0;

  /**
   * Returns list of currently selected results
   */
  virtual QList<ObjectPtr> selectedPresentations() const = 0;

  //! Returns list of currently selected QModelIndexes
  virtual QModelIndexList selectedIndexes() const = 0;

  //! Returns list of currently selected AIS objects
  virtual void selectedAISObjects(AIS_ListOfInteractive& theList) const = 0;

  //! Returns list of currently selected shapes
  virtual void selectedShapes(NCollection_List<TopoDS_Shape>& theList, 
    std::list<ObjectPtr>& theOwners) const = 0;

};

#endif
