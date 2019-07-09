// Copyright (C) 2014-2019  CEA/DEN, EDF R&D
//
// This library is free software; you can redistribute it and/or
// modify it under the terms of the GNU Lesser General Public
// License as published by the Free Software Foundation; either
// version 2.1 of the License, or (at your option) any later version.
//
// This library is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
// Lesser General Public License for more details.
//
// You should have received a copy of the GNU Lesser General Public
// License along with this library; if not, write to the Free Software
// Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307 USA
//
// See http://www.salome-platform.org/ or email : webmaster.salome@opencascade.com
//

#include "SHAPERGUI_SalomeViewer.h"
#include "SHAPERGUI_OCCSelector.h"

#include <OCCViewer_ViewPort3d.h>
#include <OCCViewer_ViewFrame.h>
#include <SOCC_ViewModel.h>
#include <SUIT_ViewManager.h>

#include <QtxActionToolMgr.h>
#include <SALOME_AISShape.hxx>

#include <SelectMgr_ListIteratorOfListOfFilter.hxx>

#include <QMouseEvent>
#include <QContextMenuEvent>

#define SALOME_PATCH_FOR_CTRL_WHEEL

SHAPERGUI_SalomeView::SHAPERGUI_SalomeView(OCCViewer_Viewer* theViewer)
: ModuleBase_IViewWindow(), myCurrentView(0)
{
  myViewer = theViewer;
}


Handle(V3d_View) SHAPERGUI_SalomeView::v3dView() const
{
  Handle(V3d_View) aView;
  if (myCurrentView) {
    OCCViewer_ViewWindow* aWnd = static_cast<OCCViewer_ViewWindow*>(myCurrentView);
    aView = aWnd->getViewPort()->getView();
  }
  return aView;
}

QWidget* SHAPERGUI_SalomeView::viewPort() const
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



SHAPERGUI_SalomeViewer::SHAPERGUI_SalomeViewer(QObject* theParent)
    : ModuleBase_IViewer(theParent),
      mySelector(0), myView(0), myIsSelectionChanged(false)
{
}

SHAPERGUI_SalomeViewer::~SHAPERGUI_SalomeViewer()
{
  if (myView)
    delete myView;
}


//**********************************************
Handle(AIS_InteractiveContext) SHAPERGUI_SalomeViewer::AISContext() const
{
  if (mySelector && mySelector->viewer())
    return mySelector->viewer()->getAISContext();
  Handle(AIS_InteractiveContext) aNull;
  return aNull;
}

//**********************************************
Handle(V3d_Viewer) SHAPERGUI_SalomeViewer::v3dViewer() const
{
  if (mySelector)
    return mySelector->viewer()->getViewer3d();
  return Handle(V3d_Viewer)();
}

//**********************************************
Handle(AIS_Trihedron) SHAPERGUI_SalomeViewer::trihedron() const
{
  return mySelector->viewer()->getTrihedron();
}

//**********************************************
Handle(V3d_View) SHAPERGUI_SalomeViewer::activeView() const
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
QWidget* SHAPERGUI_SalomeViewer::activeViewPort() const
{
  QWidget* aViewPort = 0;
  if (mySelector) {
    OCCViewer_Viewer* aViewer = mySelector->viewer();
    SUIT_ViewManager* aMgr = aViewer->getViewManager();
    OCCViewer_ViewWindow* aWnd = static_cast<OCCViewer_ViewWindow*>(aMgr->getActiveView());
    aViewPort = aWnd->getViewPort();
  }
  return aViewPort;
}

//**********************************************
void SHAPERGUI_SalomeViewer::setSelector(SHAPERGUI_OCCSelector* theSel)
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

  myView = new SHAPERGUI_SalomeView(mySelector->viewer());

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
void SHAPERGUI_SalomeViewer::onSelectionChanged()
{
  // Selection event must be sent only after mouse release
  myIsSelectionChanged = true;
}

//**********************************************
void SHAPERGUI_SalomeViewer::onMousePress(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  myView->setCurrentView(theView);
  emit mousePress(myView, theEvent);
}

