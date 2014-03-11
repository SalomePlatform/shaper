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
#include <QLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QPushButton>

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
    //aDoc->hide();

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
    addPropertyPanel();
}


QWidget* XGUI_MainWindow::getSubWindow()
{
    QMdiSubWindow* aSub = new QMdiSubWindow(this);
    aSub->setGeometry(0,0, 600, 400);
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


void XGUI_MainWindow::addPropertyPanel()
{
    QDockWidget* aPropPanel = new QDockWidget(this);
    aPropPanel->setWindowTitle("Property panel");

    QWidget* aContent = new QWidget(aPropPanel);
    QVBoxLayout* aMainLay = new QVBoxLayout(aContent);
    aMainLay->setContentsMargins(3,3,3,3);
    aPropPanel->setWidget(aContent);

    QWidget* aNameWgt = new QWidget(aContent);
    QHBoxLayout* aNameLay = new QHBoxLayout(aNameWgt);
    aNameLay->setContentsMargins(0,0,0,0);
    aMainLay->addWidget(aNameWgt);

    aNameLay->addWidget(new QLabel("Name", aNameWgt));
    aNameLay->addWidget(new QLineEdit(aNameWgt));

    QGroupBox* aGrpBox1 = new QGroupBox("Point", aContent);
    aGrpBox1->setFlat(true);
    QFormLayout* aFrmLay = new QFormLayout(aGrpBox1);
    aFrmLay->setContentsMargins(0, 6, 0, 0);
    aMainLay->addWidget(aGrpBox1);

    QLabel* aLbl = new QLabel(aGrpBox1);
    aLbl->setPixmap(QPixmap(":pictures/x_point.png"));
    aFrmLay->addRow(aLbl, new QDoubleSpinBox(aGrpBox1));

    aLbl = new QLabel(aGrpBox1);
    aLbl->setPixmap(QPixmap(":pictures/y_point.png"));
    aFrmLay->addRow(aLbl, new QDoubleSpinBox(aGrpBox1));

    aLbl = new QLabel(aGrpBox1);
    aLbl->setPixmap(QPixmap(":pictures/z_point.png"));
    aFrmLay->addRow(aLbl, new QDoubleSpinBox(aGrpBox1));
    

    aGrpBox1 = new QGroupBox("Normal vector", aContent);
    aGrpBox1->setFlat(true);
    aFrmLay = new QFormLayout(aGrpBox1);
    aFrmLay->setContentsMargins(0, 6, 0, 0);
    aMainLay->addWidget(aGrpBox1);

    aLbl = new QLabel(aGrpBox1);
    aLbl->setPixmap(QPixmap(":pictures/x_size.png"));
    aFrmLay->addRow(aLbl, new QDoubleSpinBox(aGrpBox1));

    aLbl = new QLabel(aGrpBox1);
    aLbl->setPixmap(QPixmap(":pictures/y_size.png"));
    aFrmLay->addRow(aLbl, new QDoubleSpinBox(aGrpBox1));

    aLbl = new QLabel(aGrpBox1);
    aLbl->setPixmap(QPixmap(":pictures/z_size.png"));
    aFrmLay->addRow(aLbl, new QDoubleSpinBox(aGrpBox1));
    
    aMainLay->addStretch(1);

    QFrame* aFrm = new QFrame(aContent);
    aFrm->setFrameStyle(QFrame::Sunken);
    aFrm->setFrameShape(QFrame::Panel);
    QHBoxLayout* aBtnLay = new QHBoxLayout(aFrm);
    aBtnLay->setContentsMargins(0, 0, 0, 0);
    aMainLay->addWidget(aFrm);

    QPushButton* aBtn = new QPushButton(QIcon(":pictures/button_help.png"), "", aFrm);
    aBtn->setFlat(true);
    aBtnLay->addWidget(aBtn);
    aBtnLay->addStretch(1);
    aBtn = new QPushButton(QIcon(":pictures/button_cancel.png"), "", aFrm);
    aBtn->setFlat(true);
    aBtnLay->addWidget(aBtn);
    aBtn = new QPushButton(QIcon(":pictures/button_ok.png"), "", aFrm);
    aBtn->setFlat(true);
    aBtnLay->addWidget(aBtn);

    addDockWidget(Qt::RightDockWidgetArea, aPropPanel);
}