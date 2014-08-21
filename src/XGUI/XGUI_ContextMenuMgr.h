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
 * A claas wihich provides manement of context menu
 */
class XGUI_EXPORT XGUI_ContextMenuMgr : public QObject
{
Q_OBJECT
 public:
  XGUI_ContextMenuMgr(XGUI_Workshop* theParent);
  virtual ~XGUI_ContextMenuMgr();

  /// Create all actions for context menus. It is called on creation of application
  void createActions();

  /// Returns action according to the given ID
  QAction* action(const QString& theId) const;

  /// Returns list of registered actions Ids
  QStringList actionIds() const;

  /// update state of internal commands
  void updateCommandsStatus();

  /// Connect to object browser from workshop. Has to called at creation of viewer.
  void connectObjectBrowser() const;

  /// Connect to viewer from workshop. Has to called at creation of viewer.
  void connectViewer() const;

  /// Add menu atems for viewer into the given menu (used in SALOME mode)
  void addViewerItems(QMenu* theMenu) const;

signals:
  void actionTriggered(const QString& theId, bool isChecked);

 private slots:
  void onAction(bool isChecked);

  void onContextMenuRequest(QContextMenuEvent* theEvent);

 private:
  /** 
   * Add action
   * \param theId - string ID of the item
   * \param theAction - action to add
   */
  void addAction(const QString& theId, QAction* theAction);

  QMenu* objectBrowserMenu() const;
  QMenu* viewerMenu() const;

  QMap<QString, QAction*> myActions;

  XGUI_Workshop* myWorkshop;
};

#endif
