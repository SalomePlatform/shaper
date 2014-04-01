#ifndef XGUI_MainMenu_H
#define XGUI_MainMenu_H

#include <QObject>
#include <QList>

class XGUI_Command;
class XGUI_MainWindow;
class XGUI_Workbench;
class XGUI_MenuGroupPanel;

class QTabWidget;
class QLabel;
class QAction;
class QDockWidget;

class XGUI_MainMenu: public QObject
{
Q_OBJECT
public:
  XGUI_MainMenu(XGUI_MainWindow *parent);
  virtual ~XGUI_MainMenu();

  XGUI_Workbench* addWorkbench(const QString& theId, const QString& theText = "");
  XGUI_Workbench* findWorkbench(const QString& theId);

private:
  XGUI_MainWindow* myDesktop;
  QList<QDockWidget*> myMenuTabs;
};

#endif
