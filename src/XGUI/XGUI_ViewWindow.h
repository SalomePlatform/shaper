#ifndef XGUI_ViewWindow_H
#define XGUI_ViewWindow_H

#include <QFrame>
#include <QIcon>

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

class QLabel;
class QToolBar;
class XGUI_ViewPort;
class XGUI_Viewer;

class XGUI_ViewWindow : public QFrame
{
    Q_OBJECT
public:
    XGUI_ViewWindow(XGUI_Viewer* theViewer, 
                    V3d_TypeOfView theType);

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
    enum WindowState { MinimizedState, MaximizedState, NormalState };


    XGUI_Viewer* myViewer;

    QLabel* myPicture;
    QLabel* myGripWgt;
    XGUI_ViewPort* myViewPort;
    QToolBar* myViewBar;
    QToolBar* myWindowBar;
    QAction* myMinimizeBtn;
    QAction* myMaximizeBtn;

    QIcon MinimizeIco;
    QIcon MaximizeIco;
    QIcon CloseIco;
    QIcon RestoreIco;
    
    bool myMoving;
    QPoint myMousePnt;

    WindowState myLastState;
};

#endif