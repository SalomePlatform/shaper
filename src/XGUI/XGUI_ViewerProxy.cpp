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

#include "XGUI_ViewerProxy.h"
#include "XGUI_Workshop.h"
#include "XGUI_SalomeConnector.h"
#include "XGUI_Displayer.h"

#ifndef HAVE_SALOME
  #include <AppElements_MainWindow.h>
  #include <AppElements_ViewPort.h>
  #include <AppElements_ViewWindow.h>
  #include <AppElements_Viewer.h>
#endif

#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_Preferences.h>
#include <GeomAPI_Shape.h>
#include <ModelAPI_ResultConstruction.h>

#include <Config_PropManager.h>

#include <SUIT_ResourceMgr.h>
#include <AIS_Shape.hxx>
#include <StdSelect_BRepOwner.hxx>

#include <QEvent>
#include <QKeyEvent>


#define HIGHLIGHT_COLOR Quantity_NOC_YELLOW

XGUI_ViewerProxy::XGUI_ViewerProxy(XGUI_Workshop* theParent)
    : ModuleBase_IViewer(theParent),
      myWorkshop(theParent), myShowHighlight(false)
{
}

void XGUI_ViewerProxy::connectViewProxy()
{
#ifdef HAVE_SALOME
  connect(myWorkshop->salomeConnector()->viewer(), SIGNAL(trihedronVisibilityChanged(bool)),
         SIGNAL(trihedronVisibilityChanged(bool)));
#else
  connect(myWorkshop->mainWindow()->viewer(), SIGNAL(trihedronVisibilityChanged(bool)),
         SIGNAL(trihedronVisibilityChanged(bool)));
#endif
}

Handle(AIS_InteractiveContext) XGUI_ViewerProxy::AISContext() const
{
  Handle(AIS_InteractiveContext) aContext;
#ifdef HAVE_SALOME
  aContext = myWorkshop->salomeConnector()->viewer()->AISContext();
#else
  aContext = myWorkshop->mainWindow()->viewer()->AISContext();
#endif
  return aContext;
}

Handle(AIS_Trihedron) XGUI_ViewerProxy::trihedron() const
{
#ifdef HAVE_SALOME
  return myWorkshop->salomeConnector()->viewer()->trihedron();
#else
  return myWorkshop->mainWindow()->viewer()->trihedron();
#endif
}

Handle(V3d_Viewer) XGUI_ViewerProxy::v3dViewer() const
{
#ifdef HAVE_SALOME
  return myWorkshop->salomeConnector()->viewer()->v3dViewer();
#else
  return myWorkshop->mainWindow()->viewer()->v3dViewer();
#endif
}

Handle(V3d_View) XGUI_ViewerProxy::activeView() const
{
#ifdef HAVE_SALOME
  return myWorkshop->salomeConnector()->viewer()->activeView();
#else
  AppElements_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
  return (aViewer->activeViewWindow()) ?
    aViewer->activeViewWindow()->viewPortApp()->getView() :
    Handle(V3d_View)();
#endif
}

QWidget* XGUI_ViewerProxy::activeViewPort() const
{
#ifdef HAVE_SALOME
  return myWorkshop->salomeConnector()->viewer()->activeViewPort();
#else
  AppElements_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
  return (aViewer->activeViewWindow()) ?
         aViewer->activeViewWindow()->viewPortApp(): 0;
#endif
}

void XGUI_ViewerProxy::setViewProjection(double theX, double theY, double theZ, double theTwist)
{
  Handle(V3d_View) aView3d = activeView();
  if (!aView3d.IsNull()) {
    aView3d->SetProj(theX, theY, theZ);
    aView3d->SetTwist( theTwist );
    aView3d->FitAll(0.01, false);
    aView3d->SetZSize(0.);
    if (aView3d->Depth() < 0.1)
      aView3d->DepthFitAll();
  }
}

