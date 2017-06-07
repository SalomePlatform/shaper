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
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com<mailto:webmaster.salome@opencascade.com>
//

#ifndef XGUI_SelectionMgr_H
#define XGUI_SelectionMgr_H

#include "XGUI.h"
#include <ModuleBase_Definitions.h>
#include <ModuleBase_ISelection.h>
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

  //! Clears selection in Viewer and object Browser
  void clearSelection();

  //! Sets values selected in both, ObjectBrowser and V3d viewer
  //! Selection is not synchronized between these controls.
  //! \param theValues a container of values to be selected.
  void setSelected(const QList<std::shared_ptr<ModuleBase_ViewerPrs> >& theValues);

  /// Updates selection, which are depend on the selection in the given place
  /// \param thePlace a widget where selection has happened.
  void updateSelectionBy(const ModuleBase_ISelection::SelectionPlace& thePlace);

signals:
  //! Emited when selection in a one of viewers was changed
  void selectionChanged();

 public slots:
   /// Reaction on selectio0n in Object browser
  void onObjectBrowserSelection();

   /// Reaction on selectio0n in Viewer
  void onViewerSelection();

private:
  /// Interates through the values to prepare container of objects that may be selected in OB
  /// \param theValues selection information
  /// \param theObjecs an output container
  void convertToObjectBrowserSelection(
       const QList<std::shared_ptr<ModuleBase_ViewerPrs> >& theValues, QObjectPtrList& theObjects);

private:
   /// Reference to workshop
  XGUI_Workshop* myWorkshop;

  /// Current selection object
  XGUI_Selection* mySelection;
};

#endif
