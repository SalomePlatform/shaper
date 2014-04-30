#include "XGUI_ViewWindow.h"
#include "XGUI_ViewPort.h"
#include "XGUI_Viewer.h"
#include "XGUI_Tools.h"
#include "XGUI_RubberBand.h"

#include <QLayout>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QResizeEvent>
#include <QApplication>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPainter>
#include <QTimer>
#include <QFileDialog>
#include <QStyleOptionToolBar>

#include <TopoDS_Shape.hxx>
#include <BRep_Tool.hxx>
#include <TopoDS.hxx>
#include <Visual3d_View.hxx>

#define BORDER_SIZE 2

const char* imageZoomCursor[] = { "32 32 3 1", ". c None", "a c #000000", "# c #ffffff",
    "................................", "................................",
    ".#######........................", "..aaaaaaa.......................",
    "................................", ".............#####..............",
    "...........##.aaaa##............", "..........#.aa.....a#...........",
    ".........#.a.........#..........", ".........#a..........#a.........",
    "........#.a...........#.........", "........#a............#a........",
    "........#a............#a........", "........#a............#a........",
    "........#a............#a........", ".........#...........#.a........",
    ".........#a..........#a.........", ".........##.........#.a.........",
    "........#####.....##.a..........", ".......###aaa#####.aa...........",
    "......###aa...aaaaa.......#.....", ".....###aa................#a....",
    "....###aa.................#a....", "...###aa...............#######..",
    "....#aa.................aa#aaaa.", ".....a....................#a....",
    "..........................#a....", "...........................a....",
    "................................", "................................",
    "................................", "................................" };

const char* imageRotateCursor[] = { "32 32 3 1", ". c None", "a c #000000", "# c #ffffff",
    "................................", "................................",
    "................................", "................................",
    "........#.......................", ".......#.a......................",
    "......#######...................", ".......#aaaaa#####..............",
    "........#..##.a#aa##........##..", ".........a#.aa..#..a#.....##.aa.",
    ".........#.a.....#...#..##.aa...", ".........#a.......#..###.aa.....",
    "........#.a.......#a..#aa.......", "........#a.........#..#a........",
    "........#a.........#a.#a........", "........#a.........#a.#a........",
    "........#a.........#a.#a........", ".........#.........#a#.a........",
    "........##a........#a#a.........", "......##.a#.......#.#.a.........",
    "....##.aa..##.....##.a..........", "..##.aa.....a#####.aa...........",
    "...aa.........aaa#a.............", "................#.a.............",
    "...............#.a..............", "..............#.a...............",
    "...............a................", "................................",
    "................................", "................................",
    "................................", "................................" };

const char* imageCrossCursor[] = { "32 32 3 1", ". c None", "a c #000000", "# c #ffffff",
    "................................", "................................",
    "................................", "................................",
    "................................", "................................",
    "................................", "...............#................",
    "...............#a...............", "...............#a...............",
    "...............#a...............", "...............#a...............",
    "...............#a...............", "...............#a...............",
    "...............#a...............", ".......#################........",
    "........aaaaaaa#aaaaaaaaa.......", "...............#a...............",
    "...............#a...............", "...............#a...............",
    "...............#a...............", "...............#a...............",
    "...............#a...............", "...............#a...............",
    "................a...............", "................................",
    "................................", "................................",
    "................................", "................................",
    "................................", "................................" };


ViewerToolbar::ViewerToolbar(QWidget* theParent, XGUI_ViewPort* thePort)
  : QToolBar(theParent), myVPort(thePort), myResize(false)
{
  connect(myVPort, SIGNAL(resized()), this, SLOT(onViewPortResized()));
}

void ViewerToolbar::paintEvent(QPaintEvent* theEvent)
{
  //QToolBar::paintEvent(theEvent);
  // Paint background
  QPainter aPainter(this);
  QRect aRect = rect();
  QRect aVPRect = myVPort->rect();
  QPoint aGlobPnt = mapToGlobal(aRect.topLeft());
  QPoint aPnt = myVPort->mapFromGlobal(aGlobPnt);

  QRect aImgRect(QRect(aPnt.x(), aPnt.y() + aVPRect.height() - aRect.height(),
                       aRect.width(), aRect.height()));
  QImage aImg = myVPort->dumpView(aImgRect, myResize);
  if (!aImg.isNull())
    aPainter.drawImage(aRect, aImg);
  myResize = false;

  // Paint foreground
  QStyle *style = this->style();
  QStyleOptionToolBar aOpt;
  initStyleOption(&aOpt);

  aOpt.rect = style->subElementRect(QStyle::SE_ToolBarHandle, &aOpt, this);
  if (aOpt.rect.isValid())
    style->drawPrimitive(QStyle::PE_IndicatorToolBarHandle, &aOpt, &aPainter, this);
}

//**************************************************************************
ViewerLabel::ViewerLabel(QWidget* theParent, XGUI_ViewPort* thePort)
  : QLabel(theParent), myVPort(thePort), myResize(false)
{
  connect(myVPort, SIGNAL(resized()), this, SLOT(onViewPortResized()));
}

