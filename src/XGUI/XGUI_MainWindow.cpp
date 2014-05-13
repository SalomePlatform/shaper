#include "XGUI_MainWindow.h"
#include "XGUI_Constants.h"
#include "XGUI_MainMenu.h"
#include "XGUI_ViewWindow.h"
#include "XGUI_Viewer.h"
#include "XGUI_ObjectsBrowser.h"

#include <PyConsole_Console.h>
#include <PyConsole_EnhInterp.h>

#include <QMdiArea>
#include <QMdiSubWindow>
#include <QAction>
#include <QDockWidget>
#include <QApplication>

XGUI_MainWindow::XGUI_MainWindow(QWidget* parent)
    : QMainWindow(parent), 
    myPythonConsole(0)
{
  setWindowTitle(tr("New Geom"));
  myMenuBar = new XGUI_MainMenu(this);

  QMdiArea* aMdiArea = new QMdiArea(this);
  aMdiArea->setContextMenuPolicy(Qt::ActionsContextMenu);
  setCentralWidget(aMdiArea);
  connect(aMdiArea, SIGNAL(subWindowActivated(QMdiSubWindow*)), 
          this, SLOT(onViewActivated(QMdiSubWindow*)));

  // Create actions of MDI area
  QAction* aAction = new QAction(QIcon(":pictures/new_view.png"), tr("Create Window"), aMdiArea);
  aMdiArea->addAction(aAction);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(createSubWindow()));
  
  aAction = new QAction(QIcon(":pictures/tile_views.png"), tr("Tile"), aMdiArea);
  aMdiArea->addAction(aAction);
  connect(aAction, SIGNAL(triggered(bool)), aMdiArea, SLOT(tileSubWindows()));
  
  aAction = new QAction(QIcon(":pictures/cascade_views.png"), tr("Cascade"), aMdiArea);
  aMdiArea->addAction(aAction);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(cascadeWindows()));

  myViewer = new XGUI_Viewer(this);
  connect(myViewer, SIGNAL(viewCreated(XGUI_ViewWindow*)), 
          this, SLOT(onViewCreated(XGUI_ViewWindow*)));
  connect(myViewer, SIGNAL(deleteView(XGUI_ViewWindow*)), 
          this, SLOT(onViewCreated(XGUI_ViewWindow*)));
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

//******************************************************
void XGUI_MainWindow::createSubWindow()
{
  viewer()->createView();
}

//******************************************************
void XGUI_MainWindow::cascadeWindows()
{
  QMdiArea* aMdiArea = static_cast<QMdiArea*>(centralWidget());
  QList<QMdiSubWindow*> aWindows = aMdiArea->subWindowList();
  
  QSize aSize = aMdiArea->size();
  QRect aRect = aMdiArea->geometry();
  const int aOffset = 30;
  int i = 0, j = 0;
  int x, y;
  int w = aSize.width() / 2;
  int h = aSize.height() / 2;
  QMdiSubWindow* aLastWnd;
  foreach(QMdiSubWindow* aWnd, aWindows) {
    aWnd->showNormal();
    aWnd->raise();
    x = aOffset * i;
    if ((x + w) > aSize.width()) {
      x = 0;
      i = 0;
    }
    y = aOffset * j;
    if ((y + h) > aSize.height()) {
      y = 0;
      j = 0;
    }
    aWnd->setGeometry(QStyle::visualRect(aWnd->layoutDirection(), aRect, 
      QRect(x, y, w, h)));
    i++;
    j++;
    viewer()->onWindowActivated(aWnd);
    aLastWnd = aWnd;
    QApplication::processEvents();
  }
  aLastWnd->setFocus();
}

void XGUI_MainWindow::onViewCreated(XGUI_ViewWindow* theWindow)
{
  QWidget* aSubWindow = theWindow->parentWidget();
  QWidget* aMDIWidget = centralWidget();
  QAction* aAction;
  if (aMDIWidget->actions().size() == 3) {
    aAction = new QAction(aMDIWidget);
    aAction->setSeparator(true);
    aMDIWidget->addAction(aAction);
  }
  aAction = new QAction(aSubWindow->windowTitle(), aMDIWidget);
  aAction->setCheckable(true);
  connect(aAction, SIGNAL(triggered(bool)), this, SLOT(activateView()));
  aMDIWidget->addAction(aAction);

  QList<QAction*> aActions = aMDIWidget->actions();
  foreach(QAction* aAct, aActions) {
    if (aAct->isCheckable())
      aAct->setChecked(false);
  }
  aAction->setChecked(true);
}

void XGUI_MainWindow::onDeleteView(XGUI_ViewWindow* theWindow)
{
  QWidget* aSubWindow = theWindow->parentWidget();
  QString aTitle = aSubWindow->windowTitle();
  QWidget* aMDIWidget = centralWidget();
  QList<QAction*> aActions = aMDIWidget->actions();

  QAction* aDelAct = 0;
  foreach(QAction* aAct, aActions) {
    if (aAct->text() == aTitle) {
      aDelAct = aAct;
      break;
    }
  }
  aMDIWidget->removeAction(aDelAct);
}

void XGUI_MainWindow::activateView()
{
  QAction* aAction = static_cast<QAction*>(sender());
  QString aWndTitle = aAction->text();
  QMdiArea* aMdiArea = static_cast<QMdiArea*>(centralWidget());

  QList<QMdiSubWindow*> aWndList = aMdiArea->subWindowList();
  foreach(QMdiSubWindow* aWnd, aWndList) {
    if (aWnd->windowTitle() == aWndTitle) {
      aWnd->raise();
      aWnd->activateWindow();
      aWnd->setFocus();
      break;
    }
  }
  QApplication::processEvents();
}

void XGUI_MainWindow::onViewActivated(QMdiSubWindow* theSubWnd)
{
  if (!theSubWnd)
    return;
  QMdiArea* aMdiArea = static_cast<QMdiArea*>(centralWidget());
  QString aWndTitle = theSubWnd->windowTitle();
  QList<QAction*> aActionList = aMdiArea->actions();
  foreach(QAction* aAct, aActionList) {
    if (aAct->isCheckable())
      aAct->setChecked(aAct->text() == aWndTitle);
  }
}