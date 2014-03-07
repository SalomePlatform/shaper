#include "XGUI_MainWindow.h"
#include "XGUI_MainMenu.h"

#include <QMdiArea>
#include <QTreeWidget>
#include <QDockWidget>
#include <QTextEdit>
#include <QMdiSubWindow>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QAction>
#include <QTreeWidgetItem>

XGUI_MainWindow::XGUI_MainWindow(QWidget* parent) :
    QMainWindow(parent), myObjectBrowser(0)
{
    //menuBar();
    myMenuBar = new XGUI_MainMenu(this);

    QDockWidget* aDoc = new QDockWidget(this);
    aDoc->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
    aDoc->setWindowTitle(tr("OBJECT_BROWSER_TITLE"));
    myObjectBrowser = new QTreeWidget(aDoc);
    myObjectBrowser->setColumnCount(1);
    myObjectBrowser->setHeaderHidden(true);
    aDoc->setWidget(myObjectBrowser);
    addDockWidget(Qt::LeftDockWidgetArea, aDoc);
    aDoc->hide();

    aDoc = new QDockWidget(this);
    aDoc->setFeatures(QDockWidget::AllDockWidgetFeatures | QDockWidget::DockWidgetVerticalTitleBar);
    aDoc->setMinimumHeight(20);
    aDoc->setWindowTitle("Console");
    QTextEdit* aTextEdt = new QTextEdit(aDoc);
    aTextEdt->setText(">>>");
    aTextEdt->setMinimumHeight(20);
    aDoc->setWidget(aTextEdt);
    addDockWidget(Qt::BottomDockWidgetArea, aDoc);

    QMdiArea* aMdiArea = new QMdiArea(this);
    setCentralWidget(aMdiArea);

    aMdiArea->addSubWindow(getSubWindow(), Qt::FramelessWindowHint);

    fillObjectBrowser();

}


QWidget* XGUI_MainWindow::getSubWindow()
{
    QMdiSubWindow* aSub = new QMdiSubWindow(this);
    QLabel* aLbl = new QLabel(aSub);
    aLbl->setFrameStyle(QFrame::Sunken);
    aLbl->setFrameShape(QFrame::Panel);
    aLbl->setPixmap(QPixmap(":pictures/ViewPort.png"));
    aLbl->setScaledContents(true);
    aSub->setWidget(aLbl);

    QStringList aPictures;
    aPictures<<":pictures/occ_view_camera_dump.png"<<":pictures/occ_view_style_switch.png";
    aPictures<<":pictures/occ_view_triedre.png"<<":pictures/occ_view_fitall.png";
    aPictures<<":pictures/occ_view_fitarea.png"<<":pictures/occ_view_zoom.png";
    aPictures<<":pictures/occ_view_pan.png"<<":pictures/occ_view_glpan.png";
    aPictures<<":pictures/occ_view_rotate.png"<<":pictures/occ_view_front.png";
    aPictures<<":pictures/occ_view_back.png"<<":pictures/occ_view_left.png";
    aPictures<<":pictures/occ_view_right.png"<<":pictures/occ_view_top.png";
    aPictures<<":pictures/occ_view_bottom.png"<<":pictures/occ_view_clone.png";

    QToolBar* aBar = new QToolBar(aSub);
    aBar->setGeometry(0,0,500,30);

    QAction* aBtn;
    foreach(QString aName, aPictures) {
        aBtn = new QAction(aBar);
        aBtn->setIcon(QIcon(aName));
        aBar->addAction(aBtn);
    }
    
    aBar = new QToolBar(aSub);
    aBar->setGeometry(615,0,100,25);
    QStringList aWndIcons;
    aWndIcons<<":pictures/wnd_minimize.png"<<":pictures/wnd_maximize.png"<<":pictures/wnd_close.png";
    foreach(QString aName, aWndIcons) {
        aBtn = new QAction(aBar);
        aBtn->setIcon(QIcon(aName));
        aBar->addAction(aBtn);
    }

    return aSub;
}


XGUI_MainWindow::~XGUI_MainWindow(void)
{
}

//******************************************************
void XGUI_MainWindow::showObjectBrowser()
{
    myObjectBrowser->parentWidget()->show();
}

//******************************************************
void XGUI_MainWindow::hideObjectBrowser()
{
    myObjectBrowser->parentWidget()->hide();
}

//******************************************************
void XGUI_MainWindow::fillObjectBrowser()
{
    QStringList aNames;
    aNames << "Parameters" << "Constructions";
    aNames << "Part 1" << "Part 2" << "Part 3";
    aNames << "Properties";

    QStringList aIcons;
    aIcons << ":pictures/params_folder.png";
    aIcons << ":pictures/constr_folder.png";
    aIcons << ":pictures/part_ico.png";
    aIcons << ":pictures/part_ico.png";
    aIcons << ":pictures/part_ico.png";
    aIcons << ":pictures/properties.png";

    QList<QTreeWidgetItem*> aItems;
    foreach(QString aName, aNames) {
        QTreeWidgetItem* aItem = new QTreeWidgetItem(myObjectBrowser);
        aItem->setText(0, aName);
        aItems.append(aItem);
    }
    for(int i = 0; i < aItems.length(); i++) {
        aItems[i]->setIcon(0, QIcon(aIcons[i]));
    }
    myObjectBrowser->addTopLevelItems(aItems);

    for (int i = 2; i < 5; i++) {
        QTreeWidgetItem* aItem = new QTreeWidgetItem(aItems[i]);
        aItem->setText(0, "Parameters");
        aItem->setIcon(0, QIcon(":pictures/params_folder.png"));

        aItem = new QTreeWidgetItem(aItems[i]);
        aItem->setText(0, "Construction");
        aItem->setIcon(0, QIcon(":pictures/constr_folder.png"));

        aItem = new QTreeWidgetItem(aItems[i]);
        aItem->setText(0, "Bodies");
        aItem->setIcon(0, QIcon(":pictures/part_ico.png"));

        aItem = new QTreeWidgetItem(aItems[i]);
        aItem->setText(0, "Features");
        aItem->setIcon(0, QIcon(":pictures/features.png"));
    }
}
