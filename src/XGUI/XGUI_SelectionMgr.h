// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

#ifndef XGUI_SelectionMgr_H
#define XGUI_SelectionMgr_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <QObject>
#include <QModelIndexList>

#include <AIS_ListOfInteractive.hxx>
#include <NCollection_List.hxx>
#include <TopoDS_Shape.hxx>
#include <SelectMgr_IndexedMapOfOwner.hxx>

class XGUI_Workshop;
class XGUI_ObjectsBrowser;
class XGUI_Selection;

/**\class XGUI_SelectionMgr
 * \ingroup GUI
 * \brief Selection manager. Provides selection event on selection in 
 * Object Browser and Viewer
 */
class XGUI_EXPORT XGUI_SelectionMgr : public QObject
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent workshop
  XGUI_SelectionMgr(XGUI_Workshop* theParent);
  virtual ~XGUI_SelectionMgr();

  /// Returns current selection
  XGUI_Selection* selection() const
  {
    return mySelection;
  }

  //! Connects the manager to all viewers accessible by Workshop
  void connectViewers();

  //! Selects the owners in the context.
  //! \param theSelectedOwners a container of owners
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void setSelectedOwners(const SelectMgr_IndexedMapOfOwner& theSelectedOwners,
                         bool isUpdateViewer);

  //! Check that the selected owners are valid for the current filters
  /// \param isUpdateViewer the parameter whether the viewer should be update immediatelly
  void updateSelectedOwners(bool isUpdateViewer);

signals:
  //! Emited when selection in a one of viewers was changed
  void selectionChanged();

 private slots:
   /// Reaction on selectio0n in Object browser
  void onObjectBrowserSelection();

   /// Reaction on selectio0n in Viewer
  void onViewerSelection();

 private:
   /// Reference to workshop
  XGUI_Workshop* myWorkshop;

  /// Current selection object
  XGUI_Selection* mySelection;
};

#endif
