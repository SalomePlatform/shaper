#include "XGUI_ViewerProxy.h"
#include "XGUI_Workshop.h"
#include "XGUI_MainWindow.h"
#include "XGUI_ViewPort.h"
#include "XGUI_ViewWindow.h"
#include "XGUI_Viewer.h"
#include "XGUI_SalomeConnector.h"

XGUI_ViewerProxy::XGUI_ViewerProxy(XGUI_Workshop* theParent)
    : ModuleBase_IViewer(theParent),
      myWorkshop(theParent)
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
    return (aViewer->activeViewWindow()) ? aViewer->activeViewWindow()->viewPort()->getView() :
    Handle(V3d_View)();
  }
}

void XGUI_ViewerProxy::setViewProjection(double theX, double theY, double theZ)
{
  Handle(V3d_View) aView3d = activeView();
  if (!aView3d.IsNull()) {
    aView3d->SetProj(theX, theY, theZ);
    aView3d->FitAll(0.01, true, true);
    aView3d->SetZSize(0.);
  }
}

void XGUI_ViewerProxy::fitAll()
{
  if (myWorkshop->isSalomeMode()) {
    myWorkshop->salomeConnector()->viewer()->fitAll();
  } else {
    XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
    if (aViewer->activeViewWindow())
      aViewer->activeViewWindow()->viewPort()->fitAll();
  }
}

void XGUI_ViewerProxy::connectToViewer()
{
  if (myWorkshop->isSalomeMode()) {
    ModuleBase_IViewer* aViewer = myWorkshop->salomeConnector()->viewer();

    connect(aViewer, SIGNAL(lastViewClosed()), this, SIGNAL(lastViewClosed()));
    connect(aViewer, SIGNAL(tryCloseView(ModuleBase_IViewWindow*)), 
      this, SIGNAL(tryCloseView(ModuleBase_IViewWindow*)));

    connect(aViewer, SIGNAL(deleteView(ModuleBase_IViewWindow*)), 
      this, SIGNAL(deleteView(ModuleBase_IViewWindow*)));

    connect(aViewer, SIGNAL(viewCreated(ModuleBase_IViewWindow*)), 
      this, SIGNAL(viewCreated(ModuleBase_IViewWindow*)));

    connect(aViewer, SIGNAL(activated(ModuleBase_IViewWindow*)), 
      this, SIGNAL(activated(ModuleBase_IViewWindow*)));

    connect(aViewer, SIGNAL(mousePress(ModuleBase_IViewWindow*, QMouseEvent*)), 
      this, SIGNAL(mousePress(ModuleBase_IViewWindow*, QMouseEvent*)));

    connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)), 
      this, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));

    connect(aViewer, SIGNAL(mouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)), this,
            SIGNAL(mouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)));

    connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)), 
      this, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));

    connect(aViewer, SIGNAL(keyPress(ModuleBase_IViewWindow*, QKeyEvent*)), 
      this, SIGNAL(keyPress(ModuleBase_IViewWindow*, QKeyEvent*)));

    connect(aViewer, SIGNAL(keyRelease(ModuleBase_IViewWindow*, QKeyEvent*)), 
      this, SIGNAL(keyRelease(ModuleBase_IViewWindow*, QKeyEvent*)));

    connect(aViewer, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
    connect(aViewer, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
            SIGNAL(contextMenuRequested(QContextMenuEvent*)));

  } else {
    XGUI_Viewer* aViewer = myWorkshop->mainWindow()->viewer();

    connect(aViewer, SIGNAL(lastViewClosed()), this, SIGNAL(lastViewClosed()));

    connect(aViewer, SIGNAL(tryCloseView(XGUI_ViewWindow*)), 
            this, SLOT(onTryCloseView(XGUI_ViewWindow*)));

    connect(aViewer, SIGNAL(deleteView(XGUI_ViewWindow*)), 
            this, SLOT(onDeleteView(XGUI_ViewWindow*)));

    connect(aViewer, SIGNAL(viewCreated(XGUI_ViewWindow*)), 
            this, SLOT(onViewCreated(XGUI_ViewWindow*)));

    connect(aViewer, SIGNAL(activated(XGUI_ViewWindow*)), 
            this, SLOT(onActivated(XGUI_ViewWindow*)));

    connect(aViewer, SIGNAL(mousePress(XGUI_ViewWindow*, QMouseEvent*)), this,
            SLOT(onMousePress(XGUI_ViewWindow*, QMouseEvent*)));

    connect(aViewer, SIGNAL(mouseRelease(XGUI_ViewWindow*, QMouseEvent*)), this,
            SLOT(onMouseRelease(XGUI_ViewWindow*, QMouseEvent*)));

    connect(aViewer, SIGNAL(mouseDoubleClick(XGUI_ViewWindow*, QMouseEvent*)), this,
            SLOT(onMouseDoubleClick(XGUI_ViewWindow*, QMouseEvent*)));

    connect(aViewer, SIGNAL(mouseMove(XGUI_ViewWindow*, QMouseEvent*)), this,
            SLOT(onMouseMove(XGUI_ViewWindow*, QMouseEvent*)));

    connect(aViewer, SIGNAL(keyPress(XGUI_ViewWindow*, QKeyEvent*)), this,
            SLOT(onKeyPress(XGUI_ViewWindow*, QKeyEvent*)));

    connect(aViewer, SIGNAL(keyRelease(XGUI_ViewWindow*, QKeyEvent*)), this,
            SLOT(onKeyRelease(XGUI_ViewWindow*, QKeyEvent*)));

    connect(aViewer, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
    connect(aViewer, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
            SIGNAL(contextMenuRequested(QContextMenuEvent*)));
  }
}