//**********************************************
void SHAPERGUI_SalomeViewer::onMouseRelease(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  myView->setCurrentView(theView);
  emit mouseRelease(myView, theEvent);
  if (myIsSelectionChanged) {
    emit selectionChanged();
    myIsSelectionChanged = false;
  }
}

//**********************************************
void SHAPERGUI_SalomeViewer::onMouseDoubleClick(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  myView->setCurrentView(theView);
  emit mouseDoubleClick(myView, theEvent);
}

//**********************************************
void SHAPERGUI_SalomeViewer::onMouseMove(SUIT_ViewWindow* theView, QMouseEvent* theEvent)
{
  myView->setCurrentView(theView);
  emit mouseMove(myView, theEvent);
}

//**********************************************
bool SHAPERGUI_SalomeViewer::canDragByMouse() const
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
void SHAPERGUI_SalomeViewer::onKeyPress(SUIT_ViewWindow* theView, QKeyEvent* theEvent)
{
  emit keyPress(myView, theEvent);
}

//**********************************************
void SHAPERGUI_SalomeViewer::onKeyRelease(SUIT_ViewWindow* theView, QKeyEvent* theEvent)
{
  emit keyRelease(myView, theEvent);
}

//**********************************************
void SHAPERGUI_SalomeViewer::onTryCloseView(SUIT_ViewWindow*)
{
  emit tryCloseView(myView);
}

//**********************************************
void SHAPERGUI_SalomeViewer::onDeleteView(SUIT_ViewWindow*)
{
  if(myWindowScale.contains(myView->v3dView()))
    myWindowScale.remove(myView->v3dView());
  emit deleteView(myView);
}

//**********************************************
void SHAPERGUI_SalomeViewer::onViewCreated(SUIT_ViewWindow* theView)
{
  myView->setCurrentView(theView);

  OCCViewer_ViewFrame* aView = dynamic_cast<OCCViewer_ViewFrame*>(theView);

  OCCViewer_ViewWindow* aWnd = aView->getView(OCCViewer_ViewFrame::MAIN_VIEW);
  if (aWnd) {
    connect(aWnd, SIGNAL(vpTransformationFinished(OCCViewer_ViewWindow::OperationType)),
      this, SLOT(onViewTransformed(OCCViewer_ViewWindow::OperationType)));
    OCCViewer_ViewPort3d* aViewPort = aWnd->getViewPort();
    if (aViewPort)
      connect(aViewPort, SIGNAL(vpMapped(OCCViewer_ViewPort3d*)), this, SLOT(onViewPortMapped()));
  }
  reconnectActions(aWnd, true);

  myWindowScale.insert(aView->getViewPort()->getView(),
                       aView->getViewPort()->getView()->Camera()->Scale());

  emit viewCreated(myView);


}

//**********************************************
void SHAPERGUI_SalomeViewer::onActivated(SUIT_ViewManager* theMgr)
{
  myView->setCurrentView(theMgr->getActiveView());
  emit activated(myView);
}

//**********************************************
void SHAPERGUI_SalomeViewer::enableSelection(bool isEnabled)
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
bool SHAPERGUI_SalomeViewer::isSelectionEnabled() const
{
  if (mySelector)
    return mySelector->viewer()->isSelectionEnabled();
  return false;
}

//**********************************************
void SHAPERGUI_SalomeViewer::enableMultiselection(bool isEnable)
{
  if (mySelector)
    mySelector->viewer()->enableMultiselection(isEnable);
}

//**********************************************
bool SHAPERGUI_SalomeViewer::isMultiSelectionEnabled() const
{
  if (mySelector)
    return mySelector->viewer()->isMultiSelectionEnabled();
  return false;
}

//**********************************************
bool SHAPERGUI_SalomeViewer::enableDrawMode(bool isEnabled)
{
  // TODO: Has to be replaced when SALOME patch become available
  if (mySelector)
    return mySelector->viewer()->enableDrawMode(isEnabled);
  return false;
}

