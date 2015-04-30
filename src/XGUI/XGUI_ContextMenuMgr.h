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
  void connectObjectBrowser() const;

  /// Connect to viewer from workshop. Has to called at creation of viewer.
  void connectViewer() const;

  /// Add menu atems for viewer into the given menu (used in SALOME mode)
  /// \param theMenu a popup menu to be shown in the viewer
  void addViewerMenu(QMenu* theMenu) const;

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

 private:
  /** 
   * Add action
   * \param theId - string ID of the item
   * \param theAction - action to add
   */
  void addAction(const QString& theId, QAction* theAction);

  /// Creates menu for object browser
  QMenu* objectBrowserMenu() const;

  /// Creates menu for viewer
  QMenu* viewerMenu() const;

  /// Map of created actions [id : Action]
  QMap<QString, QAction*> myActions;

  /// Reference to workshop
  XGUI_Workshop* myWorkshop;
};

#endif
