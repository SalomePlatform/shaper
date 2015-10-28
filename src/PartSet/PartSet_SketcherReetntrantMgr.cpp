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
  myIsInternalEditOperation(false),
  myLastOperationId(""),
  myPreviousAttributeID(""),
  myRestartingMode(RM_None)
{
}

PartSet_SketcherReetntrantMgr::~PartSet_SketcherReetntrantMgr()
{
}

ModuleBase_ModelWidget* PartSet_SketcherReetntrantMgr::activeWidget() const
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
      QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
      ModuleBase_ModelWidget* aFirstWidget = 0;
      ModuleBase_ModelWidget* aWgt;
      QList<ModuleBase_ModelWidget*>::const_iterator aWIt;
      for (aWIt = aWidgets.begin(); aWIt != aWidgets.end() && !aFirstWidget; ++aWIt) {
        aWgt = (*aWIt);
        if (aWgt->canSetValue())
          aFirstWidget = aWgt;
      }
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

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (!aFOperation)
    return aProcessed;

  FeaturePtr aFeature = aFOperation->feature();
  std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
            std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
  if (aSPFeature && (myRestartingMode == RM_LastFeatureUsed ||
                     myRestartingMode == RM_EmptyFeatureUsed)) {
    myLastOperationId = aFOperation->id().toStdString();
    myLastFeature = myRestartingMode == RM_LastFeatureUsed ? aFOperation->feature() : FeaturePtr();
    PartSet_Module* aModule = module();
    if (!aModule->sketchMgr()->sketchSolverError()) {
      if (!aFOperation->isEditOperation()) {
        FeaturePtr anOperationFeature = aFOperation->feature();
        if (anOperationFeature.get() != NULL) {
          aModule->editFeature(anOperationFeature);
          aProcessed = true;

          myIsInternalEditOperation = true;
          // activate selection filters of the first widget in the viewer
          onInternalActivateFirstWidgetSelection();

          // activate the last active widget in the Property Panel
          if (!myPreviousAttributeID.empty()) {
            ModuleBase_Operation* anEditOperation = aModule->currentOperation();
            if (anEditOperation) {
              ModuleBase_IPropertyPanel* aPanel = aFOperation->propertyPanel();
              ModuleBase_ModelWidget* aPreviousAttributeWidget = 0;
              QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
              for (int i = 0, aNb = aWidgets.size(); i < aNb && !aPreviousAttributeWidget; i++) {
                if (aWidgets[i]->attributeID() == myPreviousAttributeID)
                  aPreviousAttributeWidget = aWidgets[i];
              }
              // If the current widget is a selector, do nothing, it processes the mouse press
              if (aPreviousAttributeWidget && !aPreviousAttributeWidget->isViewerSelector())
                aPreviousAttributeWidget->focusTo();
            }
          }
        }
      }
      else {
        // the flag should be reset before start to do not react to the widget deactivate
        myIsInternalEditOperation = false;
        aModule->launchOperation(myLastOperationId.c_str());
        breakOperationSequence();
        aProcessed = true;
      }
    }
  }

  if (!aProcessed)
    breakOperationSequence();

  return aProcessed;
}

void PartSet_SketcherReetntrantMgr::operationAborted(ModuleBase_Operation* theOperation)
{
  if (!isActiveMgr())
    return;

  if (myIsInternalEditOperation) {
    // abort the created feature, which is currently edited
    SessionPtr aMgr = ModelAPI_Session::get();
    if (aMgr->hasModuleDocument() && aMgr->canUndo()) {
      aMgr->undo();
    }
  }
  myIsInternalEditOperation = false;
  breakOperationSequence();
}

bool PartSet_SketcherReetntrantMgr::processMouseMoved()
{
  bool aProcessed = false;
  if (!isActiveMgr())
    return aProcessed;

  if  (myIsInternalEditOperation) {
    PartSet_WidgetPoint2D* aPoint2DWdg = dynamic_cast<PartSet_WidgetPoint2D*>(module()->activeWidget());
    if (aPoint2DWdg && aPoint2DWdg->canBeActivatedByMove()) {
    ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
      //if (operationMgr()->isApplyEnabled())
      anOperation->commit();
      aProcessed = true;
    }
  }
  return aProcessed;
}

