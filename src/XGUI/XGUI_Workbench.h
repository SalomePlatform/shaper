
#ifndef XGUI_Workbench_H
#define XGUI_Workbench_H

#include <QWidget>
#include <QScrollArea>
#include <QList>

class QHBoxLayout;
class XGUI_MenuGroupPanel;
class QPushButton;
class CommandsArea;
class XGUI_Command;


class XGUI_Workbench : public QWidget
{
    Q_OBJECT
public:
    XGUI_Workbench(QWidget* theParent);

    int addGroup();

    void addCommand(int theGroupId, XGUI_Command* theCommand);

private slots:
    void onLeftScroll();
    void onRightScroll();

protected:
    virtual void resizeEvent(QResizeEvent * theEvent);

private:
    void addSeparator();
    bool isExceedsLeft();
    bool isExceedsRight();


    QWidget* myChildWidget;
    QHBoxLayout* myLayout;
    QList<XGUI_MenuGroupPanel*> myGroups;

    CommandsArea* myCommandsArea;
    QPushButton* myRightButton;
    QPushButton* myLeftButton;
};

#endif;