void XGUI_ViewerProxy::fitAll()
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->fitAll();
#else
  AppElements_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
  if (aViewer->activeViewWindow())
    aViewer->activeViewWindow()->viewPortApp()->fitAll();
#endif
}

void XGUI_ViewerProxy::eraseAll()
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->eraseAll();
#else
#endif
}

void XGUI_ViewerProxy::connectToViewer()
{
#ifdef HAVE_SALOME
  ModuleBase_IViewer* aViewer = myWorkshop->salomeConnector()->viewer();

  connect(aViewer, SIGNAL(lastViewClosed()), this, SIGNAL(lastViewClosed()));
  connect(aViewer, SIGNAL(tryCloseView(ModuleBase_IViewWindow*)),
    this, SIGNAL(tryCloseView(ModuleBase_IViewWindow*)));

  connect(aViewer, SIGNAL(deleteView(ModuleBase_IViewWindow*)),
    this, SIGNAL(deleteView(ModuleBase_IViewWindow*)));

  connect(aViewer, SIGNAL(viewCreated(ModuleBase_IViewWindow*)),
    this, SLOT(onViewCreated(ModuleBase_IViewWindow*)));

  connect(aViewer, SIGNAL(activated(ModuleBase_IViewWindow*)),
    this, SIGNAL(activated(ModuleBase_IViewWindow*)));

  connect(aViewer, SIGNAL(mousePress(ModuleBase_IViewWindow*, QMouseEvent*)),
    this, SIGNAL(mousePress(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)),
    this, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)), this,
          SIGNAL(mouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)),
    this, SLOT(onMouseMove(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(keyPress(ModuleBase_IViewWindow*, QKeyEvent*)),
    this, SLOT(onKeyPress(ModuleBase_IViewWindow*, QKeyEvent*)));

  connect(aViewer, SIGNAL(keyRelease(ModuleBase_IViewWindow*, QKeyEvent*)),
    this, SLOT(onKeyRelease(ModuleBase_IViewWindow*, QKeyEvent*)));

  connect(aViewer, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));

  connect(aViewer, SIGNAL(viewTransformed(int)), this, SIGNAL(viewTransformed(int)));

  connect(aViewer, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SIGNAL(contextMenuRequested(QContextMenuEvent*)));

