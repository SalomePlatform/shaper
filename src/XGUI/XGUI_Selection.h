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

#include <list>

class XGUI_Workshop;

class XGUI_EXPORT XGUI_Selection: public ModuleBase_ISelection
{
public:
  XGUI_Selection(XGUI_Workshop* theWorkshop);

  /// Returns a list of viewer selected presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  virtual std::list<ModuleBase_ViewerPrs> getSelected(int theShapeTypeToSkip = -1) const;

  /// Returns a list of viewer highlited presentations
  /// \param theShapeTypeToSkip the shapes with this type will be skipped during the result list build
  /// \return list of presentations
  virtual std::list<ModuleBase_ViewerPrs> getHighlighted(int theShapeTypeToSkip = -1) const;

  /**
  * Returns list of currently selected objects
  */
  virtual QList<ObjectPtr> selectedObjects() const;

  /**
  * Returns list of currently selected results
  */
  virtual QResultList selectedResults() const;
  
  //! Returns list of currently selected QModelIndexes
  virtual QModelIndexList selectedIndexes() const;

  //! Returns list of currently selected AIS objects
  virtual void selectedAISObjects(AIS_ListOfInteractive& theList) const;

  //! Returns list of currently selected shapes
  virtual void selectedShapes(NCollection_List<TopoDS_Shape>& theList) const;

private:
  XGUI_Workshop* myWorkshop;
};

#endif