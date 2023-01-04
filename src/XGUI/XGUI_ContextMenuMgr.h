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

#ifndef XGUI_ContextMenuMgr_H
#define XGUI_ContextMenuMgr_H

#include "XGUI.h"

#include <QObject>
#include <QMap>

class XGUI_Workshop;
class QAction;
class QContextMenuEvent;
class QMenu;
class QActionGroup;

/**
 * \ingroup GUI
 * A class wihich provides managent of context menu
 */
class XGUI_EXPORT XGUI_ContextMenuMgr : public QObject
{
Q_OBJECT
 public:
   /// Constructor
   /// \param theParent a parent object
  XGUI_ContextMenuMgr(XGUI_Workshop* theParent);
  virtual ~XGUI_ContextMenuMgr();

  /// Create all actions for context menus. It is called on creation of application
  void createActions();

  /// Returns action according to the given ID
  /// \param theId an id of an action
  QAction* action(const QString& theId) const;

  /// Returns action object by its Id (name)
  /// \param theName is an Id of the action
  QAction* actionByName(const QString& theName) const;

  /// Returns list of registered actions Ids
  QStringList actionIds() const;

  /// update state of internal commands
  void updateCommandsStatus();

  /// Connect to object browser from workshop. Has to called at creation of viewer.
  void connectObjectBrowser();

  /// Connect to viewer from workshop. Has to called at creation of viewer.
  void connectViewer();

  /// Add menu items for Object browser pop-up
  void addObjBrowserMenu(QMenu*) const;

  /// Add menu items for Viewer pop-up
  void addViewerMenu(QMenu*) const;

  /// Returns a list of object group names of the action
  /// \param theName a name of the action
  /// \return a list of group names
  QStringList actionObjectGroups(const QString& theName);

  /// Updates menu for object browser
  void updateObjectBrowserMenu();

  /// Updates menu for viewer
  void updateViewerMenu();

signals:
  /// Signal aabout triggered action
  /// \param theId an id of triggered action
  /// \param isChecked is checked flag
  void actionTriggered(const QString& theId, bool isChecked);

  /// A signal which is sent before context menu show
  void beforeContextMenu();

  /// A signal which is sent after context menu show
  void afterContextMenu();

 private slots:
   /// Process action event
   /// \param isChecked a checked action flag
  void onAction(bool isChecked);

  /// Process context menu event
  /// \param theEvent a context menu event
  void onContextMenuRequest(QContextMenuEvent* theEvent);

  void onRename();

  /// Slot called on Result selection menu press
  void onResultSelection(bool theChecked);

  /// Slot called on any shape selection menu press
  void onShapeSelection(bool theChecked);

 private:
  /** 
   * Add action
   * \param theId - string ID of the item
   * \param theAction - action to add
   */
  void addAction(const QString& theId, QAction* theAction);

  void addFeatures(QMenu* theMenu) const;

  /// Creates menu for object browser
  void buildObjBrowserMenu();

  /// Creates menu for viewer
  void buildViewerMenu();

  /// Map of created actions [id : Action]
  QMap<QString, QAction*> myActions;

  /// Reference to workshop
  XGUI_Workshop* myWorkshop;

  typedef QList<QAction*> QActionsList;
  QMap<std::string, QActionsList> myObjBrowserMenus;
  QMap<std::string, QActionsList> myViewerMenu;

  //QActionGroup* mySelectActions;

  QAction* mySeparator1;
  QAction* mySeparator2;
  QAction* mySeparator3;
  QAction* mySeparator4;
};

#endif
