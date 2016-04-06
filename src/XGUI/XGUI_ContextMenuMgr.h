// Copyright (C) 2014-20xx CEA/DEN, EDF R&D -->

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
 * A claas wihich provides manement of context menu
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

 private:
  /** 
   * Add action
   * \param theId - string ID of the item
   * \param theAction - action to add
   */
  void addAction(const QString& theId, QAction* theAction);

  /// Updates menu for object browser
  void updateObjectBrowserMenu();

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

  QActionGroup* mySelectActions;

  QAction* mySeparator;
};

#endif
