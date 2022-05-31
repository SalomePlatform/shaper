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

#ifndef XGUI_FacesPanel_H_
#define XGUI_FacesPanel_H_

#include "XGUI.h"

#include <ModuleBase_ActionType.h>
#include <ModuleBase_Definitions.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_ResultPrs.h>

#include <ModelAPI_Object.h>
#include <ModelAPI_ResultGroup.h>
#include <ModelAPI_Feature.h>

#include <GeomAPI_AISObject.h>

#include <SelectMgr_ListOfFilter.hxx>
#include <TopoDS_Shape.hxx>

#include <QDockWidget>
#include <QObject>
#include <QMap>

#include <set>

class AIS_InteractiveObject;

class GeomAPI_AISObject;

class XGUI_Workshop;
class ModuleBase_ListView;

class QAction;
class QCheckBox;
class QEvent;

/**
* \ingroup GUI
* A Hide Faces panel for making it possible to hide faces in the 3D view.
* The panel has multi-selector filled by faces elements. When the control is active
* it is possible to select faces in the viewer. The selected faces are hidden/transparent
* after selection and the corresponding item is appeared in the multi selector.
*
* In order to redisplay a face, it is enough to click delete on the name of this face
* in the multiselector.
* When the panel is opened, the multiselector is empty.
* When the panel is closed, the multiselector is emptied and the faces are displayed again.
* The default position by of this dockable window is to the right of the view (in SALOME mode).
* If no feature is processed (in neutral point), this panel can be activated too.
* On feature edition start or finish, movement of the history line, undo/redo and other
* modification of the model, the multiselector is emptied.
*/
class XGUI_EXPORT XGUI_FacesPanel : public QDockWidget
{
  Q_OBJECT
public:
  /// Constructor
  /// \param theParent is a parent of the property panel
  XGUI_FacesPanel(QWidget* theParent, XGUI_Workshop* theWorkshop);
  ~XGUI_FacesPanel() {}

  /// Clear content of list widget
  /// \param isToFlushRedisplay flag if redisplay should be flushed immediatelly
  virtual void reset(const bool isToFlushRedisplay);

  /// Returns whether faces panel contains elements
  /// \return boolean value
  bool isEmpty() const;

  /// Fills container with the panel selection mode: FACE
  // \param theModes [out] a container of modes
  void selectionModes(QIntList& theModes);

  /// Appends into container of workshop selection filters
  /// \param [out] selection filters
  void selectionFilters(SelectMgr_ListOfFilter& theSelectionFilters);

  /// Returns whether the panel is active or not
  bool isActivePanel() const { return myIsActive; }

  /// Stores the state if panel is active and highlight the panel in an active color
  /// \param theIsActive state whether the panel should be activated or deactivated
  void setActivePanel(const bool theIsActive);

  /// Returns true if the object is in internal container of hidden objects by this panel
  /// \param theObject a checked object
  /// \return boolean value
  bool isObjectHiddenByPanel(const ObjectPtr& theObject) const
  { return myHiddenObjects.find(theObject) != myHiddenObjects.end(); }

  /// Removed faces of the objects from the panel
  /// \param container of objects
  void restoreObjects(const std::set<ObjectPtr >& theHiddenObjects);

  /// Returns true if the event is processed. The default implementation is empty, returns false.
  virtual bool processAction(ModuleBase_ActionType theActionType);

  /// Append selected item in the list and customize presentations to hide faces
  void processSelection();

  /// Deletes item in a list of elements
  /// \return whether the delete action is processed
  bool processDelete();

  /// Delete last(s) item(s) added since the recent activation of the faces panel
  void processUndo();

  /// Processing focus in/out for the faces control
  /// \param theObject source object of event
  /// \param theEvent an event
  virtual bool eventFilter(QObject* theObject, QEvent *theEvent);

  XGUI_Workshop* workshop() const { return myWorkshop; }

public slots:
  /// Slot called on an object erase
  void onObjectDisplay(ObjectPtr theObject, AISObjectPtr theAIS);

protected:
  /// Reimplementation to emit a signal about the panel close
  virtual void closeEvent(QCloseEvent* theEvent);

signals:
  /// Signal about activating pane
  void activated();
  /// Signal about deactivating pane
  void deactivated();
  /// Signal is emitted by the top widget cross button click
  void closed();

private:
  /// Redisplay objects.
  /// \param theObjects container of objects
  /// \return true if some of objects was redisplayed
  static bool redisplayObjects(const std::set<ObjectPtr>& theObjects);

  /// Container of objects participating in the panel, it is filled by internal container
  /// \param theItems container of selected values
  /// \param theObjects [out] container of objects
  static void updateProcessedObjects(QMap<int, ModuleBase_ViewerPrsPtr> theItems,
                                     std::set<ObjectPtr>& theObjects);

  /// Returns maps of shapes and presentations. If object is a body result then it returns
  /// its ruslts. If it is a group then it returns result of shapes included into the gropup
  /// The function doesn't clear content of input maps.
  /// \param thePrs a selected presintation
  /// \param theObjectsToShapes map of objects to shapes list
  /// \param theObjectToPrs map of objects to presentations
  void getObjectsMapFromResult(ResultGroupPtr theResGroup, FeaturePtr theGroupFeature,
    std::map<ObjectPtr, TopoDS_ListOfShape>& theObjectsToShapes,
    std::map<ObjectPtr, Handle(ModuleBase_ResultPrs) >& theObjectToPrs);

  void getObjectsMapFromPrs(ModuleBase_ViewerPrsPtr thePrs,
    std::map<ObjectPtr, TopoDS_ListOfShape>& theObjectToShapes,
    std::map<ObjectPtr, Handle(ModuleBase_ResultPrs) >& theObjectToPrs);

  /// Returns true if transparency choice is checked
  /// \return boolean value
  bool useTransparency() const;

  double transparency() const;

  void removeItems(std::set<int> theIds);

protected slots:
  /// Deletes element in list of items
  void onDeleteItem();

  /// Upates hidden faces to be hidden or transparent
  void onTransparencyChanged();

  /// Closes faces panel restore all hidden faces by calling reset()
  void onClosed();

  /// Flushes redisplay event and perform update of object browser icons
  /// (objects might be hidden/shown)
  void flushRedisplay() const;

private:

  QCheckBox* myHiddenOrTransparent; ///< if checked - transparent, else hidden
  ModuleBase_ListView* myListView; ///< list control of processed faces
  XGUI_Workshop* myWorkshop; ///< workshop

  bool myIsActive; ///< current state about the panel is active
  int myLastItemIndex; ///< last index to be used in the map of items for the next added item
  std::vector<int> myUndoList; ///< indexes of items for undo

  QMap<int, ModuleBase_ViewerPrsPtr> myItems; ///< selected face items
  std::set<ObjectPtr > myItemObjects; ///< cached objects of myItems
  std::set<ObjectPtr > myHiddenObjects; ///< hidden objects
  std::set<ObjectPtr > myHiddenGroups; ///< hidden objects
};

#endif
