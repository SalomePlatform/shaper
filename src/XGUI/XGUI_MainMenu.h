#ifndef XGUI_MainMenu_H
#define XGUI_MainMenu_H

#include "XGUI_Interfaces.h"

#include <QObject>
#include <QList>

class QTabWidget;
class QLabel;
class QAction;
class XGUI_Command;
class XGUI_MainWindow;
class QDockWidget;


class XGUI_MainMenu : public QObject
{
    Q_OBJECT
public:
    XGUI_MainMenu(XGUI_MainWindow *parent);
    virtual ~XGUI_MainMenu();

    IWorkbench* addWorkbench(const QString& theTitle);
    IWorkbench* findWorkbench(const QString& theObjName);

    IMenuGroup* addGroup(int thePageId);

private:
    XGUI_MainWindow* myDesktop;
    QList<QDockWidget*> myMenuTabs;
};

#endif
