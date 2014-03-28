#ifndef XGUI_ViewWindow_H
#define XGUI_ViewWindow_H

#include "XGUI_Constants.h"

#include <QFrame>
#include <QIcon>
#include <QToolBar>
#include <QLabel>
#include <QMap>

#include <V3d_View.hxx>
#include <V3d_Viewer.hxx>

class XGUI_ViewPort;
class XGUI_Viewer;
class ViewerToolbar;
class ViewerLabel;
class XGUI_RectRubberBand;

class XGUI_ViewWindow : public QFrame
{
    Q_OBJECT
public:
    enum OperationType{ NOTHING, PANVIEW, ZOOMVIEW, ROTATE, 
                        PANGLOBAL, WINDOWFIT, FITALLVIEW, RESETVIEW,
                        FRONTVIEW, BACKVIEW, TOPVIEW, BOTTOMVIEW, LEFTVIEW, RIGHTVIEW,
	        	        CLOCKWISEVIEW, ANTICLOCKWISEVIEW };

    XGUI_ViewWindow(XGUI_Viewer* theViewer, 
                    V3d_TypeOfView theType);

    virtual ~XGUI_ViewWindow();

    XGUI_ViewPort* viewPort() const { return myViewPort; }


    XGUI::InteractionStyle interactionStyle() const { return myInteractionStyle; } 

    void setTransformEnabled( const OperationType, const bool );
    bool transformEnabled( const OperationType ) const;

signals:
    void vpTransformationStarted(XGUI_ViewWindow::OperationType type);
    void vpTransformationFinished(XGUI_ViewWindow::OperationType type);
    //void viewCloned( XGUI_ViewWindow* );

    void Show( QShowEvent * );
    void Hide( QHideEvent * );
    void maximized( XGUI_ViewWindow*, bool );
    void returnedTo3d( );


    void tryClosing( XGUI_ViewWindow* );
    void closing( XGUI_ViewWindow* );
    void mousePressed( XGUI_ViewWindow*, QMouseEvent* );
    void mouseReleased( XGUI_ViewWindow*, QMouseEvent* );
    void mouseDoubleClicked( XGUI_ViewWindow*, QMouseEvent* );
    void mouseMoving( XGUI_ViewWindow*, QMouseEvent* );
    void wheeling( XGUI_ViewWindow*, QWheelEvent* );
    void keyPressed( XGUI_ViewWindow*, QKeyEvent* );
    void keyReleased( XGUI_ViewWindow*, QKeyEvent* );
    void contextMenuRequested( QContextMenuEvent *e );
    void viewModified( XGUI_ViewWindow* );

public slots:
    void activateZoom();
    void activateRotation();
    void activatePanning();

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

    bool processWindowControls(QObject *theObj, QEvent *theEvent);
    bool processViewPort(QEvent *theEvent);

    void vpMousePressEvent(QMouseEvent* theEvent);
    void vpMouseReleaseEvent(QMouseEvent* theEvent);
    void vpMouseMoveEvent(QMouseEvent* theEvent);

    OperationType getButtonState(QMouseEvent* theEvent, XGUI::InteractionStyle theInteractionStyle);
  
    void resetState();
    void drawRect();
    void endDrawRect();

    bool transformRequested() const;
    bool setTransformRequested ( OperationType );

    // Transformation is selected and already started
    bool transformInProcess() const { return myEventStarted; }
    void setTransformInProcess( bool bOn ) { myEventStarted = bOn; }

private:
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

    int myStartX;
    int myStartY;
    int myCurrX;
    int myCurrY;

    XGUI::InteractionStyle myInteractionStyle;
    OperationType          myOperation;
    XGUI::Mode2dType       my2dMode;

    int myCurSketch;
    bool myDrawRect;           // set when a rect is used for selection or magnify 
    bool myEnableDrawMode;
    bool myRotationPointSelection;
    bool myCursorIsHand;                 
    bool myIsKeyFree;
    bool myEventStarted;       // set when transformation is in process 

    QCursor myCursor;
  
    XGUI::RotationPointType myCurrPointType;
    XGUI::RotationPointType myPrevPointType;

    gp_Pnt mySelectedPoint;

    XGUI_RectRubberBand* myRectBand; //!< selection rectangle rubber band

    typedef QMap<OperationType, bool> MapOfTransformStatus;
    MapOfTransformStatus myStatus;

    double myCurScale;
};


//******************************************************
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

//******************************************************
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