#ifndef XGUI_ViewWindow_H
#define XGUI_ViewWindow_H

#include <QFrame>
#include <QIcon>
#include <QToolBar>
#include <QLabel>

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

class XGUI_ViewPort;
class XGUI_Viewer;
class ViewerToolbar;
class ViewerLabel;

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
    ViewerLabel* myGripWgt;
    XGUI_ViewPort* myViewPort;
    ViewerToolbar* myViewBar;
    ViewerToolbar* myWindowBar;
    QAction* myMinimizeBtn;
    QAction* myMaximizeBtn;

    QIcon MinimizeIco;
    QIcon MaximizeIco;
    QIcon CloseIco;
    QIcon RestoreIco;
    
    bool myMoving;
    QPoint myMousePnt;

    WindowState myLastState;

    //QGraphicsScene* myScene;
};

class ViewerToolbar : public QToolBar
{
    Q_OBJECT
public:
    ViewerToolbar(QWidget* theParent, XGUI_ViewPort* thePort) :
    QToolBar(theParent), myVPort(thePort) {}

protected:
    virtual void paintEvent( QPaintEvent* theEvent);

private:
    XGUI_ViewPort* myVPort;
};

class ViewerLabel : public QLabel
{
    Q_OBJECT
public:
    ViewerLabel(QWidget* theParent, XGUI_ViewPort* thePort) :
    QLabel(theParent), myVPort(thePort) {}

protected:
    virtual void paintEvent( QPaintEvent* theEvent);

private:
    XGUI_ViewPort* myVPort;
};

#endif