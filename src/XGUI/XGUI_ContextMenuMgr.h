
#ifndef XGUI_ContextMenuMgr_H
#define XGUI_ContextMenuMgr_H

#include "XGUI.h"

#include <QObject>
#include <QMap>

class XGUI_Workshop;
class QAction;
class QContextMenuEvent;
class QMenu;

class XGUI_EXPORT XGUI_ContextMenuMgr: public QObject
{
Q_OBJECT
public:
  XGUI_ContextMenuMgr(XGUI_Workshop* theParent);
  virtual ~XGUI_ContextMenuMgr();

  void createActions();

  void addAction(const QString& theId, QAction* theAction);

  QAction* action(const QString& theId) const;

  QStringList actionIds() const;

  void updateCommandsStatus();

  void connectObjectBrowser() const;

  void connectViewer() const;

signals:
  void actionTriggered(const QString& theId, bool isChecked);

private slots:
  void onAction(bool isChecked);

  void onContextMenuRequest(QContextMenuEvent* theEvent);

private:
  QMenu* objectBrowserMenu() const;
  QMenu* viewerMenu() const;

  QMap<QString, QAction*> myActions;

  XGUI_Workshop* myWorkshop;
};

#endif