// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "NewGeom_SalomeViewer.h"
#include "NewGeom_OCCSelector.h"

#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewFrame.h>

#include <SUIT_ViewManager.h>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

#include <QMouseEvent>
#include <QContextMenuEvent>

NewGeom_SalomeView::NewGeom_SalomeView(OCCViewer_Viewer* theViewer)
: ModuleBase_IViewWindow(), myCurrentView(0)
{
  myViewer = theViewer;
}


Handle(V3d_View) NewGeom_SalomeView::v3dView() const
{
  Handle(V3d_View) aView;
  if (myCurrentView) {
    OCCViewer_ViewWindow* aWnd = static_cast<OCCViewer_ViewWindow*>(myCurrentView);
    aView = aWnd->getViewPort()->getView();
  }
  return aView;
}

QWidget* NewGeom_SalomeView::viewPort() const
{
  QWidget* aViewPort = 0;
  if (myCurrentView) {
    OCCViewer_ViewWindow* aWnd = static_cast<OCCViewer_ViewWindow*>(myCurrentView);
    aViewPort = aWnd->getViewPort();
  }
  return aViewPort;
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
  if (mySelector)
    return mySelector->viewer()->getViewer3d();
  return Handle(V3d_Viewer)();
}

//**********************************************
Handle(V3d_View) NewGeom_SalomeViewer::activeView() const
{
  if (mySelector) {
    OCCViewer_Viewer* aViewer = mySelector->viewer();
    SUIT_ViewManager* aMgr = aViewer->getViewManager();
    OCCViewer_ViewWindow* aWnd = static_cast<OCCViewer_ViewWindow*>(aMgr->getActiveView());
    return aWnd->getViewPort()->getView();
  }
  return Handle(V3d_View)();
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
  connect(aMgr, SIGNAL(activated(SUIT_ViewManager*)), 
          this, SLOT(onActivated(SUIT_ViewManager*)));

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
void NewGeom_SalomeViewer::onMousePress(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  myView->setCurrentView(theView);
  emit mousePress(myView, theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onMouseRelease(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  myView->setCurrentView(theView);
  emit mouseRelease(myView, theEvent);
  if (myIsSelectionChanged) {
    emit selectionChanged();
    myIsSelectionChanged = false;
  }
}

//**********************************************
void NewGeom_SalomeViewer::onMouseDoubleClick(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  myView->setCurrentView(theView);
  emit mouseDoubleClick(myView, theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onMouseMove(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  myView->setCurrentView(theView);
  emit mouseMove(myView, theEvent);
}

//**********************************************
bool NewGeom_SalomeViewer::canDragByMouse() const
{
  OCCViewer_Viewer* aViewer = mySelector->viewer();
  SUIT_ViewWindow* aWnd = aViewer->getViewManager()->getActiveView();
  OCCViewer_ViewWindow* aViewWnd = dynamic_cast<OCCViewer_ViewWindow*>(aWnd);
  if (aViewWnd) {
    return (aViewWnd->interactionStyle() == 0);
  }
  return true;
}


//**********************************************
void NewGeom_SalomeViewer::onKeyPress(SUIT_ViewWindow* theView, QKeyEvent* theEvent)
{
  emit keyPress(myView, theEvent);
}

//**********************************************
void NewGeom_SalomeViewer::onKeyRelease(SUIT_ViewWindow* theView, QKeyEvent* theEvent)
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
  if(myWindowScale.contains(myView->v3dView()))
    myWindowScale.remove(myView->v3dView());
  emit deleteView(myView);
}

//**********************************************
void NewGeom_SalomeViewer::onViewCreated(SUIT_ViewWindow* theView)
{
  myView->setCurrentView(theView);

  OCCViewer_ViewFrame* aView = dynamic_cast<OCCViewer_ViewFrame*>(theView);

  OCCViewer_ViewWindow* aWnd = aView->getView(OCCViewer_ViewFrame::MAIN_VIEW);
  if (aWnd)
    connect(aWnd, SIGNAL(vpTransformationFinished(OCCViewer_ViewWindow::OperationType)),
      this, SLOT(onViewTransformed(OCCViewer_ViewWindow::OperationType)));

  myWindowScale.insert (aView->getViewPort()->getView(), aView->getViewPort()->getView()->Camera()->Scale());

  emit viewCreated(myView);
}

//**********************************************
void NewGeom_SalomeViewer::onActivated(SUIT_ViewManager* theMgr)
{
  myView->setCurrentView(theMgr->getActiveView());
  emit activated(myView);
}

//**********************************************
void NewGeom_SalomeViewer::enableSelection(bool isEnabled)
{
  if (mySelector)
    if (mySelector->viewer()->isSelectionEnabled() != isEnabled)
      mySelector->viewer()->enableSelection(isEnabled);
  // The enableSelection() in SALOME 7.5 cause of forced Viewer update(we have blinking)
  // After this is corrected, the first row should be recommented, the last - removed
    //mySelector->viewer()->setInteractionStyle(isEnabled ? SUIT_ViewModel::STANDARD
    //                                                    : SUIT_ViewModel::KEY_FREE);
}

//**********************************************
bool NewGeom_SalomeViewer::isSelectionEnabled() const
{
  if (mySelector)
    return mySelector->viewer()->isSelectionEnabled();
}

//**********************************************
void NewGeom_SalomeViewer::enableMultiselection(bool isEnable)
{
  if (mySelector)
    mySelector->viewer()->enableMultiselection(isEnable);
}

//**********************************************
bool NewGeom_SalomeViewer::isMultiSelectionEnabled() const
{
  if (mySelector)
    return mySelector->viewer()->isMultiSelectionEnabled();
  return false;
}

//**********************************************
void NewGeom_SalomeViewer::fitAll()
{
  if (mySelector) {
    SUIT_ViewManager* aMgr = mySelector->viewer()->getViewManager();
    OCCViewer_ViewFrame* aVFrame = dynamic_cast<OCCViewer_ViewFrame*>(aMgr->getActiveView());
    if (aVFrame) {
      aVFrame->onFitAll();
    }
  }
}

//**********************************************
void NewGeom_SalomeViewer::setViewProjection(double theX, double theY, double theZ, double theTwist)
{
  if (!mySelector) 
    return;

  SUIT_ViewManager* aMgr = mySelector->viewer()->getViewManager();
  OCCViewer_ViewFrame* aVFrame = dynamic_cast<OCCViewer_ViewFrame*>(aMgr->getActiveView());
  if (aVFrame) {
    Handle(V3d_View) aView3d = aVFrame->getViewPort()->getView();
    if (!aView3d.IsNull()) {
      aView3d->SetProj(theX, theY, theZ);
      aView3d->SetTwist( theTwist );
      aView3d->FitAll(0.01, true);
      aView3d->SetZSize(0.);
      aView3d->DepthFitAll();
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
bool NewGeom_SalomeViewer::hasSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  bool aFoundFilter = false;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    const SelectMgr_ListOfFilter& aFilters = aContext->Filters();
    SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
    for (; aIt.More() && !aFoundFilter; aIt.Next()) {
      aFoundFilter = theFilter.Access() == aIt.Value().Access();
    }
  }
  return aFoundFilter;
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

//***************************************
void NewGeom_SalomeViewer::onViewTransformed(OCCViewer_ViewWindow::OperationType theType)
{
  emit viewTransformed((int) theType);
}

//***************************************
void NewGeom_SalomeViewer::activateViewer(bool toActivate)
{
  if (!mySelector || !mySelector->viewer())
    return;
  SUIT_ViewManager* aMgr = mySelector->viewer()->getViewManager();
  QVector<SUIT_ViewWindow*> aViews = aMgr->getViews();
  if (toActivate) {
    foreach (SUIT_ViewWindow* aView, aViews) {
      OCCViewer_ViewFrame* aOCCView = dynamic_cast<OCCViewer_ViewFrame*>(aView);
      OCCViewer_ViewWindow* aWnd = aOCCView->getView(OCCViewer_ViewFrame::MAIN_VIEW);
      connect(aWnd, SIGNAL(vpTransformationFinished(OCCViewer_ViewWindow::OperationType)),
        this, SLOT(onViewTransformed(OCCViewer_ViewWindow::OperationType)));
    }
  } else {
    foreach (SUIT_ViewWindow* aView, aViews) {
      OCCViewer_ViewFrame* aOCCView = dynamic_cast<OCCViewer_ViewFrame*>(aView);
      OCCViewer_ViewWindow* aWnd = aOCCView->getView(OCCViewer_ViewFrame::MAIN_VIEW);
      disconnect((OCCViewer_ViewWindow*)aWnd, SIGNAL(vpTransformationFinished(OCCViewer_ViewWindow::OperationType)),
        this, SLOT(onViewTransformed(OCCViewer_ViewWindow::OperationType)));
    }
  }
}

void NewGeom_SalomeViewer::Zfitall()
{
  if (!mySelector || !mySelector->viewer())
    return;
  SUIT_ViewManager* aMgr = mySelector->viewer()->getViewManager();
  OCCViewer_ViewFrame* aView = dynamic_cast<OCCViewer_ViewFrame*>(aMgr->getActiveView());
  if (aView) {
    OCCViewer_ViewWindow* aWnd = aView->getView(OCCViewer_ViewFrame::MAIN_VIEW);
    aWnd->getViewPort()->getView()->ZFitAll();
    aWnd->getViewPort()->getView()->DepthFitAll();
  }
}