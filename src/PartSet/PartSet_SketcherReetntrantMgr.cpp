// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

#include "PartSet_SketcherReetntrantMgr.h"
#include "PartSet_Module.h"
#include "PartSet_SketcherMgr.h"
#include "PartSet_WidgetPoint2d.h"

#include "ModelAPI_Session.h"
#include "ModelAPI_AttributeString.h"
#include "ModelAPI_AttributeRefAttr.h"

#include "GeomDataAPI_Point2D.h"

#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_WidgetSelector.h>
#include <ModuleBase_PageWidget.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_OperationDescription.h>
#include "ModuleBase_ToolBox.h"

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>

#include <XGUI_Workshop.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_PropertyPanel.h>

#include <QToolButton>

PartSet_SketcherReetntrantMgr::PartSet_SketcherReetntrantMgr(ModuleBase_IWorkshop* theWorkshop)
: QObject(theWorkshop),
  myWorkshop(theWorkshop),
  myRestartingMode(RM_None),
  myIsFlagsBlocked(false),
  myIsInternalEditOperation(false),
  myNoMoreWidgetsAttribute("")
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

  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  if (anOperation) {
    ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
    ModuleBase_ModelWidget* anActiveWidget = aPanel->activeWidget();
    if (myIsInternalEditOperation && (!anActiveWidget || !anActiveWidget->isViewerSelector()))
      aWidget = myInternalActiveWidget;
  }
  return aWidget;
}

bool PartSet_SketcherReetntrantMgr::isInternalEditActive() const
{
  return myIsInternalEditOperation;
}

void PartSet_SketcherReetntrantMgr::updateInternalEditActiveState()
{
  if  (myIsInternalEditOperation) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                         (myWorkshop->currentOperation());
    if (aFOperation) {
      FeaturePtr aFeature = aFOperation->feature();
      QString anError = myWorkshop->module()->getFeatureError(aFeature);
      // stop started internal edit operation as soon as the operation becomes invalid
      // it is especially important for the sketch tangent arc feature
      if (!anError.isEmpty()) {
        aFOperation->setEditOperation(false);
        //workshop()->operationMgr()->updateApplyOfOperations();
        beforeStopInternalEdit();
        myIsInternalEditOperation = false;
      }
    }
  }
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
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                         (myWorkshop->currentOperation());
    FeaturePtr aLastFeature = myRestartingMode == RM_LastFeatureUsed ? aFOperation->feature()
                                                                     : FeaturePtr();
    if (aLastFeature) {
      ModuleBase_ModelWidget* anActiveWidget = module()->activeWidget();
      ModuleBase_IPropertyPanel* aPanel = myWorkshop->currentOperation()->propertyPanel();
      bool aWidgetIsFilled = false;

      //bool aCanBeActivatedByMove = false;
      FeaturePtr aCurrentFeature = aFOperation->feature();
      bool isLineFeature = false, isArcFeature = false;
      if (aCurrentFeature->getKind() == SketchPlugin_Line::ID())
        isLineFeature = anActiveWidget->attributeID() == SketchPlugin_Line::START_ID();
      else if (isTangentArc(aFOperation))
        isArcFeature = anActiveWidget->attributeID() == SketchPlugin_Arc::TANGENT_POINT_ID();

      bool aCanBeActivatedByMove = isLineFeature || isArcFeature;
      if (aCanBeActivatedByMove) {
        restartOperation();

        anActiveWidget = module()->activeWidget();
        aCurrentFeature = anActiveWidget->feature();
        aProcessed = true;
        if (isLineFeature) {
          PartSet_WidgetPoint2D* aPoint2DWdg = dynamic_cast<PartSet_WidgetPoint2D*>(anActiveWidget);
          if (aPoint2DWdg) { // line, start point should be equal last point of the last feature line
            QList<ModuleBase_ViewerPrsPtr> aSelection;
            aSelection.append(std::shared_ptr<ModuleBase_ViewerPrs>(
               new ModuleBase_ViewerPrs(aLastFeature, GeomShapePtr(), NULL)));
            aWidgetIsFilled = aPoint2DWdg->setSelection(aSelection, true);
          }
        }
        else if (isArcFeature) { // arc, start point should be equal last point of the last feature arc
          if (aCurrentFeature->getKind() == SketchPlugin_Arc::ID()) {
            // get the last point of the previuos arc feature(geom point 2d)
            std::shared_ptr<ModelAPI_Data> aData = aLastFeature->data();
            std::shared_ptr<GeomDataAPI_Point2D> aPointAttr = 
                std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                           aData->attribute(SketchPlugin_Arc::END_ID()));
            // get point attribute on the current feature
            AttributeRefAttrPtr aTangentPointAttr = aCurrentFeature->data()->refattr(
                                                         SketchPlugin_Arc::TANGENT_POINT_ID());
            aTangentPointAttr->setAttr(aPointAttr);
            aWidgetIsFilled = true;
          }
        }
      }
      if (aWidgetIsFilled)
        aPanel->activateNextWidget(anActiveWidget);
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

      // block of viewer update
      // we need to block update content of the viewer because of Sketch Point feature
      // in activate() the value of the point is initialized and it can be displayed
      // but the default value is [0, 0]. So, we block update viewer contentent until
      // onMouseRelease happens, which correct the point position
      ModuleBase_ModelWidget::blockUpdateViewer(true);

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
      // unblock viewer update
      ModuleBase_ModelWidget::blockUpdateViewer(false);
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

  // we should avoid processing of the signal about no more widgets attributes and 
  // do this after the restart operaion is finished if it was called
  // onNoMoreWidgets depends on myIsFlagsBlocked and fill myNoMoreWidgetsAttribute
  // if it should be called after restart
  if (myIsFlagsBlocked) {
    myNoMoreWidgetsAttribute = thePreviousAttributeID;
    return;
  }

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                       (myWorkshop->currentOperation());
  if (!myWorkshop->module()->getFeatureError(aFOperation->feature()).isEmpty())
    return;

  if (aFOperation && PartSet_SketcherMgr::isNestedSketchOperation(aFOperation)) {
    bool isStarted = false;
    if (!module()->sketchMgr()->sketchSolverError()) {
      if (myRestartingMode != RM_Forbided) {
        myRestartingMode = RM_LastFeatureUsed;
        isStarted = startInternalEdit(thePreviousAttributeID);
      }
    }
    if (!isStarted)
      aFOperation->commit();
  }
}

