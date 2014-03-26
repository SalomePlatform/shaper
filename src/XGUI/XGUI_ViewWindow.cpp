#include "XGUI_ViewWindow.h"
#include "XGUI_ViewPort.h"
#include "XGUI_Viewer.h"

#include <QLayout>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QResizeEvent>
#include <QApplication>
#include <QMdiArea>
#include <QMdiSubWindow>
#include <QPainter>

#define BORDER_SIZE 2



XGUI_ViewWindow::XGUI_ViewWindow(XGUI_Viewer* theViewer, 
                                 V3d_TypeOfView theType):
QFrame(),
    myViewer(theViewer),
    myMoving(false),
    MinimizeIco(":pictures/wnd_minimize.png"),
    MaximizeIco(":pictures/wnd_maximize.png"),
    CloseIco(":pictures/wnd_close.png"),
    RestoreIco(":pictures/wnd_restore.png")
{
    setFrameStyle(QFrame::Raised);
    setFrameShape(QFrame::Panel);
    setLineWidth(BORDER_SIZE);
    setMouseTracking(true);

    QVBoxLayout* aLay = new QVBoxLayout(this);
    aLay->setContentsMargins(BORDER_SIZE,BORDER_SIZE,BORDER_SIZE,BORDER_SIZE);
    myViewPort = new XGUI_ViewPort(this, myViewer->v3dViewer(), theType);
    aLay->addWidget(myViewPort);

    myPicture = new QLabel();
    myPicture->setFrameStyle(QFrame::Sunken);
    myPicture->setFrameShape(QFrame::Panel);
    //aLay->addWidget(myPicture);
    myPicture->setMouseTracking(true);
    myPicture->installEventFilter(this);
    myPicture->hide();

    QStringList aPictures;
    aPictures<<":pictures/occ_view_camera_dump.png"<<":pictures/occ_view_style_switch.png";
    aPictures<<":pictures/occ_view_triedre.png"<<":pictures/occ_view_fitall.png";
    aPictures<<":pictures/occ_view_fitarea.png"<<":pictures/occ_view_zoom.png";
    aPictures<<":pictures/occ_view_pan.png"<<":pictures/occ_view_glpan.png";
    aPictures<<":pictures/occ_view_rotate.png"<<":pictures/occ_view_front.png";
    aPictures<<":pictures/occ_view_back.png"<<":pictures/occ_view_left.png";
    aPictures<<":pictures/occ_view_right.png"<<":pictures/occ_view_top.png";
    aPictures<<":pictures/occ_view_bottom.png"<<":pictures/occ_view_clone.png";

    QStringList aTitles;
    aTitles << "Dump view" << "Mouse style switch" << "Show trihedron" << "Fit all";
    aTitles << "Fit area" << "Zoom" << "Panning" << "Global panning" << "Rotate";
    aTitles << "Front" << "Back" << "Left" << "Right" << "Top" << "Bottom" << "Clone view";

    myGripWgt = new QLabel(this);
    myGripWgt->setPixmap(QPixmap(":pictures/wnd_grip.png"));
    myGripWgt->setGeometry(BORDER_SIZE + 2, BORDER_SIZE + 2, 19, 32);
    myGripWgt->setMouseTracking(true);
    myGripWgt->installEventFilter(this);
    myGripWgt->setAutoFillBackground(true);

    myViewBar = new QToolBar(this);
    myViewBar->setAutoFillBackground(true);

    QAction* aBtn;
    for (int i = 0; i < aTitles.length(); i++) {
        aBtn = new QAction(QIcon(aPictures.at(i)), aTitles.at(i), myViewBar);
        myViewBar->addAction(aBtn);
    }
    
    myWindowBar = new QToolBar(this);
    myWindowBar->setAutoFillBackground(true);

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

    myViewBar->hide();
    myWindowBar->hide();
    myGripWgt->hide();
}

//****************************************************************
XGUI_ViewWindow::~XGUI_ViewWindow()
{
}

//****************************************************************
void XGUI_ViewWindow::resizeEvent(QResizeEvent* theEvent)
{
    QSize aSize = theEvent->size();
    QSize aWndBarSize = myWindowBar->sizeHint();
    QSize myViewBarSize = myViewBar->sizeHint();

    myWindowBar->move(aSize.width() - aWndBarSize.width() - BORDER_SIZE - 4, BORDER_SIZE);
    int aViewBarWidth = aSize.width() - aWndBarSize.width() - myGripWgt->width() - 8;
    if (aViewBarWidth > myViewBarSize.width())
        aViewBarWidth = myViewBarSize.width();
    myViewBar->setGeometry(BORDER_SIZE + 18, BORDER_SIZE, aViewBarWidth, myViewBarSize.height());
}