#else
  AppElements_Viewer* aViewer = myWorkshop->mainWindow()->viewer();

  connect(aViewer, SIGNAL(lastViewClosed()), this, SIGNAL(lastViewClosed()));

  connect(aViewer, SIGNAL(tryCloseView(AppElements_ViewWindow*)),
          this, SLOT(onTryCloseView(AppElements_ViewWindow*)));

  connect(aViewer, SIGNAL(deleteView(AppElements_ViewWindow*)),
          this, SLOT(onDeleteView(AppElements_ViewWindow*)));

  connect(aViewer, SIGNAL(viewCreated(AppElements_ViewWindow*)),
          this, SLOT(onViewCreated(AppElements_ViewWindow*)));

  connect(aViewer, SIGNAL(activated(AppElements_ViewWindow*)),
          this, SLOT(onActivated(AppElements_ViewWindow*)));

  connect(aViewer, SIGNAL(mousePress(AppElements_ViewWindow*, QMouseEvent*)), this,
          SLOT(onMousePress(AppElements_ViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseRelease(AppElements_ViewWindow*, QMouseEvent*)), this,
          SLOT(onMouseRelease(AppElements_ViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseDoubleClick(AppElements_ViewWindow*, QMouseEvent*)), this,
          SLOT(onMouseDoubleClick(AppElements_ViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseMove(AppElements_ViewWindow*, QMouseEvent*)), this,
          SLOT(onMouseMove(AppElements_ViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(keyPress(AppElements_ViewWindow*, QKeyEvent*)), this,
          SLOT(onKeyPress(AppElements_ViewWindow*, QKeyEvent*)));

  connect(aViewer, SIGNAL(keyRelease(AppElements_ViewWindow*, QKeyEvent*)), this,
          SLOT(onKeyRelease(AppElements_ViewWindow*, QKeyEvent*)));

  connect(aViewer, SIGNAL(selectionChanged()), this, SIGNAL(selectionChanged()));
  connect(aViewer, SIGNAL(contextMenuRequested(QContextMenuEvent*)), this,
          SIGNAL(contextMenuRequested(QContextMenuEvent*)));
#endif
}

bool XGUI_ViewerProxy::eventFilter(QObject *theObject, QEvent *theEvent)
{
  if (theEvent->type() == QEvent::Enter) {
    emit enterViewPort();
  }
  else if (theEvent->type() == QEvent::Leave) {
    emit leaveViewPort();
  }
  return ModuleBase_IViewer::eventFilter(theObject, theEvent);
}

void XGUI_ViewerProxy::onViewCreated(ModuleBase_IViewWindow* theWnd)
{
  theWnd->viewPort()->installEventFilter(this);
  myWindowScale.insert (theWnd->v3dView(), theWnd->v3dView()->Camera()->Scale());
  emit viewCreated(theWnd);
}

#ifndef HAVE_SALOME
void XGUI_ViewerProxy::onTryCloseView(AppElements_ViewWindow* theWnd)
{
  emit tryCloseView(theWnd);
}

void XGUI_ViewerProxy::onDeleteView(AppElements_ViewWindow* theWnd)
{
  if (myWindowScale.contains(theWnd->v3dView()))
    myWindowScale.remove (theWnd->v3dView());
  emit deleteView(theWnd);
}

void XGUI_ViewerProxy::onViewCreated(AppElements_ViewWindow* theWnd)
{
  theWnd->viewPort()->installEventFilter(this);

  connect(theWnd, SIGNAL(vpTransformationFinished(AppElements_ViewWindow::OperationType)),
    this, SLOT(onViewTransformed(AppElements_ViewWindow::OperationType)));

  myWindowScale.insert (theWnd->v3dView(), theWnd->v3dView()->Camera()->Scale());

  emit viewCreated(theWnd);
}

void XGUI_ViewerProxy::onActivated(AppElements_ViewWindow* theWnd)
{
  emit activated(theWnd);
}

void XGUI_ViewerProxy::onMousePress(AppElements_ViewWindow* theWnd, QMouseEvent* theEvent)
{
  emit mousePress(theWnd, theEvent);
}

void XGUI_ViewerProxy::onMouseRelease(AppElements_ViewWindow* theWnd, QMouseEvent* theEvent)
{
  emit mouseRelease(theWnd, theEvent);
}

void XGUI_ViewerProxy::onMouseDoubleClick(AppElements_ViewWindow* theWnd, QMouseEvent* theEvent)
{
  emit mouseDoubleClick(theWnd, theEvent);
}

void XGUI_ViewerProxy::onMouseMove(AppElements_ViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myIs2dMode) {
    bool aHighlight2d =
      ModuleBase_Preferences::resourceMgr()->booleanValue("Viewer", "highlighting-2d", true);
    if (aHighlight2d || myShowHighlight)
      updateHighlight();
    else
      eraseHighlight();
  }
  else {
    bool aHighlight3d =
      ModuleBase_Preferences::resourceMgr()->booleanValue("Viewer", "highlighting-3d", false);
    if (aHighlight3d || myShowHighlight)
      updateHighlight();
    else
      eraseHighlight();
  }
  emit mouseMove(theWnd, theEvent);
}

void XGUI_ViewerProxy::onKeyPress(AppElements_ViewWindow* theWnd, QKeyEvent* theEvent)
{
  myShowHighlight = theEvent->key() == Qt::Key_H;
  emit keyPress(theWnd, theEvent);
}

void XGUI_ViewerProxy::onKeyRelease(AppElements_ViewWindow* theWnd, QKeyEvent* theEvent)
{
  if (theEvent->key() == Qt::Key_H) {
    myShowHighlight = false;
  }
  emit keyRelease(theWnd, theEvent);
}

void XGUI_ViewerProxy::onViewTransformed(AppElements_ViewWindow::OperationType theType)
{
  emit viewTransformed((int) theType);
}

#endif


//***************************************
void XGUI_ViewerProxy::enableSelection(bool isEnabled)
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->enableSelection(isEnabled);
#else
  myWorkshop->mainWindow()->viewer()->setSelectionEnabled(isEnabled);
#endif
}

//***************************************
bool XGUI_ViewerProxy::isSelectionEnabled() const
{
#ifdef HAVE_SALOME
  return myWorkshop->salomeConnector()->viewer()->isSelectionEnabled();
#else
  return myWorkshop->mainWindow()->viewer()->isSelectionEnabled();
#endif
}

//***************************************
void XGUI_ViewerProxy::enableMultiselection(bool isEnable)
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->enableMultiselection(isEnable);
#else
  myWorkshop->mainWindow()->viewer()->setMultiSelectionEnabled(isEnable);
#endif
}

//***************************************
bool XGUI_ViewerProxy::isMultiSelectionEnabled() const
{
#ifdef HAVE_SALOME
  return myWorkshop->salomeConnector()->viewer()->isMultiSelectionEnabled();
#else
  return myWorkshop->mainWindow()->viewer()->isMultiSelectionEnabled();
#endif
}

//***************************************
bool XGUI_ViewerProxy::enableDrawMode(bool isEnabled)
{
#ifdef HAVE_SALOME
  return myWorkshop->salomeConnector()->viewer()->enableDrawMode(isEnabled);
#else
  return myWorkshop->mainWindow()->viewer()->enableDrawMode(isEnabled);
#endif
}

//***************************************
void XGUI_ViewerProxy::addSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  myWorkshop->displayer()->addSelectionFilter(theFilter);
}

