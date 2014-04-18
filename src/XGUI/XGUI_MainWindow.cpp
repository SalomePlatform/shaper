#include "XGUI_MainWindow.h"
#include "XGUI_Constants.h"
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
#include <QAction>

XGUI_MainWindow::XGUI_MainWindow(QWidget* parent)
    : QMainWindow(parent), 
    myObjectBrowser(0),
    myPythonConsole(0),
    myPropertyPanelDock(0)
{
  setWindowTitle(tr("New Geom"));
  myMenuBar = new XGUI_MainMenu(this);

  QMdiArea* aMdiArea = new QMdiArea(this);
  setCentralWidget(aMdiArea);

  myViewer = new XGUI_Viewer(this);

  //createDockWidgets();
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
    aDoc->setWidget(myPythonConsole);
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

void XGUI_MainWindow::showPropertyPanel()
{
  QAction* aViewAct = myPropertyPanelDock->toggleViewAction();
  //<! Restore ability to close panel from the window's menu
  aViewAct->setEnabled(true);
  myPropertyPanelDock->show();
  myPropertyPanelDock->raise();
}

void XGUI_MainWindow::hidePropertyPanel()
{
  QAction* aViewAct = myPropertyPanelDock->toggleViewAction();
  //<! Do not allow to show empty property panel
  aViewAct->setEnabled(false);
  myPropertyPanelDock->hide();
}

/*
 * Creates dock widgets, places them in corresponding area
 * and tabifies if necessary.
 */
void XGUI_MainWindow::createDockWidgets()
{
  QDockWidget* aObjDock = createObjectBrowser();
  addDockWidget(Qt::LeftDockWidgetArea, aObjDock);
  myPropertyPanelDock = createPropertyPanel();
  addDockWidget(Qt::LeftDockWidgetArea, myPropertyPanelDock);
  hidePropertyPanel(); //<! Invisible by default
  hideObjectBrowser();
  tabifyDockWidget(aObjDock, myPropertyPanelDock);
}

void XGUI_MainWindow::setPropertyPannelTitle(const QString& theTitle)
{
  myPropertyPanelDock->setWindowTitle(theTitle);
}


QDockWidget* XGUI_MainWindow::createPropertyPanel()
{
  QDockWidget* aPropPanel = new QDockWidget(this);
  aPropPanel->setWindowTitle(tr("Property Panel"));
  QAction* aViewAct = aPropPanel->toggleViewAction();
  aPropPanel->setObjectName(XGUI::PROP_PANEL);

  QWidget* aContent = new QWidget(aPropPanel);
  QVBoxLayout* aMainLay = new QVBoxLayout(aContent);
  aMainLay->setContentsMargins(3, 3, 3, 3);
  aPropPanel->setWidget(aContent);

  QWidget* aCustomWidget = new QWidget(aContent);
  aCustomWidget->setObjectName(XGUI::PROP_PANEL_WDG);
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
  //connect(aBtn, SIGNAL(clicked()), this, SIGNAL(propertyHelpPressed()));
  aBtnLay->addWidget(aBtn);
  aBtnLay->addStretch(1);
  aBtn = new QPushButton(QIcon(":pictures/button_ok.png"), "", aFrm);
  aBtn->setObjectName(XGUI::PROP_PANEL_OK);
  aBtn->setFlat(true);
  //connect(aBtn, SIGNAL(clicked()), this, SIGNAL(propertyOkPressed()));
  aBtnLay->addWidget(aBtn);
  aBtn = new QPushButton(QIcon(":pictures/button_cancel.png"), "", aFrm);
  aBtn->setObjectName(XGUI::PROP_PANEL_CANCEL);
  aBtn->setFlat(true);
  //connect(aBtn, SIGNAL(clicked()), this, SIGNAL(propertyClosePressed()));
  aBtnLay->addWidget(aBtn);

  return aPropPanel;
}

QDockWidget* XGUI_MainWindow::createObjectBrowser()
{
  QDockWidget* aObjDock = new QDockWidget(this);
  aObjDock->setAllowedAreas(Qt::LeftDockWidgetArea | Qt::RightDockWidgetArea);
  aObjDock->setWindowTitle(tr("Object browser"));
  myObjectBrowser = new XGUI_ObjectsBrowser(aObjDock);
  aObjDock->setWidget(myObjectBrowser);
  return aObjDock;
}
