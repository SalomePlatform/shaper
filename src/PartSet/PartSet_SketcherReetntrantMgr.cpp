// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "PartSet_SketcherReetntrantMgr.h"
#include "PartSet_Module.h"
#include "PartSet_SketcherMgr.h"
#include "PartSet_WidgetPoint2D.h"

#include "ModelAPI_Session.h"

#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_WidgetSelector.h>

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Line.h>

#include <XGUI_Workshop.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_OperationMgr.h>

PartSet_SketcherReetntrantMgr::PartSet_SketcherReetntrantMgr(ModuleBase_IWorkshop* theWorkshop)
: QObject(theWorkshop),
  myWorkshop(theWorkshop),
  myRestartingMode(RM_None),
  myIsFlagsBlocked(false),
  myIsInternalEditOperation(false)
{
}

PartSet_SketcherReetntrantMgr::~PartSet_SketcherReetntrantMgr()
{
}

ModuleBase_ModelWidget* PartSet_SketcherReetntrantMgr::internalActiveWidget() const
{
  ModuleBase_ModelWidget* aWidget = 0;
  if (!isActiveMgr())
    return aWidget;

  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (aOperation) {
    ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
    ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
    if (myIsInternalEditOperation && (!anActiveWidget || !anActiveWidget->isViewerSelector())) {
      // finds the first widget which can accept a value
      ModuleBase_ModelWidget* aFirstWidget = aPanel->findFirstAcceptingValueWidget();
      if (aFirstWidget)
        aWidget = aFirstWidget;
    }
  }
  return aWidget;
}

bool PartSet_SketcherReetntrantMgr::operationCommitted(ModuleBase_Operation* theOperation)
{
  bool aProcessed = false;
  if (!isActiveMgr())
    return aProcessed;

  aProcessed = myIsInternalEditOperation;
  resetFlags();

  return aProcessed;
}

void PartSet_SketcherReetntrantMgr::operationStarted(ModuleBase_Operation* theOperation)
{
  if (!isActiveMgr())
    return;

  resetFlags();
}

void PartSet_SketcherReetntrantMgr::operationAborted(ModuleBase_Operation* theOperation)
{
  if (!isActiveMgr())
    return;

  resetFlags();
}

bool PartSet_SketcherReetntrantMgr::processMouseMoved(ModuleBase_IViewWindow* /* theWnd*/,
                                                      QMouseEvent* /* theEvent*/)
{
  bool aProcessed = false;
  if (!isActiveMgr())
    return aProcessed;

  if  (myIsInternalEditOperation) {
    PartSet_WidgetPoint2D* aPoint2DWdg = dynamic_cast<PartSet_WidgetPoint2D*>(module()->activeWidget());
    if (aPoint2DWdg && aPoint2DWdg->canBeActivatedByMove()) {
      ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                         (myWorkshop->currentOperation());
      FeaturePtr aLastFeature = myRestartingMode == RM_LastFeatureUsed ? aFOperation->feature() : FeaturePtr();
      restartOperation();
      aProcessed = true;

      if (aLastFeature) {
        ModuleBase_IPropertyPanel* aPanel = myWorkshop->currentOperation()->propertyPanel();
        PartSet_WidgetPoint2D* aPoint2DWdg = dynamic_cast<PartSet_WidgetPoint2D*>(aPanel->activeWidget());
        if (aPoint2DWdg && aPoint2DWdg->canBeActivatedByMove()) {
          QList<ModuleBase_ViewerPrs> aSelection;
          aSelection.append(ModuleBase_ViewerPrs(aLastFeature, TopoDS_Shape(), NULL));
          if (aPoint2DWdg->setSelection(aSelection, true))
            aPanel->activateNextWidget(aPoint2DWdg);
        }
      }
    }
  }
  return aProcessed;
}

bool PartSet_SketcherReetntrantMgr::processMousePressed(ModuleBase_IViewWindow* /* theWnd*/,
                                                        QMouseEvent* /* theEvent*/)
{
  return isActiveMgr() && myIsInternalEditOperation;
}