//***************************************
void XGUI_ViewerProxy::removeSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  myWorkshop->displayer()->removeSelectionFilter(theFilter);
}

//***************************************
bool XGUI_ViewerProxy::hasSelectionFilter(const Handle(SelectMgr_Filter)& theFilter)
{
  return myWorkshop->displayer()->hasSelectionFilter(theFilter);
}

//***************************************
void XGUI_ViewerProxy::clearSelectionFilters()
{
  myWorkshop->displayer()->removeFilters();
}

//***************************************
void XGUI_ViewerProxy::update()
{
  myWorkshop->displayer()->updateViewer();
}

//***************************************
bool XGUI_ViewerProxy::canDragByMouse() const
{
  if (myWorkshop->isSalomeMode()) {
    ModuleBase_IViewer* aViewer = myWorkshop->salomeConnector()->viewer();
    return aViewer->canDragByMouse();
  } else {
    return true;
  }
}

//***************************************
void XGUI_ViewerProxy::displayHighlight(FeaturePtr theFeature, const TopoDS_Shape& theIgnoreShape)
{
  Handle(AIS_InteractiveContext) aContext = AISContext();

  double aDeflection;
  if (myResult->groupName() == ModelAPI_ResultConstruction::group()) {
    //FeaturePtr aFeature = ModelAPI_Feature::feature(myResult);
    if (theFeature.get()) {
      std::list<ResultPtr> aResults = theFeature->results();
      std::list<ResultPtr>::const_iterator aIt;
      ResultPtr aRes;
      Handle(AIS_Shape) aAis;
      for (aIt = aResults.cbegin(); aIt != aResults.cend(); aIt++) {
        aRes = (*aIt);
        TopoDS_Shape aTShape = aRes->shape()->impl<TopoDS_Shape>();
        if (!aTShape.IsSame(theIgnoreShape)) {
          aAis = new AIS_Shape(aTShape);
          aAis->SetColor(HIGHLIGHT_COLOR);
          aAis->SetZLayer(Graphic3d_ZLayerId_Top); //Graphic3d_ZLayerId_Topmost
          aDeflection = Config_PropManager::real("Visualization", "construction_deflection");
          aAis->Attributes()->SetDeviationCoefficient(aDeflection);
          myHighlights.Append(aAis);
          aContext->Display(aAis, false);
          aContext->Deactivate(aAis);
        }
      }
    }
  }
  else {
    TopoDS_Shape aTShape = myResult->shape()->impl<TopoDS_Shape>();
    Handle(AIS_Shape) aAis = new AIS_Shape(aTShape);
    aAis->SetColor(HIGHLIGHT_COLOR);
    aAis->SetZLayer(Graphic3d_ZLayerId_Top); //Graphic3d_ZLayerId_Topmost
    aDeflection = Config_PropManager::real("Visualization", "body_deflection");
    aAis->Attributes()->SetDeviationCoefficient(aDeflection);
    myHighlights.Append(aAis);
    aContext->Display(aAis, false);
    aContext->Deactivate(aAis);
  }
}

