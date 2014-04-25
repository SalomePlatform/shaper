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
    myPythonConsole(0)
{
  setWindowTitle(tr("New Geom"));
  myMenuBar = new XGUI_MainMenu(this);

  QMdiArea* aMdiArea = new QMdiArea(this);
  setCentralWidget(aMdiArea);

  myViewer = new XGUI_Viewer(this);
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


