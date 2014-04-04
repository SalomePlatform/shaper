#include "XGUI_MainWindow.h"
#include "XGUI_MainMenu.h"
#include "XGUI_ViewWindow.h"
#include "XGUI_Viewer.h"

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
  setWindowTitle(tr("WINDOW_TITLE"));
  myMenuBar = new XGUI_MainMenu(this);

  QMdiArea* aMdiArea = new QMdiArea(this);
  setCentralWidget(aMdiArea);

  myViewer = new XGUI_Viewer(this);

  createDockWidgets();
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

/*
 * Creates dock widgets, places them in corresponding area
 * and tabifies if necessary.
 */
void XGUI_MainWindow::createDockWidgets()
{
  QDockWidget* aObjDock = createObjectBrowser();
  addDockWidget(Qt::LeftDockWidgetArea, aObjDock);
  QDockWidget* aPropPanelDock = createPropertyPanel();
  addDockWidget(Qt::LeftDockWidgetArea, aPropPanelDock);

  tabifyDockWidget(aPropPanelDock, aObjDock);
}


QDockWidget* XGUI_MainWindow::createPropertyPanel()
{
  QDockWidget* aPropPanel = new QDockWidget(this);
  aPropPanel->setWindowTitle(tr("Property Panel"));

  QWidget* aContent = new QWidget(aPropPanel);
  QVBoxLayout* aMainLay = new QVBoxLayout(aContent);
  aMainLay->setContentsMargins(3, 3, 3, 3);
  aPropPanel->setWidget(aContent);

  QWidget* aCustomWidget = new QWidget(aContent);
  aCustomWidget->setObjectName("PropertyPanelWidget");
  aMainLay->addWidget(aCustomWidget);
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

  return aPropPanel;
}

QDockWidget* XGUI_MainWindow::createObjectBrowser()
{
  QDockWidget* aObjDock = new QDockWidget(this);
  aObjDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  aObjDock->setWindowTitle(tr("OBJECT_BROWSER_TITLE"));
  myObjectBrowser = new QTreeWidget(aObjDock);
  myObjectBrowser->setColumnCount(1);
  myObjectBrowser->setHeaderHidden(true);
  aObjDock->setWidget(myObjectBrowser);
//  fillObjectBrowser();
  return aObjDock;
}

//******************************************************

// TEST FUNCTIONS

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
}