void XGUI_ViewerProxy::eraseHighlight()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  Handle(AIS_InteractiveObject) anAISIO;
  AIS_ListIteratorOfListOfInteractive aLIt;
  for (aLIt.Initialize(myHighlights); aLIt.More(); aLIt.Next()) {
    anAISIO = aLIt.Value();
    aContext->Remove(anAISIO, false);
  }
  myHighlights.Clear();
}

void XGUI_ViewerProxy::updateHighlight()
{
  Handle(AIS_InteractiveContext) aContext = AISContext();
  if (!aContext.IsNull()) {
    Handle(StdSelect_BRepOwner) aOwner;
    Handle(AIS_InteractiveObject) anIO;
    bool isDisplayed = false;
    TopoDS_Shape aShape, aShp;
    ResultPtr aRes;
    XGUI_Displayer* aDisplayer = myWorkshop->displayer();
    for (aContext->InitDetected(); aContext->MoreDetected(); aContext->NextDetected()) {
      aOwner = Handle(StdSelect_BRepOwner)::DownCast(aContext->DetectedOwner());
      if ((!aOwner.IsNull()) && aOwner->HasShape()) {
        aShape = aOwner->Shape();
        anIO = Handle(AIS_InteractiveObject)::DownCast(aOwner->Selectable());
        aRes = std::dynamic_pointer_cast<ModelAPI_Result>(aDisplayer->getObject(anIO));
        if (aRes.get() && (aRes != myResult)) {
          eraseHighlight();
          FeaturePtr aFeature = ModelAPI_Feature::feature(aRes);
          aShp = aRes->shape()->impl<TopoDS_Shape>();
          if ((aFeature->results().size() > 1) || (!aShp.IsSame(aShape))) {
            myResult = aRes;
            displayHighlight(aFeature, aShape);
          }
          else {
            myResult = ResultPtr();
          }
          aContext->UpdateCurrentViewer();
        }
        isDisplayed = aRes.get();
      }
    }
    if (!isDisplayed) {
      eraseHighlight();
      aContext->UpdateCurrentViewer();
      myResult = ResultPtr();
    }
  }
}

#ifdef HAVE_SALOME
void XGUI_ViewerProxy::onMouseMove(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myIs2dMode) {
    bool aHighlight2d =
      ModuleBase_Preferences::resourceMgr()->booleanValue("Viewer", "highlighting-2d", true);
    if (aHighlight2d || myShowHighlight)
      updateHighlight();
    else
      eraseHighlight();
  }
  else {
    bool aHighlight3d =
      ModuleBase_Preferences::resourceMgr()->booleanValue("Viewer", "highlighting-3d", false);
    if (aHighlight3d || myShowHighlight)
      updateHighlight();
    else
      eraseHighlight();
  }
  emit mouseMove(theWnd, theEvent);
}

void XGUI_ViewerProxy::onKeyPress(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent)
{
  myShowHighlight = theEvent->key() == Qt::Key_H;
  emit keyPress(theWnd, theEvent);
}

