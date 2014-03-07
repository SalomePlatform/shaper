
#ifndef XGUI_MenuGroupPanel_H
#define XGUI_MenuGroupPanel_H

#include <QWidget>
#include <QList>

class QGridLayout;
class XGUI_Command;

class XGUI_MenuGroupPanel : public QWidget
{
    Q_OBJECT
public:
    explicit XGUI_MenuGroupPanel(QWidget *parent = 0);

    void addCommand(XGUI_Command* theAction);

protected:
    virtual void resizeEvent(QResizeEvent *theEvent);

private:
    void addWidget(QWidget* theWgt);
    void placeWidget(QWidget* theWgt);

    QList<QWidget*> myWidgets;
    QGridLayout* myLayout;
    int myNewRow;
    int myNewCol;
    int myMaxRow;
};

#endif