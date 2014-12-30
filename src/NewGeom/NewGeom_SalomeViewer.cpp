// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "NewGeom_SalomeViewer.h"
#include "NewGeom_OCCSelector.h"

#include <OCCViewer_ViewWindow.h>
#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewFrame.h>

#include <SUIT_ViewManager.h>

#include <QMouseEvent>
#include <QContextMenuEvent>


Handle(V3d_View) NewGeom_SalomeView::v3dView() const
{
  SUIT_ViewManager* aMgr = myViewer->getViewManager();
  OCCViewer_ViewWindow* aWnd = static_cast<OCCViewer_ViewWindow*>(aMgr->getActiveView());
  Handle(V3d_View) aView = aWnd->getViewPort()->getView();
  return aView;
}

//**********************************************
//**********************************************
//**********************************************



NewGeom_SalomeViewer::NewGeom_SalomeViewer(QObject* theParent)
    : ModuleBase_IViewer(theParent),
      mySelector(0), myView(0), myIsSelectionChanged(false)
{
}

NewGeom_SalomeViewer::~NewGeom_SalomeViewer()
{
  if (myView)
    delete myView;
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
      OCCViewer_Viewer* aViewer = mySelector->viewer();
      if (aViewer)
        aViewer->disconnect(this);
    }
  }
  mySelector = theSel;
  if (!mySelector)
    return;
  OCCViewer_Viewer* aViewer = mySelector->viewer();
  SUIT_ViewManager* aMgr = aViewer->getViewManager();

  myView = new NewGeom_SalomeView(mySelector->viewer());

  // TODO: Provide ModuleBase_IViewWindow interface
  connect(aMgr, SIGNAL(lastViewClosed(SUIT_ViewManager*)), this, SIGNAL(lastViewClosed()));

  connect(aMgr, SIGNAL(tryCloseView(SUIT_ViewWindow*)), 
          this, SLOT(onTryCloseView(SUIT_ViewWindow*)));
  connect(aMgr, SIGNAL(deleteView(SUIT_ViewWindow*)), 
          this, SLOT(onDeleteView(SUIT_ViewWindow*)));
  connect(aMgr, SIGNAL(viewCreated(SUIT_ViewWindow*)), 
          this, SLOT(onViewCreated(SUIT_ViewWindow*)));
  connect(aMgr, SIGNAL(activated(SUIT_ViewWindow*)), 
          this, SLOT(onActivated(SUIT_ViewWindow*)));

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

  connect(aViewer, SIGNAL(selectionChanged()), this, SLOT(onSelectionChanged()));
}

//**********************************************
void NewGeom_SalomeViewer::onSelectionChanged()
{
  // Selection event must be sent only after mouse release
  myIsSelectionChanged = true;
}

//**********************************************
void NewGeom_SalomeViewer::onMousePress(SUIT_ViewWindow*, QMouseEvent* theEvent)
{
  emit mousePress(myView, theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onMouseRelease(SUIT_ViewWindow*, QMouseEvent* theEvent)
{
  emit mouseRelease(myView, theEvent);
  if (myIsSelectionChanged) {
    emit selectionChanged();
    myIsSelectionChanged = false;
  }
}

//**********************************************
void NewGeom_SalomeViewer::onMouseDoubleClick(SUIT_ViewWindow*, QMouseEvent* theEvent)
{
  emit mouseDoubleClick(myView, theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onMouseMove(SUIT_ViewWindow*, QMouseEvent* theEvent)
{
  emit mouseMove(myView, theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onKeyPress(SUIT_ViewWindow*, QKeyEvent* theEvent)
{
  emit keyPress(myView, theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onKeyRelease(SUIT_ViewWindow*, QKeyEvent* theEvent)
{
  emit keyRelease(myView, theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onTryCloseView(SUIT_ViewWindow*)
{
  emit tryCloseView(myView);
}

//**********************************************
void NewGeom_SalomeViewer::onDeleteView(SUIT_ViewWindow*)
{
  emit deleteView(myView);
}

//**********************************************
void NewGeom_SalomeViewer::onViewCreated(SUIT_ViewWindow*)
{
  emit viewCreated(myView);
}

//**********************************************
void NewGeom_SalomeViewer::onActivated(SUIT_ViewWindow*)
{
  emit activated(myView);
}

//**********************************************
void NewGeom_SalomeViewer::enableSelection(bool isEnabled)
{
  //mySelector->viewer()->enableSelection(isEnabled);
  // The enableSelection() in SALOME 7.5 cause of forced Viewer update(we have blinking)
  // After this is corrected, the first row should be recommented, the last - removed
  mySelector->viewer()->setInteractionStyle(isEnabled ? SUIT_ViewModel::STANDARD
                                                      : SUIT_ViewModel::KEY_FREE);
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
      aView3d->FitAll(0.01, true);
      aView3d->SetZSize(0.);
    }
  }
}

//***************************************
void NewGeom_SalomeViewer::addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->AddFilter(theFilter);
  }
}

//***************************************
void NewGeom_SalomeViewer::removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->RemoveFilter(theFilter);
  }
}

//***************************************
void NewGeom_SalomeViewer::clearSelectionFilters()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->RemoveFilters();
  }
}

//***************************************
void NewGeom_SalomeViewer::update()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->UpdateCurrentViewer();
  }
}