//**********************************************
void SHAPERGUI_SalomeViewer::reconnectActions(SUIT_ViewWindow* theWindow,
                                            const bool theUseSHAPERSlot)
{
  OCCViewer_ViewWindow* aWindow = dynamic_cast<OCCViewer_ViewWindow*>(theWindow);
  if (!aWindow)
    return;

  QAction* anAction = theWindow->toolMgr()->action(OCCViewer_ViewWindow::TrihedronShowId);
  if (!anAction)
    return;

  if (theUseSHAPERSlot) {
    anAction->disconnect(anAction, SIGNAL(toggled(bool)),
                         theWindow, SLOT(onTrihedronShow(bool)));
    anAction->connect(anAction, SIGNAL(toggled(bool)),
                      this, SIGNAL(trihedronVisibilityChanged(bool)));
  }
  else {
    anAction->connect(anAction, SIGNAL(toggled(bool)),
                      theWindow, SLOT(onTrihedronShow(bool)));
    anAction->disconnect(anAction, SIGNAL(toggled(bool)),
                         this, SIGNAL(trihedronVisibilityChanged(bool)));
  }
}

//**********************************************
void SHAPERGUI_SalomeViewer::fitAll()
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
void SHAPERGUI_SalomeViewer::eraseAll()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (aContext.IsNull())
    return;
  AIS_ListOfInteractive aList;
  aContext->DisplayedObjects(aList);
  AIS_ListIteratorOfListOfInteractive aLIt;
  Handle(AIS_InteractiveObject) anAISIO;
  for (aLIt.Initialize(aList); aLIt.More(); aLIt.Next()) {
    anAISIO = aLIt.Value();
    Handle(Standard_Type) aType = anAISIO->DynamicType();
    if (anAISIO->IsKind(STANDARD_TYPE(SALOME_AISShape))) {
      aContext->Erase(anAISIO, false);
    }
  }
}

//**********************************************
void SHAPERGUI_SalomeViewer::setViewProjection(double theX, double theY,
                                               double theZ, double theTwist)
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
      aView3d->FitAll(0.01, false);
      aView3d->SetZSize(0.);
      if (aView3d->Depth() < 0.1)
        aView3d->DepthFitAll();
    }
  }
}

//***************************************
void SHAPERGUI_SalomeViewer::addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->AddFilter(theFilter);
  }
}

//***************************************
void SHAPERGUI_SalomeViewer::removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->RemoveFilter(theFilter);
  }
}

//***************************************
bool SHAPERGUI_SalomeViewer::hasSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  bool aFoundFilter = false;
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    const SelectMgr_ListOfFilter& aFilters = aContext->Filters();
    SelectMgr_ListIteratorOfListOfFilter aIt(aFilters);
    for (; aIt.More() && !aFoundFilter; aIt.Next()) {
      aFoundFilter = theFilter.get() == aIt.Value().get();
    }
  }
  return aFoundFilter;
}

//***************************************
void SHAPERGUI_SalomeViewer::clearSelectionFilters()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->RemoveFilters();
  }
}

//***************************************
void SHAPERGUI_SalomeViewer::update()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    aContext->UpdateCurrentViewer();
  }
}

//***************************************
void SHAPERGUI_SalomeViewer::onViewTransformed(OCCViewer_ViewWindow::OperationType theType)
{
  emit viewTransformed((int) theType);
}

//***************************************
void SHAPERGUI_SalomeViewer::onViewPortMapped()
{
  emit trihedronVisibilityChanged(true);
}

