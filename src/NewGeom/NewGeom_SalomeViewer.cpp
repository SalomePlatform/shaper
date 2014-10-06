#include "NewGeom_SalomeViewer.h"
#include "NewGeom_OCCSelector.h"

#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewFrame.h>

#include <SUIT_ViewManager.h>

#include <QMouseEvent>
#include <QContextMenuEvent>

NewGeom_SalomeViewer::NewGeom_SalomeViewer(QObject* theParent)
    : ModuleBase_IViewer(theParent),
      mySelector(0)
{
}

//**********************************************
Handle(AIS_InteractiveContext) NewGeom_SalomeViewer::AISContext() const
{
  if (mySelector && mySelector->viewer())
    return mySelector->viewer()->getAISContext();
  Handle(AIS_InteractiveContext) aNull;
  return aNull;
}

//**********************************************
Handle(V3d_Viewer) NewGeom_SalomeViewer::v3dViewer() const
{
  return mySelector->viewer()->getViewer3d();
}

//**********************************************
Handle(V3d_View) NewGeom_SalomeViewer::activeView() const
{
  OCCViewer_Viewer* aViewer = mySelector->viewer();
  SUIT_ViewManager* aMgr = aViewer->getViewManager();
  OCCViewer_ViewWindow* aWnd = static_cast<OCCViewer_ViewWindow*>(aMgr->getActiveView());
  return aWnd->getViewPort()->getView();
}

//**********************************************
void NewGeom_SalomeViewer::setSelector(NewGeom_OCCSelector* theSel)
{
  if (mySelector) {
    if (mySelector == theSel)
      return;
    else {
      mySelector->viewer()->getViewManager()->disconnect(this);
    }
  }
  mySelector = theSel;
  OCCViewer_Viewer* aViewer = mySelector->viewer();
  SUIT_ViewManager* aMgr = aViewer->getViewManager();

  connect(aMgr, SIGNAL(lastViewClosed(SUIT_ViewManager*)), this, SIGNAL(lastViewClosed()));
  connect(aMgr, SIGNAL(tryCloseView(SUIT_ViewManager*)), this, SIGNAL(tryCloseView()));
  connect(aMgr, SIGNAL(deleteView(SUIT_ViewManager*)), this, SIGNAL(deleteView()));
  connect(aMgr, SIGNAL(viewCreated(SUIT_ViewManager*)), this, SIGNAL(viewCreated()));
  connect(aMgr, SIGNAL(activated(SUIT_ViewManager*)), this, SIGNAL(activated()));

  connect(aMgr, SIGNAL(mousePress(SUIT_ViewWindow*, QMouseEvent*)), this,
          SLOT(onMousePress(SUIT_ViewWindow*, QMouseEvent*)));
  connect(aMgr, SIGNAL(mouseRelease(SUIT_ViewWindow*, QMouseEvent*)), this,
          SLOT(onMouseRelease(SUIT_ViewWindow*, QMouseEvent*)));
  connect(aMgr, SIGNAL(mouseDoubleClick(SUIT_ViewWindow*, QMouseEvent*)), this,
          SLOT(onMouseDoubleClick(SUIT_ViewWindow*, QMouseEvent*)));
  connect(aMgr, SIGNAL(mouseMove(SUIT_ViewWindow*, QMouseEvent*)), this,
          SLOT(onMouseMove(SUIT_ViewWindow*, QMouseEvent*)));
  connect(aMgr, SIGNAL(keyPress(SUIT_ViewWindow*, QKeyEvent*)), this,
          SLOT(onKeyPress(SUIT_ViewWindow*, QKeyEvent*)));
  connect(aMgr, SIGNAL(keyRelease(SUIT_ViewWindow*, QKeyEvent*)), this,
          SLOT(onKeyRelease(SUIT_ViewWindow*, QKeyEvent*)));

  connect(aViewer, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
}

//**********************************************
void NewGeom_SalomeViewer::onMousePress(SUIT_ViewWindow*, QMouseEvent* theEvent)
{
  emit mousePress(theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onMouseRelease(SUIT_ViewWindow*, QMouseEvent* theEvent)
{
  emit mouseRelease(theEvent);
  //if ((theEvent->button() == Qt::RightButton) && 
  //  (theEvent->modifiers() == Qt::NoModifier)) {
  //  QContextMenuEvent aEvent(QContextMenuEvent::Mouse, theEvent->pos(), theEvent->globalPos());
  //  emit contextMenuRequested(&aEvent);
  //}
}

//**********************************************
void NewGeom_SalomeViewer::onMouseDoubleClick(SUIT_ViewWindow*, QMouseEvent* theEvent)
{
  emit mouseDoubleClick(theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onMouseMove(SUIT_ViewWindow*, QMouseEvent* theEvent)
{
  emit mouseMove(theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onKeyPress(SUIT_ViewWindow*, QKeyEvent* theEvent)
{
  emit keyPress(theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onKeyRelease(SUIT_ViewWindow*, QKeyEvent* theEvent)
{
  emit keyRelease(theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::enableSelection(bool isEnabled)
{
  mySelector->viewer()->enableSelection(isEnabled);
}

//**********************************************
bool NewGeom_SalomeViewer::isSelectionEnabled() const
{
  return mySelector->viewer()->isSelectionEnabled();
}

//**********************************************
void NewGeom_SalomeViewer::enableMultiselection(bool isEnable)
{
  mySelector->viewer()->enableMultiselection(isEnable);
}

//**********************************************
bool NewGeom_SalomeViewer::isMultiSelectionEnabled() const
{
  return mySelector->viewer()->isMultiSelectionEnabled();
}

//**********************************************
void NewGeom_SalomeViewer::fitAll()
{
  SUIT_ViewManager* aMgr = mySelector->viewer()->getViewManager();
  OCCViewer_ViewFrame* aVFrame = dynamic_cast<OCCViewer_ViewFrame*>(aMgr->getActiveView());
  if (aVFrame) {
    aVFrame->onFitAll();
  }
}

//**********************************************
void NewGeom_SalomeViewer::setViewProjection(double theX, double theY, double theZ)
{
  SUIT_ViewManager* aMgr = mySelector->viewer()->getViewManager();
  OCCViewer_ViewFrame* aVFrame = dynamic_cast<OCCViewer_ViewFrame*>(aMgr->getActiveView());
  if (aVFrame) {
    Handle(V3d_View) aView3d = aVFrame->getViewPort()->getView();
    if (!aView3d.IsNull()) {
      aView3d->SetProj(theX, theY, theZ);
      aView3d->FitAll(0.01, true, true);
      aView3d->SetZSize(0.);
    }
  }
}