bool PartSet_SketcherReetntrantMgr::processMousePressed()
{
  return isActiveMgr() && myIsInternalEditOperation;
}

bool PartSet_SketcherReetntrantMgr::processMouseReleased()
{
  bool aProcessed = false;
  if (!isActiveMgr())
    return aProcessed;

  if (myIsInternalEditOperation) {
    ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
    //if (operationMgr()->isApplyEnabled())
    anOperation->commit();
    aProcessed = true;
  }
  return aProcessed;
}

void PartSet_SketcherReetntrantMgr::propertyPanelDefined(ModuleBase_Operation* theOperation)
{
  if (!isActiveMgr())
    return;
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(theOperation);
  if (!aFOperation)
    return;

  ModuleBase_IPropertyPanel* aPanel = aFOperation->propertyPanel();
  if (PartSet_SketcherMgr::isSketchOperation(aFOperation) &&  (aFOperation->isEditOperation())) {
    // we have to manually activate the sketch label in edit mode
    aPanel->activateWidget(aPanel->modelWidgets().first());
  }
  else if ((aFOperation->id() == myLastOperationId.c_str()) && myLastFeature) {
    // Restart last operation type 
    ModuleBase_ModelWidget* aWgt = aPanel->activeWidget();
    PartSet_WidgetPoint2D* aPoint2DWdg = dynamic_cast<PartSet_WidgetPoint2D*>(module()->activeWidget());
    if (aPoint2DWdg && aPoint2DWdg->canBeActivatedByMove()) {
      QList<ModuleBase_ViewerPrs> aSelection;
      aSelection.append(ModuleBase_ViewerPrs(myLastFeature, TopoDS_Shape(), NULL));
      if (aPoint2DWdg->setSelection(aSelection, true))
        aPanel->activateNextWidget(aPoint2DWdg);
    }
  }
}

void PartSet_SketcherReetntrantMgr::noMoreWidgets(const std::string& thePreviousAttributeID)
{
  if (!isActiveMgr())
    return;

  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  if (anOperation) {
    if (PartSet_SketcherMgr::isNestedSketchOperation(anOperation)) {
      if (myRestartingMode != RM_Forbided) {
        myRestartingMode = RM_LastFeatureUsed;
        myPreviousAttributeID = thePreviousAttributeID;
      }
      XGUI_Workshop* aWorkshop = workshop();
      XGUI_OperationMgr* anOpMgr = aWorkshop->operationMgr();
      // do nothing if the feature can not be applyed
      if (anOpMgr->isApplyEnabled())
        anOperation->commit();
    }
  }
}

void PartSet_SketcherReetntrantMgr::vertexSelected()
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

void PartSet_SketcherReetntrantMgr::enterReleased()
{
  if (!isActiveMgr())
    return;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                      (myWorkshop->currentOperation());
  if (/*!aFOperation->isEditOperation() || */myIsInternalEditOperation)
    myRestartingMode = RM_EmptyFeatureUsed;
}

bool PartSet_SketcherReetntrantMgr::canBeCommittedByPreselection()
{
  return !isActiveMgr() || myRestartingMode == RM_None;
}

void PartSet_SketcherReetntrantMgr::onInternalActivateFirstWidgetSelection()
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

bool PartSet_SketcherReetntrantMgr::isActiveMgr() const
{
  PartSet_SketcherMgr* aSketcherMgr = module()->sketchMgr();
  ModuleBase_Operation* aCurrentOperation = myWorkshop->currentOperation();
  return PartSet_SketcherMgr::isSketchOperation(aCurrentOperation) ||
         PartSet_SketcherMgr::isNestedSketchOperation(aCurrentOperation);
}

void PartSet_SketcherReetntrantMgr::breakOperationSequence()
{
  myLastOperationId = "";
  myLastFeature = FeaturePtr();
  myRestartingMode = RM_None;
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