bool PartSet_SketcherReetntrantMgr::processMouseReleased(ModuleBase_IViewWindow* theWnd,
                                                         QMouseEvent* theEvent)
{
  bool aProcessed = false;
  if (!isActiveMgr())
    return aProcessed;

  if (myIsInternalEditOperation) {
    ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
    ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();

    ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
    if (!anActiveWidget || !anActiveWidget->isViewerSelector()) {
      restartOperation();
      aProcessed = true;

      // fill the first widget by the mouse event point
      // if the active widget is not the first, it means that the restarted operation is filled by
      // the current preselection.
      PartSet_WidgetPoint2D* aPoint2DWdg = dynamic_cast<PartSet_WidgetPoint2D*>(module()->activeWidget());
      ModuleBase_ModelWidget* aFirstWidget = aPanel->findFirstAcceptingValueWidget();
      if (aPoint2DWdg && aPoint2DWdg == aFirstWidget) {
        aPoint2DWdg->onMouseRelease(theWnd, theEvent);
      }
    }
  }

  return aProcessed;
}

void PartSet_SketcherReetntrantMgr::onWidgetActivated()
{
  if (!isActiveMgr())
    return;
  if (!myIsInternalEditOperation)
    return;

  PartSet_Module* aModule = module();
  ModuleBase_ModelWidget* aFirstWidget = aModule->activeWidget();
  ModuleBase_IPropertyPanel* aPanel = aModule->currentOperation()->propertyPanel();
  if (aFirstWidget != aPanel->activeWidget()) {
    ModuleBase_WidgetSelector* aWSelector = dynamic_cast<ModuleBase_WidgetSelector*>(aFirstWidget);
    if (aWSelector)
      aWSelector->activateSelectionAndFilters(true);
  }
}

void PartSet_SketcherReetntrantMgr::onNoMoreWidgets(const std::string& thePreviousAttributeID)
{
  if (!isActiveMgr())
    return;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                       (myWorkshop->currentOperation());
  if (!myWorkshop->module()->getFeatureError(aFOperation->feature(), false).isEmpty())
    return;

  if (aFOperation) {
    if (PartSet_SketcherMgr::isNestedSketchOperation(aFOperation)) {
      if (myRestartingMode != RM_Forbided) {
        myRestartingMode = RM_LastFeatureUsed;
        startInternalEdit(thePreviousAttributeID);
      }
      else {
        aFOperation->commit();
      }
    }
  }
}

bool PartSet_SketcherReetntrantMgr::processEnter(const std::string& thePreviousAttributeID)
{
  bool isDone = false;

  if (!isActiveMgr())
    return isDone;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                       (myWorkshop->currentOperation());
  if (!myWorkshop->module()->getFeatureError(aFOperation->feature(), false).isEmpty())
    return isDone;

  myRestartingMode = RM_EmptyFeatureUsed;
  startInternalEdit(thePreviousAttributeID);
  isDone = true;

  return isDone;
}

void PartSet_SketcherReetntrantMgr::onVertexSelected()
{
  if (!isActiveMgr())
    return;

  ModuleBase_Operation* aOperation = myWorkshop->currentOperation();
  if (aOperation->id().toStdString() == SketchPlugin_Line::ID()) {
    /// If last line finished on vertex the lines creation sequence has to be break
    ModuleBase_IPropertyPanel* aPanel = aOperation->propertyPanel();
    ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
    const QList<ModuleBase_ModelWidget*>& aWidgets = aPanel->modelWidgets();
    QList<ModuleBase_ModelWidget*>::const_iterator anIt = aWidgets.begin(), aLast = aWidgets.end();
    bool aFoundWidget = false;
    bool aFoundObligatory = false;
    for (; anIt != aLast && !aFoundObligatory; anIt++) {
      if (!aFoundWidget)
        aFoundWidget = *anIt == anActiveWidget;
      else
        aFoundObligatory = (*anIt)->isObligatory();
    }
    if (!aFoundObligatory)
      myRestartingMode = RM_Forbided;
  }
}

