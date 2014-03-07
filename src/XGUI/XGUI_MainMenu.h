#ifndef XGUI_MainMenu_H
#define XGUI_MainMenu_H

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

    int addWorkbench(QString theTitle);

    int addGroup(int thePageId);

    void addCommand(int thePageId, int theGroupId, XGUI_Command* theCommand);

private:
    XGUI_MainWindow* myDesktop;
    QList<QDockWidget*> myMenuTabs;
};

#endif