//****************************************************************
void XGUI_ViewWindow::changeEvent(QEvent* theEvent)
{

    if (theEvent->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            //parentWidget()->setGeometry(parentWidget()->x(), parentWidget()->y(), 100, 80);
            //myViewPort->hide();
            //myViewBar->hide();
            //myGripWgt->hide();
            //myWindowBar->hide();
            //myMinimizeBtn->setIcon(RestoreIco);
            //myMaximizeBtn->setIcon(MaximizeIco);

            if (!myPicture->parentWidget()) {
                QMdiSubWindow* aParent = static_cast<QMdiSubWindow*>(parentWidget());
                QMdiArea* aMDIArea = aParent->mdiArea();
                myPicture->setParent(aMDIArea);
            }
            myPicture->move(parentWidget()->x(), parentWidget()->y());
            myPicture->show();
        } else {
            //myViewPort->show();
            myPicture->hide();
            if (isMaximized()) {
                myMinimizeBtn->setIcon(MinimizeIco);
                myMaximizeBtn->setIcon(RestoreIco);
            }
        }
    } else
        QWidget::changeEvent(theEvent);
}

//****************************************************************
void XGUI_ViewWindow::onClose()
{
    if (parentWidget())
        parentWidget()->close();

}

//****************************************************************
void XGUI_ViewWindow::enterEvent(QEvent* theEvent)
{
    if (!isMinimized()) {
        myViewBar->show();
        if (!isMaximized())
            myGripWgt->show(); 
    }
    myWindowBar->show();
}

//****************************************************************
void XGUI_ViewWindow::leaveEvent(QEvent* theEvent)
{
    myViewBar->hide();
    myGripWgt->hide(); 
    myWindowBar->hide();
}

//****************************************************************
void XGUI_ViewWindow::onMinimize()
{
    //QPixmap aPMap = myViewPort->grab();
    QPixmap aPMap = QPixmap::fromImage(myViewPort->dumpView());
    int aW = width();
    int aH = height();
    double aR = aW / 100.;
    myPicture->setPixmap(aPMap.scaled(100,  int(aH / aR)));

    myLastState = isMaximized()? MaximizedState : NormalState;
    //if (isMinimized()) {
        //myMinimizeBtn->setIcon(MinimizeIco);
        //showNormal();
    //} else {
        //myMinimizeBtn->setIcon(RestoreIco);
    showMinimized();
    //}
    //myMaximizeBtn->setIcon(MaximizeIco);
}

//****************************************************************
void XGUI_ViewWindow::onMaximize()
{
    if (isMaximized()) {
        myMaximizeBtn->setIcon(MaximizeIco);
        myGripWgt->show();
        showNormal();
    } else {
        myMaximizeBtn->setIcon(RestoreIco);
        myGripWgt->hide();
        showMaximized();
    }
    myMinimizeBtn->setIcon(MinimizeIco);
}

//****************************************************************
bool XGUI_ViewWindow::eventFilter(QObject *theObj, QEvent *theEvent)
{
    if ((theObj == myGripWgt) || (theObj == myPicture)) {
        QWidget* aWgt = (theObj == myPicture)? myPicture : static_cast<QWidget*>(parentWidget());
        switch (theEvent->type()) {
        case QEvent::MouseButtonPress: 
            {
                QMouseEvent* aEvent = static_cast<QMouseEvent*>(theEvent);
                if ((aEvent->button() == Qt::LeftButton) && (!myMoving)){
                    myMoving = true;
                    myMousePnt = aEvent->globalPos();
                    return true;
                }
            }
            break;
        case QEvent::MouseButtonRelease: 
            {
                QMouseEvent* aEvent = static_cast<QMouseEvent*>(theEvent);
                if ((aEvent->button() == Qt::LeftButton) && myMoving) {
                    myMoving = false;
                    return true;
                }
            }
            break;
        case QEvent::MouseMove: 
            {
                QMouseEvent* aEvent = static_cast<QMouseEvent*>(theEvent);
                if (myMoving) {
                    QMdiSubWindow* aParent = static_cast<QMdiSubWindow*>(parentWidget());
                    QMdiArea* aMDIArea = aParent->mdiArea();
                    
                    QPoint aPnt = aEvent->globalPos();
                    QPoint aMDIPnt = aMDIArea->mapFromGlobal(aPnt);
                    if (aMDIArea->rect().contains(aMDIPnt)) {
                        int aX = aWgt->x() + (aPnt.x() - myMousePnt.x());
                        int aY = aWgt->y() + (aPnt.y() - myMousePnt.y());
                        aWgt->move(aX, aY);
                        myMousePnt = aPnt;
                    }
                    return true;
                }
            }
        }
        if ((theObj == myPicture) && (theEvent->type() == QEvent::MouseButtonDblClick)) {
            myMoving = false;
            if (myLastState == MaximizedState)
                showMaximized();
            else
                showNormal();
            return true;
        }
    }
    return QFrame::eventFilter(theObj, theEvent);
}