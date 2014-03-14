#include "XGUI_ViewWindow.h"


#include <QLayout>
#include <QLabel>
#include <QToolBar>
#include <QAction>
#include <QResizeEvent>
#include <QApplication>

#define BORDER_SIZE 2

XGUI_ViewWindow::XGUI_ViewWindow()
{
    setMouseTracking(true);
    QVBoxLayout* aLay = new QVBoxLayout(this);
    aLay->setContentsMargins(BORDER_SIZE,BORDER_SIZE,BORDER_SIZE,BORDER_SIZE);
    myViewPort = new QLabel(this);
    aLay->addWidget(myViewPort);
    myViewPort->setFrameStyle(QFrame::Raised);
    myViewPort->setCursor(Qt::ArrowCursor);
    myViewPort->setFrameShape(QFrame::Panel);
    myViewPort->setPixmap(QPixmap(":pictures/ViewPort.png"));
    myViewPort->setScaledContents(true);

    myPicture = new QLabel(this);
    aLay->addWidget(myPicture);
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

    aViewBar = new QToolBar(this);
    //aViewBar->move(BORDER_SIZE, BORDER_SIZE);

    QAction* aBtn;
    for (int i = 0; i < aTitles.length(); i++) {
        aBtn = new QAction(QIcon(aPictures.at(i)), aTitles.at(i), aViewBar);
        aViewBar->addAction(aBtn);
    }
    
    aWindowBar = new QToolBar(this);
    //aWindowBar->move(615,0);

    aBtn = new QAction(aWindowBar);
    aBtn->setIcon(QIcon(":pictures/wnd_minimize.png"));
    aWindowBar->addAction(aBtn);
    connect(aBtn, SIGNAL(triggered()), SLOT(onMinimize()));
    connect(aBtn, SIGNAL(triggered()), SLOT(showMinimized()));

    aBtn = new QAction(aWindowBar);
    aBtn->setIcon(QIcon(":pictures/wnd_maximize.png"));
    aWindowBar->addAction(aBtn);
    connect(aBtn, SIGNAL(triggered()), SLOT(showMaximized()));

    aBtn = new QAction(aWindowBar);
    aBtn->setIcon(QIcon(":pictures/wnd_close.png"));
    aWindowBar->addAction(aBtn);
    connect(aBtn, SIGNAL(triggered()), SLOT(onClose()));

    aViewBar->hide();
    aWindowBar->hide();
}


XGUI_ViewWindow::~XGUI_ViewWindow()
{
}


void XGUI_ViewWindow::resizeEvent(QResizeEvent* theEvent)
{
    QSize aSize = theEvent->size();
    QSize aWndBarSize = aWindowBar->sizeHint();
    QSize aViewBarSize = aViewBar->sizeHint();

    aWindowBar->move(aSize.width() - aWndBarSize.width() - BORDER_SIZE, BORDER_SIZE);
    aViewBar->setGeometry(BORDER_SIZE, BORDER_SIZE, aSize.width() - aWndBarSize.width(), aViewBarSize.height());
}


void XGUI_ViewWindow::changeEvent(QEvent* theEvent)
{

    if (theEvent->type() == QEvent::WindowStateChange) {
        if (isMinimized()) {
            parentWidget()->setGeometry(0, 0, 110, 80);
            myViewPort->hide();
            aViewBar->hide();
            aWindowBar->hide();
            myPicture->show();
        } else {
            myViewPort->show();
            myPicture->hide();
        }
    } else
        QWidget::changeEvent(theEvent);
}

void XGUI_ViewWindow::onClose()
{
    if (parentWidget())
        parentWidget()->close();

}

void XGUI_ViewWindow::enterEvent(QEvent* theEvent)
{
    if (!isMinimized())
        aViewBar->show();
    aWindowBar->show();
}

void XGUI_ViewWindow::leaveEvent(QEvent* theEvent)
{
    aViewBar->hide();
    aWindowBar->hide();
}


void XGUI_ViewWindow::onMinimize()
{
    QPixmap aPMap = grab();
    myPicture->setPixmap(aPMap.scaled(110, 80));
}