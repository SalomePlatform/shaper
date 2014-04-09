#ifndef XGUI_ViewWindow_H
#define XGUI_ViewWindow_H

#include "XGUI_Constants.h"
#include "XGUI_ViewBackground.h"

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
class QMdiSubWindow;

/*!
  \class XGUI_ViewWindow
  \ingroup GUI
  \brief Implements a one view window of 3d viewer object.
  It contains a view port object (drawing area) and toolbars for view camera and window management.
  Also it managements events in view port
*/
class XGUI_ViewWindow: public QFrame
{
Q_OBJECT
public:
  //! Types of viewer operations
  enum OperationType
  {
    NOTHING,
    PANVIEW,
    ZOOMVIEW,
    ROTATE,
    PANGLOBAL,
    WINDOWFIT,
    FITALLVIEW,
    RESETVIEW,
    FRONTVIEW,
    BACKVIEW,
    TOPVIEW,
    BOTTOMVIEW,
    LEFTVIEW,
    RIGHTVIEW,
    CLOCKWISEVIEW,
    ANTICLOCKWISEVIEW
  };

  XGUI_ViewWindow(XGUI_Viewer* theViewer, V3d_TypeOfView theType);

  virtual ~XGUI_ViewWindow();

  //! Returns view port object
  XGUI_ViewPort* viewPort() const
  {
    return myViewPort;
  }

  //! Retrurns current interaction style
  XGUI::InteractionStyle interactionStyle() const
  {
    return myInteractionStyle;
  }

  //! Disable or enable given operation type
  void setTransformEnabled(const OperationType, const bool);

  //! Returns true if the given operation type is enabled
  bool transformEnabled(const OperationType) const;

  //! Returns View background object
  XGUI_ViewBackground background() const;

  //! Sets View background object
  void setBackground(const XGUI_ViewBackground& theBackground);

  //! Returns true if the current view window can be closed
  bool closable() const { return myClosable; }

  //! Sets the current view window closable or not
  void setClosable( const bool isClosable ) { myClosable = isClosable; }

signals:
  //! Emited whien view transformation operation is started
  void vpTransformationStarted(XGUI_ViewWindow::OperationType type);

  //! Emited whien view transformation operation is finished
  void vpTransformationFinished(XGUI_ViewWindow::OperationType type);

  //void Show(QShowEvent *);
  //void Hide(QHideEvent *);
  //void maximized(XGUI_ViewWindow*, bool);
  //void returnedTo3d();

  //! Emited before the window closing
  void tryClosing(XGUI_ViewWindow*);

  //! Emited when window is closing
  void closed( QMdiSubWindow* );

  //! Emited on mouse press in view port
  void mousePressed(XGUI_ViewWindow*, QMouseEvent*);

  //! Emited on mouse release in view port
  void mouseReleased(XGUI_ViewWindow*, QMouseEvent*);

  //! Emited on mouse double click in view port
  void mouseDoubleClicked(XGUI_ViewWindow*, QMouseEvent*);

  //! Emited on mouse moving in view port
  void mouseMoving(XGUI_ViewWindow*, QMouseEvent*);
 
  //! Emited on key press in view port
  void keyPressed(XGUI_ViewWindow*, QKeyEvent*);
 
  //! Emited on key release in view port
  void keyReleased(XGUI_ViewWindow*, QKeyEvent*);
 
  //! Emited on context menu request in view port
  void contextMenuRequested(QContextMenuEvent *e);

  //void viewModified(XGUI_ViewWindow*);
  void viewCloned( QMdiSubWindow* theView );

public slots:
  //! Start zooming operation
  void activateZoom();

  //! Start rotation operation
  void activateRotation();

  //! Start panning operation
  void activatePanning();

  //! Start window fit operation
  void activateWindowFit();

  //! Start global panning operation
  void activateGlobalPanning();

  //! Clone the view window preserving a view point of the current view
  void cloneView();

  //! Dump the view window into external file (*.bmp *.png *.jpg *.jpeg *.eps *.ps)
  void dumpView();

  //! Fit all command
  void fitAll();

  //! Set front view
  void frontView();

  //! Set back view
  void backView();

  //! Set top view
  void topView();

  //! Set bottom view
  void bottomView();

  //! Set left view
  void leftView();

  //! Set right view
  void rightView();

  //! Reset point of view
  void reset();


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

  void updateToolBar();
//  void repaintToolBar();

private:
  enum WindowState
  {
    MinimizedState, MaximizedState, NormalState
  };

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
  bool setTransformRequested(OperationType);

  // Transformation is selected and already started
  bool transformInProcess() const
  {
    return myEventStarted;
  }
  void setTransformInProcess(bool bOn)
  {
    myEventStarted = bOn;
  }

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
  OperationType myOperation;
  XGUI::Mode2dType my2dMode;

  int myCurSketch;
  bool myDrawRect;          // set when a rect is used for selection or magnify 
  bool myEnableDrawMode;
  bool myRotationPointSelection;
  bool myCursorIsHand;
  bool myIsKeyFree;
  bool myEventStarted;       // set when transformation is in process 
  bool myClosable;

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
/*!
  \class ViewerToolbar
  \ingroup GUI
  \brief Provides a toolbar widget with treansparent background over OCCT View window
*/
class ViewerToolbar: public QToolBar
{
Q_OBJECT
public:
  ViewerToolbar(QWidget* theParent, XGUI_ViewPort* thePort)
      : QToolBar(theParent), myVPort(thePort)
  {
    setBackgroundRole(QPalette::NoRole);
    setAttribute(Qt::WA_NoSystemBackground);
    setAttribute(Qt::WA_PaintOnScreen);
  }

  void repaintBackground();

protected:
  virtual void paintEvent(QPaintEvent* theEvent);

private:
  XGUI_ViewPort* myVPort;
};

//******************************************************
/*!
  \class ViewerToolbar
  \ingroup GUI
  \brief Provides a Label widget with treansparent background over OCCT View window
*/
class ViewerLabel: public QLabel
{
Q_OBJECT
public:
  ViewerLabel(QWidget* theParent, XGUI_ViewPort* thePort)
      : QLabel(theParent), myVPort(thePort)
  {
    setAttribute(Qt::WA_NoSystemBackground);
  }

  void repaintBackground();

protected:
  virtual void paintEvent(QPaintEvent* theEvent);

private:
  XGUI_ViewPort* myVPort;
};

#endif