void XGUI_ViewerProxy::onTryCloseView(XGUI_ViewWindow* theWnd)
{
  emit tryCloseView(theWnd);
}

void XGUI_ViewerProxy::onDeleteView(XGUI_ViewWindow* theWnd)
{
  emit deleteView(theWnd);
}

void XGUI_ViewerProxy::onViewCreated(XGUI_ViewWindow* theWnd)
{
  emit viewCreated(theWnd);
}

void XGUI_ViewerProxy::onActivated(XGUI_ViewWindow* theWnd)
{
  emit activated(theWnd);
}

void XGUI_ViewerProxy::onMousePress(XGUI_ViewWindow* theWnd, QMouseEvent* theEvent)
{
  emit mousePress(theWnd, theEvent);
}

void XGUI_ViewerProxy::onMouseRelease(XGUI_ViewWindow* theWnd, QMouseEvent* theEvent)
{
  emit mouseRelease(theWnd, theEvent);
}

void XGUI_ViewerProxy::onMouseDoubleClick(XGUI_ViewWindow* theWnd, QMouseEvent* theEvent)
{
  emit mouseDoubleClick(theWnd, theEvent);
}

void XGUI_ViewerProxy::onMouseMove(XGUI_ViewWindow* theWnd, QMouseEvent* theEvent)
{
  emit mouseMove(theWnd, theEvent);
}

void XGUI_ViewerProxy::onKeyPress(XGUI_ViewWindow* theWnd, QKeyEvent* theEvent)
{
  emit keyPress(theWnd, theEvent);
}

void XGUI_ViewerProxy::onKeyRelease(XGUI_ViewWindow* theWnd, QKeyEvent* theEvent)
{
  emit keyRelease(theWnd, theEvent);
}



//***************************************
void XGUI_ViewerProxy::enableSelection(bool isEnabled)
{
  if (myWorkshop->isSalomeMode()) {
    myWorkshop->salomeConnector()->viewer()->enableSelection(isEnabled);
  } else {
    myWorkshop->mainWindow()->viewer()->setSelectionEnabled(isEnabled);
  }
}

//***************************************
bool XGUI_ViewerProxy::isSelectionEnabled() const
{
  if (myWorkshop->isSalomeMode()) {
    return myWorkshop->salomeConnector()->viewer()->isSelectionEnabled();
  } else {
    return myWorkshop->mainWindow()->viewer()->isSelectionEnabled();
  }
}

//***************************************
void XGUI_ViewerProxy::enableMultiselection(bool isEnable)
{
  if (myWorkshop->isSalomeMode()) {
    myWorkshop->salomeConnector()->viewer()->enableMultiselection(isEnable);
  } else {
    myWorkshop->mainWindow()->viewer()->setMultiSelectionEnabled(isEnable);
  }
}

//***************************************
bool XGUI_ViewerProxy::isMultiSelectionEnabled() const
{
  if (myWorkshop->isSalomeMode()) {
    return myWorkshop->salomeConnector()->viewer()->isMultiSelectionEnabled();
  } else {
    return myWorkshop->mainWindow()->viewer()->isMultiSelectionEnabled();
  }
}

//***************************************
void XGUI_ViewerProxy::addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->AddFilter(theFilter);
  }
}

//***************************************
void XGUI_ViewerProxy::removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->RemoveFilter(theFilter);
  }
}

//***************************************
void XGUI_ViewerProxy::clearSelectionFilters()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->RemoveFilters();
  }
}