bool PartSet_SketcherReetntrantMgr::processEnter(const std::string& thePreviousAttributeID)
{
  bool isDone = false;

  if (!isActiveMgr())
    return isDone;

  // empty previous attribute means that the Apply/Ok button has focus and the enter
  // should not lead to start edition mode of the previous operation
  if (thePreviousAttributeID.empty())
    return isDone;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                       (myWorkshop->currentOperation());
  if (!myWorkshop->module()->getFeatureError(aFOperation->feature()).isEmpty())
    return isDone;

  bool isSketchSolverError = module()->sketchMgr()->sketchSolverError();

  if (!isSketchSolverError) {
    myRestartingMode = RM_EmptyFeatureUsed;
    isDone = startInternalEdit(thePreviousAttributeID);
  }

  return isDone;
}

void PartSet_SketcherReetntrantMgr::onVertexSelected()
{
  if (!isActiveMgr())
    return;

  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  std::string anOperationId = anOperation->id().toStdString();
  if (anOperationId == SketchPlugin_Line::ID() || isTangentArc(anOperation)) {
    /// If last line finished on vertex the lines creation sequence has to be break
    ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
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

  beforeStopInternalEdit();
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

bool PartSet_SketcherReetntrantMgr::startInternalEdit(const std::string& thePreviousAttributeID)
{
  bool isDone = false;
  /// this is workaround for ModuleBase_WidgetEditor, used in SALOME mode. Sometimes key enter
  /// event comes two times, so we should not start another internal edit operation
  /// the Apply button becomes disabled becase the second additional internal feature is created
  if (myIsInternalEditOperation)
    return true;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                     (myWorkshop->currentOperation());

  if (aFOperation && PartSet_SketcherMgr::isNestedSketchOperation(aFOperation)) {
    aFOperation->setEditOperation(true/*, false*/);
    workshop()->operationMgr()->updateApplyOfOperations();

    createInternalFeature();

    myIsInternalEditOperation = true;
    isDone = true;
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
          if (aWidgets[i]->attributeID() == thePreviousAttributeID) {
          /// workaround for the same attributes used in different stacked widgets(attribute types)
          if (ModuleBase_ToolBox::isOffToolBoxParent(aWidgets[i]))
            continue;
            aPreviousAttributeWidget = aWidgets[i];
          }
        }
        // If the current widget is a selector, do nothing, it processes the mouse press
        if (aPreviousAttributeWidget) {
          if (!aPreviousAttributeWidget->isViewerSelector()) {
            aPreviousAttributeWidget->focusTo();
            aPreviousAttributeWidget->selectContent();
          }
          else {
            // in case of shape multi selector, the widget does not lose focus by filling
            // like it is in shape selector. So, if enter is pressed, the multi shape selector
            // control should be deactivated. The focus is moved to Apply button and there
            // should not be active control visualized in property panel
            if (aPreviousAttributeWidget == aPanel->activeWidget()) {
              aPanel->activateWidget(NULL, false);
            }
            // if there is no the next widget to be automatically activated, the Ok button in property
            // panel should accept the focus(example is parallel constraint on sketch lines)
            QToolButton* anOkBtn = aPanel->findChild<QToolButton*>(PROP_PANEL_OK);
            if (anOkBtn)
              anOkBtn->setFocus(Qt::TabFocusReason);
          }
        }
      }
    }
  }
  if (isDone)
    module()->sketchMgr()->clearClickedFlags();

  return isDone;
}

