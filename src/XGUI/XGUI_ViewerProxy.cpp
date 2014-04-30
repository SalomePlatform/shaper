#include "XGUI_ViewerProxy.h"
#include "XGUI_Workshop.h"
#include "XGUI_MainWindow.h"
#include "XGUI_ViewPort.h"
#include "XGUI_ViewWindow.h"
#include "XGUI_Viewer.h"
#include "XGUI_SalomeConnector.h"


XGUI_ViewerProxy::XGUI_ViewerProxy(XGUI_Workshop* theParent)
: XGUI_SalomeViewer(theParent), myWorkshop(theParent)
{
}


Handle(AIS_InteractiveContext) XGUI_ViewerProxy::AISContext() const
{
  if (myWorkshop->isSalomeMode()) {
    return myWorkshop->salomeConnector()->viewer()->AISContext();
  } else {
    return myWorkshop->mainWindow()->viewer()->AISContext();
  }
}

Handle(V3d_Viewer) XGUI_ViewerProxy::v3dViewer() const
{
  if (myWorkshop->isSalomeMode()) {
    return myWorkshop->salomeConnector()->viewer()->v3dViewer();
  } else {
    return myWorkshop->mainWindow()->viewer()->v3dViewer();
  }
}

Handle(V3d_View) XGUI_ViewerProxy::activeView() const
{
  if (myWorkshop->isSalomeMode()) {
    return myWorkshop->salomeConnector()->viewer()->activeView();
  } else {
    XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
    return aViewer->activeViewWindow()->viewPort()->getView();
  }
}
 
void XGUI_ViewerProxy::connectToViewer()
{
  if (myWorkshop->isSalomeMode()) {
    XGUI_SalomeViewer* aViewer = myWorkshop->salomeConnector()->viewer();

    connect(aViewer, SIGNAL(lastViewClosed()),
            this, SIGNAL(lastViewClosed()));
    connect(aViewer, SIGNAL(tryCloseView()),
            this, SIGNAL(tryCloseView()));
    connect(aViewer, SIGNAL(deleteView()),
            this, SIGNAL(deleteView()));
    connect(aViewer, SIGNAL(viewCreated()),
            this, SIGNAL(viewCreated()));
    connect(aViewer, SIGNAL(activated()),
            this, SIGNAL(activated()));

    connect(aViewer, SIGNAL(mousePress(QMouseEvent* theEvent)),
            this, SIGNAL(mousePress(QMouseEvent* theEvent)));
    connect(aViewer, SIGNAL(mouseRelease(QMouseEvent* theEvent)),
            this, SIGNAL(mouseRelease(QMouseEvent* theEvent)));
    connect(aViewer, SIGNAL(mouseDoubleClick(QMouseEvent* theEvent)),
            this, SIGNAL(mouseDoubleClick(QMouseEvent* theEvent)));
    connect(aViewer, SIGNAL(mouseMove(QMouseEvent* theEvent)),
            this, SIGNAL(mouseMove(QMouseEvent* theEvent)));
    connect(aViewer, SIGNAL(keyPress(QKeyEvent* theEvent)),
            this, SIGNAL(keyPress(QKeyEvent* theEvent)));
    connect(aViewer, SIGNAL(keyRelease(QKeyEvent* theEvent)),
            this, SIGNAL(keyRelease(QKeyEvent* theEvent)));
  } else {
    XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();

    connect(aViewer, SIGNAL(lastViewClosed()),
            this, SIGNAL(lastViewClosed()));
    connect(aViewer, SIGNAL(tryCloseView(XGUI_ViewWindow*)),
            this, SIGNAL(tryCloseView()));
    connect(aViewer, SIGNAL(deleteView(XGUI_ViewWindow*)),
            this, SIGNAL(deleteView()));
    connect(aViewer, SIGNAL(viewCreated(XGUI_ViewWindow*)),
            this, SIGNAL(viewCreated()));
    connect(aViewer, SIGNAL(activated(XGUI_ViewWindow*)),
            this, SIGNAL(activated()));

    connect(aViewer, SIGNAL(mousePress(XGUI_ViewWindow*, QMouseEvent*)),
            this, SLOT(onMousePress(XGUI_ViewWindow*, QMouseEvent*)));
    connect(aViewer, SIGNAL(mouseRelease(XGUI_ViewWindow*, QMouseEvent*)),
            this, SLOT(onMouseRelease(XGUI_ViewWindow*, QMouseEvent*)));
    connect(aViewer, SIGNAL(mouseDoubleClick(XGUI_ViewWindow*, QMouseEvent*)),
            this, SLOT(onMouseDoubleClick(XGUI_ViewWindow*, QMouseEvent*)));
    connect(aViewer, SIGNAL(mouseMove(XGUI_ViewWindow*, QMouseEvent*)),
            this, SLOT(onMouseMove(XGUI_ViewWindow*, QMouseEvent*)));
    connect(aViewer, SIGNAL(keyPress(XGUI_ViewWindow*, QKeyEvent*)),
            this, SLOT(onKeyPress(XGUI_ViewWindow*, QKeyEvent*)));
    connect(aViewer, SIGNAL(keyRelease(XGUI_ViewWindow*, QKeyEvent*)),
            this, SLOT(onKeyRelease(XGUI_ViewWindow*, QKeyEvent*)));
  }
}

void XGUI_ViewerProxy::onMousePress(XGUI_ViewWindow*, QMouseEvent* theEvent)
{
  emit mousePress(theEvent);
}

void XGUI_ViewerProxy::onMouseRelease(XGUI_ViewWindow*, QMouseEvent* theEvent)
{
  emit mouseRelease(theEvent);
}

void XGUI_ViewerProxy::onMouseDoubleClick(XGUI_ViewWindow*, QMouseEvent* theEvent)
{
  emit mouseDoubleClick(theEvent);
}

void XGUI_ViewerProxy::onMouseMove(XGUI_ViewWindow*, QMouseEvent* theEvent)
{
  emit mouseMove(theEvent);
}

void XGUI_ViewerProxy::onKeyPress(XGUI_ViewWindow*, QKeyEvent* theEvent)
{
  emit keyPress(theEvent);
}

void XGUI_ViewerProxy::onKeyRelease(XGUI_ViewWindow*, QKeyEvent* theEvent)
{
  emit keyRelease(theEvent);
}