void XGUI_ViewerProxy::onKeyRelease(ModuleBase_IViewWindow* theWnd, QKeyEvent* theEvent)
{
  if (theEvent->key() == Qt::Key_H) {
    myShowHighlight = false;
  }
  emit keyRelease(theWnd, theEvent);
}
#endif


bool XGUI_ViewerProxy::isColorScaleVisible() const
{
#ifdef HAVE_SALOME
  return myWorkshop->salomeConnector()->viewer()->isColorScaleVisible();
#else
  return myWorkshop->mainWindow()->viewer()->isColorScaleVisible();
#endif
}

void XGUI_ViewerProxy::setColorScaleShown(bool on)
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->setColorScaleShown(on);
#else
  myWorkshop->mainWindow()->viewer()->setColorScaleShown(on);
#endif
}

void XGUI_ViewerProxy::setColorScalePosition(double theX, double theY)
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->setColorScalePosition(theX, theY);
#else
  QWidget* aWindow = activeViewPort();
  Handle(AIS_ColorScale) aColorScale = myWorkshop->mainWindow()->viewer()->colorScale();
  aColorScale->SetPosition(aWindow->width() * theX, aWindow->height() * theY);
#endif
}

void XGUI_ViewerProxy::setColorScaleSize(double theW, double theH)
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->setColorScaleSize(theW, theH);
#else
  QWidget* aWindow = activeViewPort();
  Handle(AIS_ColorScale) aColorScale = myWorkshop->mainWindow()->viewer()->colorScale();
  aColorScale->SetSize(aWindow->width() * theW, aWindow->height() * theH);
#endif
}

void XGUI_ViewerProxy::setColorScaleRange(double theMin, double theMax)
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->setColorScaleRange(theMin, theMax);
#else
  Handle(AIS_ColorScale) aColorScale = myWorkshop->mainWindow()->viewer()->colorScale();
  aColorScale->SetRange(theMin, theMax);
#endif
}

void XGUI_ViewerProxy::setColorScaleIntervals(int theNb)
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->setColorScaleIntervals(theNb);
#else
  Handle(AIS_ColorScale) aColorScale = myWorkshop->mainWindow()->viewer()->colorScale();
  aColorScale->SetNumberOfIntervals(theNb);
#endif
}

void XGUI_ViewerProxy::setColorScaleTextHeigth(int theH)
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->setColorScaleTextHeigth(theH);
#else
  Handle(AIS_ColorScale) aColorScale = myWorkshop->mainWindow()->viewer()->colorScale();
  aColorScale->SetTextHeight(theH);
#endif
}

void XGUI_ViewerProxy::setColorScaleTitle(const QString& theText)
{
#ifdef HAVE_SALOME
  myWorkshop->salomeConnector()->viewer()->setColorScaleTitle(theText);
#else
  Handle(AIS_ColorScale) aColorScale = myWorkshop->mainWindow()->viewer()->colorScale();
  aColorScale->SetTitle(theText.toStdString().c_str());
#endif
}


//******************************************************
void XGUI_ViewerProxy::setupColorScale()
{
  setColorScalePosition(0.03, 0.35);
  setColorScaleSize(0.2, 0.5);
  setColorScaleTextHeigth(14);
  setColorScaleIntervals(20);
}


//***************************************
//void XGUI_ViewerProxy::Zfitall()
//{
//#ifdef HAVE_SALOME
//  myWorkshop->salomeConnector()->viewer()->Zfitall();
//#else
//  AppElements_Viewer* aViewer = myWorkshop->mainWindow()->viewer();
//  AppElements_ViewWindow* aView = aViewer->activeViewWindow();
//  if (aView) {
//    Handle(V3d_View) aView3d = aView->v3dView();
//    aView3d->ZFitAll();
//    if (aView3d->Depth() < 0.1)
//      aView3d->DepthFitAll();
//  }
//#endif
//}