void PartSet_SketcherReetntrantMgr::beforeStopInternalEdit()
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                      (myWorkshop->currentOperation());
  if (aFOperation) {
    disconnect(aFOperation, SIGNAL(beforeCommitted()), this, SLOT(onBeforeStopped()));
    disconnect(aFOperation, SIGNAL(beforeAborted()), this, SLOT(onBeforeStopped()));
  }

  deleteInternalFeature();
}

void PartSet_SketcherReetntrantMgr::restartOperation()
{
  if (myIsInternalEditOperation) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(
                                                                  myWorkshop->currentOperation());
    if (aFOperation) {
      myNoMoreWidgetsAttribute = "";
      myIsFlagsBlocked = true;
      FeaturePtr aPrevFeature = aFOperation->feature();
      aFOperation->commit();
      module()->launchOperation(aFOperation->id());
      // allow the same attribute values in restarted operation
      ModuleBase_OperationFeature* aCurrentOperation = dynamic_cast<ModuleBase_OperationFeature*>(
                                                                  myWorkshop->currentOperation());
      copyReetntrantAttributes(aPrevFeature, aCurrentOperation->feature());

      myIsFlagsBlocked = false;
      resetFlags();
      // we should avoid processing of the signal about no more widgets attributes and 
      // do this after the restart operaion is finished if it was called
      // onNoMoreWidgets depends on myIsFlagsBlocked and fill myNoMoreWidgetsAttribute
      // if it should be called after restart
      if (!myNoMoreWidgetsAttribute.empty()) {
        onNoMoreWidgets(myNoMoreWidgetsAttribute);
        myNoMoreWidgetsAttribute = "";
      }
    }
  }
}