void PartSet_SketcherReetntrantMgr::onBeforeStopped()
{
  if (!isActiveMgr() || !myIsInternalEditOperation)
    return;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                      (myWorkshop->currentOperation());
  if (aFOperation) {
    disconnect(aFOperation, SIGNAL(beforeCommitted()), this, SLOT(onBeforeStopped()));
    disconnect(aFOperation, SIGNAL(beforeAborted()), this, SLOT(onBeforeStopped()));
  }

  PartSet_Module* aModule = module();
  ModuleBase_ModelWidget* aFirstWidget = aModule->activeWidget();
  ModuleBase_IPropertyPanel* aPanel = aModule->currentOperation()->propertyPanel();
  if (aFirstWidget != aPanel->activeWidget()) {
    ModuleBase_WidgetSelector* aWSelector = dynamic_cast<ModuleBase_WidgetSelector*>(aFirstWidget);
    if (aWSelector)
      aWSelector->activateSelectionAndFilters(false);
  }
}

bool PartSet_SketcherReetntrantMgr::canBeCommittedByPreselection()
{
  return !isActiveMgr() || myRestartingMode == RM_None;
}

bool PartSet_SketcherReetntrantMgr::isActiveMgr() const
{
  ModuleBase_Operation* aCurrentOperation = myWorkshop->currentOperation();

  bool anActive = PartSet_SketcherMgr::isSketchOperation(aCurrentOperation);
  if (!anActive) {
    anActive = PartSet_SketcherMgr::isNestedSketchOperation(aCurrentOperation);
    if (anActive) { // the manager is not active when the current operation is a usual Edit
      ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                       (myWorkshop->currentOperation());
      if (aFOperation->isEditOperation())
        anActive = myIsInternalEditOperation;
    }
  }
  return anActive;
}

void PartSet_SketcherReetntrantMgr::startInternalEdit(const std::string& thePreviousAttributeID)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                     (myWorkshop->currentOperation());

  aFOperation->setEditOperation();
  FeaturePtr anOperationFeature = aFOperation->feature();
  if (anOperationFeature.get() != NULL) {

    myIsInternalEditOperation = true;
    connect(aFOperation, SIGNAL(beforeCommitted()), this, SLOT(onBeforeStopped()));
    connect(aFOperation, SIGNAL(beforeAborted()), this, SLOT(onBeforeStopped()));

    // activate selection filters of the first widget in the viewer
    onWidgetActivated();

    // activate the last active widget in the Property Panel
    if (!thePreviousAttributeID.empty()) {
      ModuleBase_Operation* anEditOperation = module()->currentOperation();
      if (anEditOperation) {
        ModuleBase_IPropertyPanel* aPanel = aFOperation->propertyPanel();
        ModuleBase_ModelWidget* aPreviousAttributeWidget = 0;
        QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
        for (int i = 0, aNb = aWidgets.size(); i < aNb && !aPreviousAttributeWidget; i++) {
          if (aWidgets[i]->attributeID() == thePreviousAttributeID)
            aPreviousAttributeWidget = aWidgets[i];
        }
        // If the current widget is a selector, do nothing, it processes the mouse press
        if (aPreviousAttributeWidget && !aPreviousAttributeWidget->isViewerSelector())
          aPreviousAttributeWidget->focusTo();
      }
    }
  }
}

void PartSet_SketcherReetntrantMgr::restartOperation()
{
  if (myIsInternalEditOperation) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(
                                                                  myWorkshop->currentOperation());
    if (aFOperation) {
      myIsFlagsBlocked = true;
      aFOperation->commit();
      module()->launchOperation(aFOperation->id());
      myIsFlagsBlocked = false;
      resetFlags();
    }
  }
}

void PartSet_SketcherReetntrantMgr::resetFlags()
{
  if (!myIsFlagsBlocked) {
    myIsInternalEditOperation = false;
    myRestartingMode = RM_None;
  }
}

XGUI_Workshop* PartSet_SketcherReetntrantMgr::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}

PartSet_Module* PartSet_SketcherReetntrantMgr::module() const
{
  return dynamic_cast<PartSet_Module*>(myWorkshop->module());
}

