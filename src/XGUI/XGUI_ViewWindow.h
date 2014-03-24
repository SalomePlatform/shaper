#ifndef XGUI_ViewWindow_H
#define XGUI_ViewWindow_H

#include <QWidget>
#include <QIcon>

class QLabel;
class QToolBar;

class XGUI_ViewWindow : public QWidget
{
    Q_OBJECT
public:
    XGUI_ViewWindow();
    virtual ~XGUI_ViewWindow();

protected:
    virtual void resizeEvent(QResizeEvent* theEvent);

    virtual void changeEvent(QEvent* theEvent);

    virtual void enterEvent(QEvent* theEvent);
    virtual void leaveEvent(QEvent* theEvent);

    virtual bool eventFilter(QObject *theObj, QEvent *theEvent);

private slots:
    void onClose();
    void onMinimize();
    void onMaximize();

private:

    QLabel* myPicture;
    QLabel* myViewPort;
    QLabel* myGripWgt;
    QToolBar* myViewBar;
    QToolBar* myWindowBar;
    QAction* myMinimizeBtn;
    QAction* myMaximizeBtn;

    QPixmap ViewPortPxm;
    QIcon MinimizeIco;
    QIcon MaximizeIco;
    QIcon CloseIco;
    QIcon RestoreIco;
    
    bool myMoving;
    QPoint myMousePnt;
};

#endif