void PartSet_SketcherReetntrantMgr::createInternalFeature()
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                     (myWorkshop->currentOperation());

  if (aFOperation && PartSet_SketcherMgr::isNestedSketchOperation(aFOperation)) {
    FeaturePtr anOperationFeature = aFOperation->feature();

    CompositeFeaturePtr aSketch = module()->sketchMgr()->activeSketch();
    myInternalFeature = aSketch->addFeature(anOperationFeature->getKind());

    bool isFeatureChanged = copyReetntrantAttributes(anOperationFeature, myInternalFeature);
    XGUI_PropertyPanel* aPropertyPanel = dynamic_cast<XGUI_PropertyPanel*>
                                                  (aFOperation->propertyPanel());

    myInternalWidget = new QWidget(aPropertyPanel->contentWidget()->pageWidget());
    myInternalWidget->setVisible(false);

    ModuleBase_PageWidget* anInternalPage = new ModuleBase_PageWidget(myInternalWidget);

    QString aXmlRepr = aFOperation->getDescription()->xmlRepresentation();
    ModuleBase_WidgetFactory aFactory(aXmlRepr.toStdString(), myWorkshop);

    aFactory.createWidget(anInternalPage);
    QList<ModuleBase_ModelWidget*> aWidgets = aFactory.getModelWidgets();

    foreach (ModuleBase_ModelWidget* aWidget, aWidgets) {
      bool isStoreValue = !aFOperation->isEditOperation() &&
                          !aWidget->getDefaultValue().empty() &&
                          !aWidget->isComputedDefault();
      aWidget->setFeature(myInternalFeature, isStoreValue);
      if (!isStoreValue && isFeatureChanged)
        aWidget->restoreValue();
    }

    ModuleBase_ModelWidget* aFirstWidget = ModuleBase_IPropertyPanel::findFirstAcceptingValueWidget
                                                                                        (aWidgets);
    if (aFirstWidget)
      myInternalActiveWidget = aFirstWidget;
  }
}

void PartSet_SketcherReetntrantMgr::deleteInternalFeature()
{
  if (myInternalActiveWidget) {
    ModuleBase_WidgetSelector* aWSelector = dynamic_cast<ModuleBase_WidgetSelector*>(myInternalActiveWidget);
    if (aWSelector)
      aWSelector->activateSelectionAndFilters(false);
    myInternalActiveWidget = 0;
  }
  delete myInternalWidget;
  myInternalWidget = 0;

  QObjectPtrList anObjects;
  anObjects.append(myInternalFeature);
  workshop()->deleteFeatures(anObjects);
}

void PartSet_SketcherReetntrantMgr::resetFlags()
{
  if (!myIsFlagsBlocked) {
    myIsInternalEditOperation = false;
    myRestartingMode = RM_None;
  }
}

bool PartSet_SketcherReetntrantMgr::copyReetntrantAttributes(const FeaturePtr& theSourceFeature,
                                                             const FeaturePtr& theNewFeature)
{
  bool aChanged = false;
  std::string aTypeAttributeId;
  if (theSourceFeature->getKind() == SketchPlugin_Circle::ID()) {
    aTypeAttributeId = SketchPlugin_Circle::CIRCLE_TYPE();
  }
  if (theSourceFeature->getKind() == SketchPlugin_Arc::ID()) {
    aTypeAttributeId = SketchPlugin_Arc::ARC_TYPE();
  }
  if (!aTypeAttributeId.empty()) {
    AttributeStringPtr aSourceFeatureTypeAttr = theSourceFeature->data()->string(aTypeAttributeId);
    AttributeStringPtr aNewFeatureTypeAttr = theNewFeature->data()->string(aTypeAttributeId);
    aNewFeatureTypeAttr->setValue(aSourceFeatureTypeAttr->value());
    ModuleBase_ModelWidget::updateObject(theNewFeature);
    aChanged = true;
  }
  return aChanged;
}

bool PartSet_SketcherReetntrantMgr::isTangentArc(ModuleBase_Operation* theOperation)
{
  bool aTangentArc = false;
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                        (theOperation);
  if (aFOperation && PartSet_SketcherMgr::isNestedSketchOperation(aFOperation)) {
    FeaturePtr aFeature = aFOperation->feature();
    if (aFeature.get() && aFeature->getKind() == SketchPlugin_Arc::ID()) {
      AttributeStringPtr aTypeAttr = aFeature->data()->string(SketchPlugin_Arc::ARC_TYPE());
      std::string anArcType = aTypeAttr.get() ? aTypeAttr->value() : "";
      aTangentArc = anArcType == SketchPlugin_Arc::ARC_TYPE_TANGENT();
    }
  }
  return aTangentArc;
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