//***************************************
void SHAPERGUI_SalomeViewer::activateViewer(bool toActivate)
{
  if (!mySelector || !mySelector->viewer())
    return;
  SUIT_ViewManager* aMgr = mySelector->viewer()->getViewManager();
#ifdef SALOME_PATCH_FOR_CTRL_WHEEL
  OCCViewer_Viewer* aViewer = dynamic_cast<OCCViewer_Viewer*>(aMgr->getViewModel());
  if (aViewer)
    aViewer->setUseLocalSelection(toActivate);
#endif
  QVector<SUIT_ViewWindow*> aViews = aMgr->getViews();
  if (toActivate) {
    foreach (SUIT_ViewWindow* aView, aViews) {
      OCCViewer_ViewFrame* aOCCView = dynamic_cast<OCCViewer_ViewFrame*>(aView);
      OCCViewer_ViewWindow* aWnd = aOCCView->getView(OCCViewer_ViewFrame::MAIN_VIEW);
      connect(aWnd, SIGNAL(vpTransformationFinished(OCCViewer_ViewWindow::OperationType)),
        this, SLOT(onViewTransformed(OCCViewer_ViewWindow::OperationType)));
      reconnectActions(aWnd, true);
    }
  } else {
    foreach (SUIT_ViewWindow* aView, aViews) {
      OCCViewer_ViewFrame* aOCCView = dynamic_cast<OCCViewer_ViewFrame*>(aView);
      OCCViewer_ViewWindow* aWnd = aOCCView->getView(OCCViewer_ViewFrame::MAIN_VIEW);
      disconnect((OCCViewer_ViewWindow*)aWnd,
                 SIGNAL(vpTransformationFinished(OCCViewer_ViewWindow::OperationType)),
        this, SLOT(onViewTransformed(OCCViewer_ViewWindow::OperationType)));
      reconnectActions(aWnd, false);
    }
  }
}

bool SHAPERGUI_SalomeViewer::isColorScaleVisible() const
{
  if (mySelector) {
    return mySelector->viewer()->isColorScaleVisible();
  }
  return false;
}

void SHAPERGUI_SalomeViewer::setColorScaleShown(bool on)
{
  if (mySelector) {
    mySelector->viewer()->setColorScaleShown(on);
  }
}

void SHAPERGUI_SalomeViewer::setColorScalePosition(double theX, double theY)
{
  if (mySelector) {
    QWidget* aWindow = activeViewPort();
    mySelector->viewer()->getColorScale()->SetPosition(aWindow->width() * theX,
      aWindow->height() * theY);
  }
}

void SHAPERGUI_SalomeViewer::setColorScaleSize(double theW, double theH)
{
  if (mySelector) {
    QWidget* aWindow = activeViewPort();
    mySelector->viewer()->getColorScale()->SetSize(aWindow->width() * theW,
      aWindow->height() * theH);
  }
}

void SHAPERGUI_SalomeViewer::setColorScaleRange(double theMin, double theMax)
{
  if (mySelector) {
    mySelector->viewer()->getColorScale()->SetRange(theMin, theMax);
  }
}

void SHAPERGUI_SalomeViewer::setColorScaleIntervals(int theNb)
{
  if (mySelector) {
    mySelector->viewer()->getColorScale()->SetNumberOfIntervals(theNb);
  }
}

void SHAPERGUI_SalomeViewer::setColorScaleTextHeigth(int theH)
{
  if (mySelector) {
    mySelector->viewer()->getColorScale()->SetTextHeight(theH);
  }
}

void SHAPERGUI_SalomeViewer::setColorScaleTextColor(const QColor& theColor)
{
  if (mySelector) {
    Quantity_Color aColor(theColor.redF(), theColor.greenF(), theColor.blueF(), Quantity_TOC_RGB);
    mySelector->viewer()->getColorScale()->SetColor(aColor);
  }
}

void SHAPERGUI_SalomeViewer::setColorScaleTitle(const QString& theText)
{
  if (mySelector) {
    mySelector->viewer()->getColorScale()->SetTitle(theText.toStdString().c_str());
  }
}



//void SHAPERGUI_SalomeViewer::Zfitall()
//{
//  if (!mySelector || !mySelector->viewer())
//    return;
//  SUIT_ViewManager* aMgr = mySelector->viewer()->getViewManager();
//  /// WORKAROUND for issue #1798. SUIT_ViewManager::closeAllViews() should nullify myActiveView
//  /// As a result, we need to check views count in manager
//  if (aMgr->getViews().size() > 0) {
//    OCCViewer_ViewFrame* aView = dynamic_cast<OCCViewer_ViewFrame*>(aMgr->getActiveView());
//    if (aView) {
//      OCCViewer_ViewWindow* aWnd = aView->getView(OCCViewer_ViewFrame::MAIN_VIEW);
//      Handle(V3d_View) aView3d = aWnd->getViewPort()->getView();
//      aView3d->ZFitAll();
//      if (aView3d->Depth() < 0.1)
//        aView3d->DepthFitAll();
//    }
//  }
//}