void ViewerLabel::paintEvent(QPaintEvent* theEvent)
{
  QRect aRect = rect();
  QRect aVPRect = myVPort->rect();
  QPoint aGlobPnt = mapToGlobal(aRect.topLeft());
  QPoint aPnt = myVPort->mapFromGlobal(aGlobPnt);

  QRect aImgRect(QRect(aPnt.x(), aPnt.y() + aVPRect.height() - aRect.height(), 
                 aRect.width(), aRect.height()));
  QImage aImg = myVPort->dumpView(aImgRect, myResize);
  if (!aImg.isNull())
    QPainter(this).drawImage(aRect, aImg);
  QLabel::paintEvent(theEvent);
}

//**************************************************************************
//**************************************************************************
//**************************************************************************
XGUI_ViewWindow::XGUI_ViewWindow(XGUI_Viewer* theViewer, V3d_TypeOfView theType)
    : QFrame(), 
    myViewer(theViewer), 
    myMoving(false), 
    MinimizeIco(":pictures/wnd_minimize.png"), 
    MaximizeIco(":pictures/wnd_maximize.png"), 
    CloseIco(":pictures/wnd_close.png"), 
    RestoreIco(":pictures/wnd_restore.png"), 
    myInteractionStyle(XGUI::STANDARD), 
    myRectBand(0), 
    myIsKeyFree(false), 
    my2dMode(XGUI::No2dMode), 
    myCurrPointType(XGUI::GRAVITY), 
    myPrevPointType(XGUI::GRAVITY), 
    myRotationPointSelection(false),
    myClosable(true),
    myStartX(0), myStartY(0), myCurrX(0), myCurrY(0), myCurScale(0.0), myCurSketch(0),
    myDrawRect(false), myEnableDrawMode(true), myCursorIsHand(false), myEventStarted(false),
    myIsActive(false),
    myLastState(WindowNormalState), myOperation(NOTHING)
{
  mySelectedPoint = gp_Pnt(0., 0., 0.);
  setFrameStyle(QFrame::Raised);
  setFrameShape(QFrame::Panel);
  setLineWidth(BORDER_SIZE);
  setMouseTracking(true);

  QVBoxLayout* aLay = new QVBoxLayout(this);
  aLay->setContentsMargins(BORDER_SIZE, BORDER_SIZE, BORDER_SIZE, BORDER_SIZE);
  myViewPort = new XGUI_ViewPort(this, myViewer->v3dViewer(), theType);
  myViewPort->installEventFilter(this);
  myViewPort->setCursor(Qt::ArrowCursor);
  aLay->addWidget(myViewPort);

  myPicture = new QLabel(this);
  myPicture->setFrameStyle(QFrame::Sunken);
  myPicture->setFrameShape(QFrame::Panel);
  myPicture->setMouseTracking(true);
  myPicture->installEventFilter(this);
  aLay->addWidget(myPicture);
  myPicture->hide();

  QVBoxLayout* aVPLay = new QVBoxLayout(myViewPort);
  aVPLay->setMargin(0);
  aVPLay->setSpacing(0);
  aVPLay->setContentsMargins(0,0,0,0);

  QHBoxLayout* aToolLay = new QHBoxLayout();
  aToolLay->setMargin(0);
  aToolLay->setSpacing(0);
  aToolLay->setContentsMargins(0,0,0,0);
  aVPLay->addLayout(aToolLay);
  aVPLay->addStretch(); 

  myGripWgt = new ViewerLabel(this, myViewPort);
  myGripWgt->setPixmap(QPixmap(":pictures/wnd_grip.png"));
  myGripWgt->setMouseTracking(true);
  myGripWgt->installEventFilter(this);
  myGripWgt->setCursor(Qt::OpenHandCursor);
  aToolLay->addWidget(myGripWgt);

    // Create Viewer management buttons
  myViewBar = new ViewerToolbar(this, myViewPort);
  myViewBar->setCursor(Qt::PointingHandCursor);
  aToolLay->addWidget(myViewBar); 
  aToolLay->addStretch();

  QAction* aBtn;

  // Dump view
  aBtn = new QAction(QIcon(":pictures/occ_view_camera_dump.png"), tr("Dump view"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(dumpView()));
  myViewBar->addAction(aBtn);
  // Fit all
  aBtn = new QAction(QIcon(":pictures/occ_view_fitall.png"), tr("Fit all"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(fitAll()));
  myViewBar->addAction(aBtn);
  // Fit area
  aBtn = new QAction(QIcon(":pictures/occ_view_fitarea.png"), tr("Fit area"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(activateWindowFit()));
  myViewBar->addAction(aBtn);
  // Zoom
  aBtn = new QAction(QIcon(":pictures/occ_view_zoom.png"), tr("Zoom"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(activateZoom()));
  myViewBar->addAction(aBtn);
  // Pan
  aBtn = new QAction(QIcon(":pictures/occ_view_pan.png"), tr("Panning"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(activatePanning()));
  myViewBar->addAction(aBtn);
  // Global Panning
  aBtn = new QAction(QIcon(":pictures/occ_view_glpan.png"), tr("Global panning"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(activateGlobalPanning()));
  myViewBar->addAction(aBtn);
  // Rotation
  aBtn = new QAction(QIcon(":pictures/occ_view_rotate.png"), tr("Rotate"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(activateRotation()));
  myViewBar->addAction(aBtn);
  // Reset
  aBtn = new QAction(QIcon(":pictures/occ_view_reset.png"), tr("Reset"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(reset()));
  myViewBar->addAction(aBtn);
  // Front view
  aBtn = new QAction(QIcon(":pictures/occ_view_front.png"), tr("Front"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(frontView()));
  myViewBar->addAction(aBtn);
  // Back view
  aBtn = new QAction(QIcon(":pictures/occ_view_back.png"), tr("Back"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(backView()));
  myViewBar->addAction(aBtn);
  // Top view
  aBtn = new QAction(QIcon(":pictures/occ_view_top.png"), tr("Top"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(topView()));
  myViewBar->addAction(aBtn);
  // Bottom view
  aBtn = new QAction(QIcon(":pictures/occ_view_bottom.png"), tr("Bottom"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(bottomView()));
  myViewBar->addAction(aBtn);
  // Left view
  aBtn = new QAction(QIcon(":pictures/occ_view_left.png"), tr("Left"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(leftView()));
  myViewBar->addAction(aBtn);
  // Right view
  aBtn = new QAction(QIcon(":pictures/occ_view_right.png"), tr("Right"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(rightView()));
  myViewBar->addAction(aBtn);
  // Clone view
  aBtn = new QAction(QIcon(":pictures/occ_view_clone.png"), tr("Clone"), myViewBar);
  connect(aBtn, SIGNAL(triggered()), SLOT(cloneView()));
  myViewBar->addAction(aBtn);

    // Create Window management buttons
  myWindowBar = new ViewerToolbar(this, myViewPort);
  myWindowBar->setCursor(Qt::PointingHandCursor);
  aToolLay->addWidget(myWindowBar);

  myMinimizeBtn = new QAction(myWindowBar);
  myMinimizeBtn->setIcon(MinimizeIco);
  myWindowBar->addAction(myMinimizeBtn);
  connect(myMinimizeBtn, SIGNAL(triggered()), SLOT(onMinimize()));

  myMaximizeBtn = new QAction(myWindowBar);
  myMaximizeBtn->setIcon(MaximizeIco);
  myWindowBar->addAction(myMaximizeBtn);
  connect(myMaximizeBtn, SIGNAL(triggered()), SLOT(onMaximize()));

  aBtn = new QAction(myWindowBar);
  aBtn->setIcon(CloseIco);
  myWindowBar->addAction(aBtn);
  connect(aBtn, SIGNAL(triggered()), SLOT(onClose()));

  //Support copy of background on updating of viewer
  connect(myViewPort, SIGNAL(vpTransformed()), this, SLOT(updateToolBar()));
  connect(myViewPort, SIGNAL(vpUpdated()), this, SLOT(updateToolBar()));
  connect(this, SIGNAL(vpTransformationFinished(XGUI_ViewWindow::OperationType)), 
          this, SLOT(updateToolBar()));

}

//****************************************************************
XGUI_ViewWindow::~XGUI_ViewWindow()
{
}


//****************************************************************
void XGUI_ViewWindow::showEvent(QShowEvent* theEvent)
{
  QFrame::showEvent(theEvent);
  myWindowBar->setFixedSize(myWindowBar->sizeHint());
}

//****************************************************************
void XGUI_ViewWindow::changeEvent(QEvent* theEvent)
{

  if (theEvent->type() == QEvent::WindowStateChange) {
    if (isMinimized()) {
      if (myPicture->isHidden()) {
        myViewBar->hide();
        myGripWgt->hide(); 
        myWindowBar->hide();
        myViewPort->hide();
        myPicture->show();
      }
    } else {
      if (myPicture->isVisible()) {
        myPicture->hide();
        myViewPort->show();
      }
      if (isMaximized()) {
        myMinimizeBtn->setIcon(MinimizeIco);
        myMaximizeBtn->setIcon(RestoreIco);
      }
      myViewBar->setVisible(myIsActive);
      myWindowBar->setVisible(myIsActive);
      myGripWgt->setVisible(myIsActive && (!isMaximized()));
    }
  } else
    QWidget::changeEvent(theEvent);
}



//****************************************************************
void XGUI_ViewWindow::windowActivated()
{
  if (!(isMinimized() || parentWidget()->isMinimized())) {
    myIsActive = true;
    if (isMaximized() || parentWidget()->isMaximized()) {
      myMaximizeBtn->setIcon(RestoreIco);
    } else {
      myMaximizeBtn->setIcon(MaximizeIco);
    }
    myViewBar->show();
    myWindowBar->show();
    myGripWgt->setVisible(!(isMaximized() || isMinimized() ||
        parentWidget()->isMaximized() || parentWidget()->isMinimized()));
  } else 
    myIsActive = false;
}

//****************************************************************
void XGUI_ViewWindow::windowDeactivated()
{
  myIsActive = false;
  if (!(isMinimized() || parentWidget()->isMinimized())) {
    if (isMaximized() || parentWidget()->isMaximized()) {
      myMaximizeBtn->setIcon(RestoreIco);
    } else {
      myMaximizeBtn->setIcon(MaximizeIco);
    }
    myViewBar->hide();
    myWindowBar->hide();
    myGripWgt->hide(); 
  }
}


//****************************************************************
void XGUI_ViewWindow::onClose()
{
  if (parentWidget()) {
    emit tryClosing(this);
    if (closable()) {
      emit closed(static_cast<QMdiSubWindow*>(parentWidget()));
      parentWidget()->close();
    }
  }
}

//****************************************************************
void XGUI_ViewWindow::onMinimize()
{
  QPixmap aPMap = QPixmap::fromImage(myViewPort->dumpView());
  int aW = width();
  int aH = height();
  double aR = aW / 100.;
  int aNewH = int(aH / aR);
  myPicture->setPixmap(aPMap.scaled(100,  aNewH));

  myLastState = (isMaximized() || parentWidget()->isMaximized()) ? MaximizedState : WindowNormalState;
  showMinimized();
  parentWidget()->showMinimized();
  parentWidget()->setGeometry(parentWidget()->x(), parentWidget()->y(), 100, aNewH);
  parentWidget()->lower();
  windowDeactivated();
  myViewer->onWindowMinimized((QMdiSubWindow*)parentWidget());
}

//****************************************************************
void XGUI_ViewWindow::onMaximize()
{
  if (isMaximized() || parentWidget()->isMaximized()) {
    myMaximizeBtn->setIcon(MaximizeIco);
    myGripWgt->show();
    showNormal();
    parentWidget()->showNormal();
  } else {
    myMaximizeBtn->setIcon(RestoreIco);
    myGripWgt->hide();
    showMaximized();
    parentWidget()->showMaximized();
  }
  parentWidget()->activateWindow();
  myMinimizeBtn->setIcon(MinimizeIco);
  
  //  In order to avoid frosen background in toolbars when it shown as a second view
  QTimer::singleShot(50, parentWidget(), SLOT(setFocus()));
}

//****************************************************************
bool XGUI_ViewWindow::processWindowControls(QObject *theObj, QEvent *theEvent)
{
  switch(theEvent->type()) {
  case QEvent::MouseButtonPress: {
    QMouseEvent* aEvent = static_cast<QMouseEvent*>(theEvent);
    if ((aEvent->button() == Qt::LeftButton) && (!myMoving)) {
      myMoving = true;
      myMousePnt = aEvent->globalPos();
      return true;
    }
  }
    break;
  case QEvent::MouseButtonRelease: {
    QMouseEvent* aEvent = static_cast<QMouseEvent*>(theEvent);
    if ((aEvent->button() == Qt::LeftButton) && myMoving) {
      myMoving = false;
      return true;
    }
  }
    break;
  case QEvent::MouseMove: {
    QMouseEvent* aEvent = static_cast<QMouseEvent*>(theEvent);
    if (myMoving) {
      QMdiSubWindow* aParent = static_cast<QMdiSubWindow*>(parentWidget());
      QMdiArea* aMDIArea = aParent->mdiArea();

      QPoint aPnt = aEvent->globalPos();
      QPoint aMDIPnt = aMDIArea->mapFromGlobal(aPnt);
      if (aMDIArea->rect().contains(aMDIPnt)) {
                    int aX = aParent->x() + (aPnt.x() - myMousePnt.x());
                    int aY = aParent->y() + (aPnt.y() - myMousePnt.y());
                    aParent->move(aX, aY);
        myMousePnt = aPnt;
      }
      return true;
    }
  }
    break;
  case QEvent::MouseButtonDblClick:
    if (theObj == myPicture) {
      myMoving = false;
      if (myLastState == MaximizedState) {
        showMaximized();
      } else {
        showNormal();
      }
      myViewer->onWindowActivated((QMdiSubWindow*)parentWidget());

      //  In order to avoid frosen background in toolbars when it shown as a second view
      QTimer::singleShot(20, parentWidget(), SLOT(setFocus()));

      return true;
    }
  }
  return false;
}

//****************************************************************
bool XGUI_ViewWindow::processViewPort(QEvent *theEvent)
{
  switch(theEvent->type()) {
  case QEvent::MouseButtonPress:
    vpMousePressEvent((QMouseEvent*) theEvent);
    return true;

  case QEvent::MouseButtonRelease:
    vpMouseReleaseEvent((QMouseEvent*) theEvent);
    return true;

  case QEvent::MouseMove:
    vpMouseMoveEvent((QMouseEvent*) theEvent);
    return true;

  case QEvent::MouseButtonDblClick:
    emit mouseDoubleClicked(this, (QMouseEvent*) theEvent);
    return true;
    case QEvent::Wheel:
        {
            QWheelEvent* aEvent = (QWheelEvent*) theEvent;
            myViewPort->startZoomAtPoint( aEvent->x(), aEvent->y() );
            double aDelta = (double)( aEvent->delta() ) / ( 15 * 8 );
            int x  = aEvent->x();
            int y  = aEvent->y();
            int x1 = (int)( aEvent->x() + width()*aDelta/100 );
            int y1 = (int)( aEvent->y() + height()*aDelta/100 );
            myViewPort->zoom( x, y, x1, y1 );
        }
        return true;
  }
  return false;
}

//****************************************************************
bool XGUI_ViewWindow::eventFilter(QObject *theObj, QEvent *theEvent)
{
  if ((theObj == myGripWgt) || (theObj == myPicture)) {
    if (processWindowControls(theObj, theEvent))
      return true;
  } else if (theObj == myViewPort) {
    if (processViewPort(theEvent)) {
      return true;
    }
    if (theEvent->type() == QEvent::KeyRelease) {
      emit keyReleased(this, (QKeyEvent*) theEvent);
      return true;
    }
  }
  return QFrame::eventFilter(theObj, theEvent);
}

//****************************************************************
XGUI_ViewWindow::OperationType XGUI_ViewWindow::getButtonState(
    QMouseEvent* theEvent, XGUI::InteractionStyle theInteractionStyle)
{
  OperationType aOp = NOTHING;
  XGUI::InteractionStyle aStyle = (XGUI::InteractionStyle) theInteractionStyle;
  if ((theEvent->modifiers() == XGUI_Viewer::myStateMap[aStyle][XGUI::ZOOM])
      && (theEvent->buttons() == XGUI_Viewer::myButtonMap[aStyle][XGUI::ZOOM]))
    aOp = ZOOMVIEW;
  else if ((theEvent->modifiers() == XGUI_Viewer::myStateMap[aStyle][XGUI::PAN])
      && (theEvent->buttons() == XGUI_Viewer::myButtonMap[aStyle][XGUI::PAN]))
    aOp = PANVIEW;
  else if ((theEvent->modifiers() == XGUI_Viewer::myStateMap[aStyle][XGUI::ROTATE])
      && (theEvent->buttons() == XGUI_Viewer::myButtonMap[aStyle][XGUI::ROTATE])
      && (my2dMode == XGUI::No2dMode))
    aOp = ROTATE;

  return aOp;
}

//****************************************************************
void XGUI_ViewWindow::vpMousePressEvent(QMouseEvent* theEvent)
{
  myStartX = theEvent->x();
  myStartY = theEvent->y();
  XGUI::InteractionStyle anInteractionStyle = interactionStyle();

  // in "key free" interaction style zoom operation is activated by two buttons (simultaneously pressed),
  // which are assigned for pan and rotate - these operations are activated immediately after pressing 
  // of the first button, so it is necessary to switch to zoom when the second button is pressed
  bool aSwitchToZoom = false;
  if ((anInteractionStyle == XGUI::KEY_FREE) && (myOperation == PANVIEW || myOperation == ROTATE)) {
    aSwitchToZoom = getButtonState(theEvent, anInteractionStyle) == ZOOMVIEW;
  }

  switch(myOperation) {
  case WINDOWFIT:
    if (theEvent->button() == Qt::LeftButton)
      emit vpTransformationStarted(WINDOWFIT);
    break;

  case PANGLOBAL:
    if (theEvent->button() == Qt::LeftButton)
      emit vpTransformationStarted(PANGLOBAL);
    break;

  case ZOOMVIEW:
    if (theEvent->button() == Qt::LeftButton) {
      myViewPort->startZoomAtPoint(myStartX, myStartY);
      emit vpTransformationStarted(ZOOMVIEW);
    }
    break;

  case PANVIEW:
    if (aSwitchToZoom) {
      myViewPort->startZoomAtPoint(myStartX, myStartY);
      activateZoom();
    } else if (theEvent->button() == Qt::LeftButton)
      emit vpTransformationStarted(PANVIEW);
    break;

  case ROTATE:
    if (aSwitchToZoom) {
      myViewPort->startZoomAtPoint(myStartX, myStartY);
      activateZoom();
    } else if (theEvent->button() == Qt::LeftButton) {
      myViewPort->startRotation(myStartX, myStartY, myCurrPointType, mySelectedPoint);
      emit vpTransformationStarted(ROTATE);
    }
    break;

  default:
    /*  Try to activate a transformation */
    OperationType aState;
    if (interactionStyle() == XGUI::STANDARD)
      aState = getButtonState(theEvent, anInteractionStyle);
    else {
      aState = XGUI_ViewWindow::NOTHING;
      myIsKeyFree = true;
    }
    switch(aState) {
    case ZOOMVIEW:
      myViewPort->startZoomAtPoint(myStartX, myStartY);
      activateZoom();
      break;
    case PANVIEW:
      activatePanning();
      break;
    case ROTATE:
      activateRotation();
      myViewPort->startRotation(myStartX, myStartY, myCurrPointType, mySelectedPoint);
      break;
    default:
      if (myRotationPointSelection) {
        if (theEvent->button() == Qt::LeftButton) {
          Handle(AIS_InteractiveContext) ic = myViewer->AISContext();
          ic->Select();
          for(ic->InitSelected(); ic->MoreSelected(); ic->NextSelected()) {
            TopoDS_Shape aShape = ic->SelectedShape();
            if (!aShape.IsNull() && aShape.ShapeType() == TopAbs_VERTEX) {
              gp_Pnt aPnt = BRep_Tool::Pnt(TopoDS::Vertex(ic->SelectedShape()));
              /*if ( mySetRotationPointDlg ) {
               myRotationPointSelection = false;
               mySetRotationPointDlg->setCoords(aPnt.X(), aPnt.Y(), aPnt.Z());
               }*/
            } else {
              myCurrPointType = myPrevPointType;
              break;
            }
          }
          if (ic->NbSelected() == 0)
            myCurrPointType = myPrevPointType;
          //if ( mySetRotationPointDlg ) mySetRotationPointDlg->toggleChange();
          ic->CloseAllContexts();
          myOperation = NOTHING;
          myViewPort->setCursor(myCursor);
          myCursorIsHand = false;
          myRotationPointSelection = false;
        }
      } else
        emit mousePressed(this, theEvent);
      break;
    }
    /* notify that we start a transformation */
    if (transformRequested())
      emit vpTransformationStarted(myOperation);
  }
  if (transformRequested())
    setTransformInProcess(true);

  /* we may need it for sketching... */
  /*    if ( l_mbPressEvent )
   delete l_mbPressEvent;
   l_mbPressEvent = new QMouseEvent( *theEvent );*/
}

//****************************************************************
void XGUI_ViewWindow::vpMouseReleaseEvent(QMouseEvent* theEvent)
{
  switch(myOperation) {
  case NOTHING: {
    int prevState = myCurSketch;
    /*            if(theEvent->button() == Qt::RightButton) {
     QList<OCCViewer_ViewSketcher*>::Iterator it;
     for ( it = mySketchers.begin(); it != mySketchers.end() && myCurSketch != -1; ++it ) {
     OCCViewer_ViewSketcher* sk = (*it);
     if( ( sk->sketchButton() & theEvent->button() ) && sk->sketchButton() == myCurSketch )
     myCurSketch = -1;
     }
     }
     */
    emit mouseReleased(this, theEvent);
    if (theEvent->button() == Qt::RightButton && prevState == -1) {
      QContextMenuEvent aEvent(QContextMenuEvent::Mouse, theEvent->pos(), theEvent->globalPos());
      emit contextMenuRequested(&aEvent);
    }
  }
    break;
  case ROTATE:
    myViewPort->endRotation();
    resetState();
    break;

  case PANVIEW:
  case ZOOMVIEW:
    resetState();
    break;

  case PANGLOBAL:
    if (theEvent->button() == Qt::LeftButton) {
      myViewPort->setCenter(theEvent->x(), theEvent->y());
      myViewPort->getView()->SetScale(myCurScale);
      resetState();
    }
    break;

  case WINDOWFIT:
    if (theEvent->button() == Qt::LeftButton) {
      myCurrX = theEvent->x();
      myCurrY = theEvent->y();
      drawRect();
      QRect rect = makeRect(myStartX, myStartY, myCurrX, myCurrY);
      if (!rect.isEmpty())
        myViewPort->fitRect(rect);
      endDrawRect();
      resetState();
    }
    break;
  }

  // NOTE: viewer 3D detects a rectangle of selection using this event
  // so we must emit it BEFORE resetting the selection rectangle
  if (theEvent->button() == Qt::LeftButton && myDrawRect) {
    drawRect();
    endDrawRect();
    resetState();
    myViewPort->update();
  }
  /*    if ( l_mbPressEvent ) {
   delete l_mbPressEvent;
   l_mbPressEvent = 0;
   }*/
}

//****************************************************************
void XGUI_ViewWindow::vpMouseMoveEvent(QMouseEvent* theEvent)
{
  if (myIsKeyFree && interactionStyle() == XGUI::KEY_FREE) {
    myIsKeyFree = false;
    switch(getButtonState(theEvent, interactionStyle())) {
    case ZOOMVIEW:
      myViewPort->startZoomAtPoint(myStartX, myStartY);
      activateZoom();
      break;
    case PANVIEW:
      activatePanning();
      break;
    case ROTATE:
      activateRotation();
      myViewPort->startRotation(myStartX, myStartY, myCurrPointType, mySelectedPoint);
      break;
    default:
      break;
    }
  }

  myCurrX = theEvent->x();
  myCurrY = theEvent->y();
  switch(myOperation) {
  case ROTATE:
    myViewPort->rotate(myCurrX, myCurrY, myCurrPointType, mySelectedPoint);
    break;

  case ZOOMVIEW:
    myViewPort->zoom(myStartX, myStartY, myCurrX, myCurrY);
    myStartX = myCurrX;
    myStartY = myCurrY;
    break;

  case PANVIEW:
    myViewPort->pan(myCurrX - myStartX, myStartY - myCurrY);
    myStartX = myCurrX;
    myStartY = myCurrY;
    break;

  case PANGLOBAL:
    break;

  default:
    if (myRotationPointSelection /*|| isSketcherStyle()*/) {
      emit mouseMoving(this, theEvent);
    } else {
      int aState = theEvent->modifiers();
      int aButton = theEvent->buttons();
      int anInteractionStyle = interactionStyle();
      if (((anInteractionStyle == XGUI::STANDARD) && (aButton == Qt::LeftButton)
          && (aState == Qt::NoModifier || Qt::ShiftModifier))
          || ((anInteractionStyle == XGUI::KEY_FREE) && (aButton == Qt::LeftButton)
              && (aState == Qt::ControlModifier
                  || aState == (Qt::ControlModifier | Qt::ShiftModifier)))) {
        myDrawRect = myEnableDrawMode;
        if (myDrawRect) {
          drawRect();
          if (!myCursorIsHand) {   // we are going to sketch a rectangle
            QCursor handCursor(Qt::PointingHandCursor);
            myCursorIsHand = true;
            myCursor = cursor();
            myViewPort->setCursor(handCursor);
          }
        }
        emit mouseMoving(this, theEvent);
      } /* else if ( ( (anInteractionStyle == XGUI::STANDARD) &&
       (aButton == Qt::RightButton) && 
       ( aState == Qt::NoModifier || Qt::ShiftModifier ) ) ||
       ( (anInteractionStyle == XGUI::KEY_FREE) &&
       (aButton == Qt::RightButton) && 
       ( aState == Qt::ControlModifier || aState == ( Qt::ControlModifier|Qt::ShiftModifier ) ) ) ) {
       OCCViewer_ViewSketcher* sketcher = 0;
       QList<OCCViewer_ViewSketcher*>::Iterator it;
       for ( it = mySketchers.begin(); it != mySketchers.end() && !sketcher; ++it ) {
       OCCViewer_ViewSketcher* sk = (*it);
       if( sk->isDefault() && sk->sketchButton() == aButton )
       sketcher = sk;
       }
       if ( sketcher && myCurSketch == -1 ) {
       activateSketching( sketcher->type() );
       if ( mypSketcher ) {
       myCurSketch = mypSketcher->sketchButton();

       if ( l_mbPressEvent )  {
       QApplication::sendEvent( getViewPort(), l_mbPressEvent );
       delete l_mbPressEvent;
       l_mbPressEvent = 0;
       }
       QApplication::sendEvent( getViewPort(), theEvent );
       }
       }
       } */else
        emit mouseMoving(this, theEvent);
    }
  }
}

/*!
 \brief Draw rubber band rectangle.
 */
void XGUI_ViewWindow::drawRect()
{
  if (!myRectBand) {
    myRectBand = new XGUI_RectRubberBand(myViewPort);
  }

  myRectBand->setUpdatesEnabled(false);
  QRect aRect = makeRect(myStartX, myStartY, myCurrX, myCurrY);
  myRectBand->initGeometry(aRect);

  if (!myRectBand->isVisible())
    myRectBand->show();

  myRectBand->setUpdatesEnabled(true);
}

/*!
 \brief Clear rubber band rectangle on the end on the dragging operation.
 */
void XGUI_ViewWindow::endDrawRect()
{
  if (myRectBand) {
    myRectBand->clearGeometry();
    myRectBand->hide();
  }
}

void XGUI_ViewWindow::activateZoom()
{
  if (!transformRequested() && !myCursorIsHand)
    myCursor = cursor(); /* save old cursor */

  if (myOperation != ZOOMVIEW) {
    QPixmap zoomPixmap(imageZoomCursor);
    QCursor zoomCursor(zoomPixmap);
    if (setTransformRequested(ZOOMVIEW))
      myViewPort->setCursor(zoomCursor);
  }
}

bool XGUI_ViewWindow::transformRequested() const
{
  return (myOperation != NOTHING);
}

/*!
 \brief Start delayed viewer operation.
 */
bool XGUI_ViewWindow::setTransformRequested(OperationType op)
{
  bool ok = transformEnabled(op);
  myOperation = ok ? op : NOTHING;
  myViewPort->setMouseTracking(myOperation == NOTHING);
  return ok;
}

/*!
 Set enabled state of transformation (rotate, zoom, etc)
 */
void XGUI_ViewWindow::setTransformEnabled(const OperationType id, const bool on)
{
  if (id != NOTHING)
    myStatus.insert(id, on);
}

/*!
 \return enabled state of transformation (rotate, zoom, etc)
 */
bool XGUI_ViewWindow::transformEnabled(const OperationType id) const
{
  return myStatus.contains(id) ? myStatus[id] : true;
}

/*!
 \brief Start panning operation.

 Sets the corresponding cursor for the widget.
 */
void XGUI_ViewWindow::activatePanning()
{
  if (!transformRequested() && !myCursorIsHand)
    myCursor = cursor();                // save old cursor

  if (myOperation != PANVIEW) {
    QCursor panCursor(Qt::SizeAllCursor);
    if (setTransformRequested(PANVIEW))
      myViewPort->setCursor(panCursor);
  }
}

/*!
  \brief Start global panning operation

  Sets the corresponding cursor for the widget.
*/
void XGUI_ViewWindow::activateGlobalPanning()
{
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) {
    QPixmap globalPanPixmap (imageCrossCursor);
    QCursor glPanCursor (globalPanPixmap);
    myCurScale = aView3d->Scale();
    aView3d->FitAll(0.01, false);
    myCursor = cursor();                // save old cursor
    myViewPort->fitAll(); // fits view before selecting a new scene center
    if( setTransformRequested( PANGLOBAL ) )
      myViewPort->setCursor( glPanCursor );
  }
}

/*!
 \brief Start rotation operation

 Sets the corresponding cursor for the widget.
 */
void XGUI_ViewWindow::activateRotation()
{
  if (!transformRequested() && !myCursorIsHand)
    myCursor = cursor();                // save old cursor

  if (myOperation != ROTATE) {
    QPixmap rotatePixmap(imageRotateCursor);
    QCursor rotCursor(rotatePixmap);
    if (setTransformRequested(ROTATE))
      myViewPort->setCursor(rotCursor);
  }
}

/*!
 \brief Reset the viewport to its initial state
 ( no transformations in process etc. )
 */
void XGUI_ViewWindow::resetState()
{
  myDrawRect = false;

  if (myRotationPointSelection) {
    QCursor handCursor(Qt::PointingHandCursor);
    myViewPort->setCursor(handCursor);
  } else {
    if (transformRequested() || myCursorIsHand)
      myViewPort->setCursor(myCursor);
    myCursorIsHand = false;
  }

  if (transformRequested())
    emit vpTransformationFinished(myOperation);

  setTransformInProcess(false);
  setTransformRequested(NOTHING);
}

XGUI_ViewBackground XGUI_ViewWindow::background() const
{
  return myViewPort ? myViewPort->background() : XGUI_ViewBackground();
}

void XGUI_ViewWindow::setBackground(const XGUI_ViewBackground& theBackground)
{
  if (myViewPort) 
  	myViewPort->setBackground( theBackground );
}

/*!
   \brief Create one more window with same content.
*/
void XGUI_ViewWindow::cloneView()
{
  QMdiSubWindow* vw = myViewer->createView();
  XGUI_ViewWindow* aNewWnd = static_cast<XGUI_ViewWindow*>(vw->widget());
  aNewWnd->viewPort()->syncronizeWith(myViewPort);

  emit viewCloned( vw );

  //  In order to avoid frosen background in toolbars when it shown as a second view
  QTimer::singleShot(20, vw, SLOT(setFocus()));
}

void XGUI_ViewWindow::dumpView()
{
  QString aFilter(tr("Images Files (*.bmp *.png *.jpg *.jpeg *.eps *.ps)"));
  QString aSelectedFilter;
  QString aFileName = QFileDialog::getSaveFileName(this, "Save picture", QString(), aFilter, &aSelectedFilter);
  if (!aFileName.isNull()) {
    QApplication::setOverrideCursor( Qt::WaitCursor );
    QImage aPicture = myViewPort->dumpView();

    QString aFmt = extension(aFileName).toUpper();
    if( aFmt.isEmpty() )
      aFmt = QString( "BMP" ); // default format
    else if( aFmt == "JPG" )
      aFmt = "JPEG";

    Handle(Visual3d_View) a3dView = myViewPort->getView()->View();
    if (aFmt == "PS")
      a3dView->Export(strdup(qPrintable(aFileName)), Graphic3d_EF_PostScript);
    else if (aFmt == "EPS")
      a3dView->Export(strdup(qPrintable(aFileName)), Graphic3d_EF_EnhPostScript);
    else
      aPicture.save( aFileName, aFmt.toLatin1() );
    QApplication::restoreOverrideCursor();
  }
}

void XGUI_ViewWindow::fitAll()
{
  emit vpTransformationStarted( FITALLVIEW );
  myViewPort->fitAll();
  emit vpTransformationFinished( FITALLVIEW );
}

/*!
  \brief Starts fit operation.

  Sets the corresponding cursor for the widget.
*/
void XGUI_ViewWindow::activateWindowFit()
{
  if ( !transformRequested() && !myCursorIsHand )
    myCursor = cursor();                /* save old cursor */

  if ( myOperation != WINDOWFIT ) {
    QCursor handCursor (Qt::PointingHandCursor);
    if( setTransformRequested ( WINDOWFIT ) ) {
      myViewPort->setCursor ( handCursor );
      myCursorIsHand = true;
    }
  }
}


/*!
  \brief Perform "front view" transformation.
*/
void XGUI_ViewWindow::frontView()
{
  emit vpTransformationStarted ( FRONTVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) 
    aView3d->SetProj (V3d_Xpos);
  myViewPort->fitAll();
  emit vpTransformationFinished ( FRONTVIEW );
}

/*!
  \brief Perform "back view" transformation.
*/
void XGUI_ViewWindow::backView()
{
  emit vpTransformationStarted ( BACKVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) 
    aView3d->SetProj (V3d_Xneg);
  myViewPort->fitAll();
  emit vpTransformationFinished ( BACKVIEW );
}

/*!
  \brief Perform "top view" transformation.
*/
void XGUI_ViewWindow::topView()
{
  emit vpTransformationStarted ( TOPVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) 
    aView3d->SetProj (V3d_Zpos);
  myViewPort->fitAll();
  emit vpTransformationFinished ( TOPVIEW );
}

/*!
  \brief Perform "bottom view" transformation.
*/
void XGUI_ViewWindow::bottomView()
{
  emit vpTransformationStarted ( BOTTOMVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) 
    aView3d->SetProj (V3d_Zneg);
  myViewPort->fitAll();
  emit vpTransformationFinished ( BOTTOMVIEW );
}

/*!
  \brief Perform "left view" transformation.
*/
void XGUI_ViewWindow::leftView()
{
  emit vpTransformationStarted ( LEFTVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) 
    aView3d->SetProj (V3d_Yneg);
  myViewPort->fitAll();
  emit vpTransformationFinished ( LEFTVIEW );
}

/*!
  \brief Perform "right view" transformation.
*/
void XGUI_ViewWindow::rightView()
{
  emit vpTransformationStarted ( RIGHTVIEW );
  Handle(V3d_View) aView3d = myViewPort->getView();
  if ( !aView3d.IsNull() ) 
    aView3d->SetProj (V3d_Ypos);
  myViewPort->fitAll();
  emit vpTransformationFinished ( RIGHTVIEW );
}

void XGUI_ViewWindow::reset()
{
  emit vpTransformationStarted( RESETVIEW );
  bool upd = myViewPort->getView()->SetImmediateUpdate( false );
  myViewPort->getView()->Reset( false );
  myViewPort->fitAll( false, true, false );
  myViewPort->getView()->SetImmediateUpdate( upd );
  myViewPort->getView()->Update();
  emit vpTransformationFinished( RESETVIEW );
}


void XGUI_ViewWindow::updateToolBar()
{
  myGripWgt->update();
  myViewBar->update();
  myWindowBar->update();
}

