#include "XGUI_MainWindow.h"
#include "XGUI_MainMenu.h"
#include "XGUI_ViewWindow.h"
#include "XGUI_Viewer.h"
#include "XGUI_ObjectsBrowser.h"

#include <PyConsole_Console.h>
#include <PyConsole_EnhInterp.h>

#include <QMdiArea>
#include <QTreeWidget>
#include <QDockWidget>
#include <QTextEdit>
#include <QLabel>
#include <QToolBar>
#include <QToolButton>
#include <QTreeWidgetItem>
#include <QLayout>
#include <QLineEdit>
#include <QGroupBox>
#include <QFormLayout>
#include <QDoubleSpinBox>
#include <QPushButton>
#include <QScrollArea>
#include <QComboBox>

XGUI_MainWindow::XGUI_MainWindow(QWidget* parent)
    : QMainWindow(parent), 
    myObjectBrowser(0), 
    myPythonConsole(0)
{
  setWindowTitle(tr("New Geom"));
  myMenuBar = new XGUI_MainMenu(this);

  QDockWidget* aDoc = new QDockWidget(this);
  aDoc->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  aDoc->setWindowTitle(tr("Object browser"));

  myObjectBrowser = new XGUI_ObjectsBrowser(aDoc);
  //myObjectBrowser->setColumnCount(1);
  //myObjectBrowser->setHeaderHidden(true);
  aDoc->setWidget(myObjectBrowser);
  addDockWidget(Qt::LeftDockWidgetArea, aDoc);
  //aDoc->hide();

  QMdiArea* aMdiArea = new QMdiArea(this);
  setCentralWidget(aMdiArea);

  myViewer = new XGUI_Viewer(this);

  //fillObjectBrowser();
  //addPropertyPanel();
}

XGUI_MainWindow::~XGUI_MainWindow(void)
{
}

//******************************************************
QMdiArea* XGUI_MainWindow::mdiArea() const
{
  return static_cast<QMdiArea*>(centralWidget());
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
void XGUI_MainWindow::showPythonConsole()
{
  if (!myPythonConsole) {

    QDockWidget* aDoc = new QDockWidget(this);
    aDoc->setFeatures(QDockWidget::AllDockWidgetFeatures | QDockWidget::DockWidgetVerticalTitleBar);
    aDoc->setMinimumHeight(0);
    aDoc->setWindowTitle("Console");
    myPythonConsole = new PyConsole_EnhConsole( aDoc, new PyConsole_EnhInterp());
    //myPythonConsole = new QTextEdit(aDoc);
    //myPythonConsole->setGeometry(0,0,200, 50);
    //myPythonConsole->setText(">>>");
    aDoc->setWidget(myPythonConsole);
    //myPythonConsole->setMinimumHeight(0);
    addDockWidget(Qt::TopDockWidgetArea, aDoc);
    tabifyDockWidget(myMenuBar->getLastDockWindow(), aDoc);
  }
  myPythonConsole->parentWidget()->show();
}

//******************************************************
void XGUI_MainWindow::hidePythonConsole()
{
  if (myPythonConsole)
    myPythonConsole->parentWidget()->hide();
}



//******************************************************

// TEST FUNCTIONS

//******************************************************
/*void XGUI_MainWindow::fillObjectBrowser()
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
  foreach(QString aName, aNames)
  {
    QTreeWidgetItem* aItem = new QTreeWidgetItem(myObjectBrowser);
    aItem->setText(0, aName);
    aItems.append(aItem);
  }
  for(int i = 0; i < aItems.length(); i++) {
    aItems[i]->setIcon(0, QIcon(aIcons[i]));
  }
  myObjectBrowser->addTopLevelItems(aItems);

  for(int i = 2; i < 5; i++) {
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
}*/

void XGUI_MainWindow::addPropertyPanel()
{
  QDockWidget* aPropPanel = new QDockWidget(this);
  aPropPanel->setWindowTitle("Point");

  QWidget* aContent = new QWidget(aPropPanel);
  QVBoxLayout* aMainLay = new QVBoxLayout(aContent);
  aMainLay->setContentsMargins(3, 3, 3, 3);
  aPropPanel->setWidget(aContent);

  /*QWidget* aNameWgt = new QWidget(aContent);
   QHBoxLayout* aNameLay = new QHBoxLayout(aNameWgt);
   aNameLay->setContentsMargins(0,0,0,0);
   aMainLay->addWidget(aNameWgt);

   aNameLay->addWidget(new QLabel("Name", aNameWgt));
   aNameLay->addWidget(new QLineEdit(aNameWgt));*/

  QComboBox* aCombo = new QComboBox(aContent);
  aCombo->addItem("By coordinates");
  aMainLay->addWidget(aCombo);

  QWidget* aGrpBox1 = new QWidget(aContent);
  //aGrpBox1->setFlat(true);
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

  /*    aGrpBox1 = new QGroupBox("Normal vector", aContent);
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
   aFrmLay->addRow(aLbl, new QDoubleSpinBox(aGrpBox1));*/

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
  aBtn = new QPushButton(QIcon(":pictures/button_ok.png"), "", aFrm);
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);
  aBtn = new QPushButton(QIcon(":pictures/button_cancel.png"), "", aFrm);
  aBtn->setFlat(true);
  aBtnLay->addWidget(aBtn);

  addDockWidget(Qt::RightDockWidgetArea, aPropPanel);
}
