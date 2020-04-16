// Copyright (C) 2014-2020  CEA/DEN, EDF R&D
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

#include "PartSet_SketcherReentrantMgr.h"
#include "PartSet_ExternalObjectsMgr.h"
#include "PartSet_Module.h"
#include "PartSet_SketcherMgr.h"
#include "PartSet_WidgetPoint2d.h"

#include "ModelAPI_Session.h"
#include "ModelAPI_AttributeString.h"
#include "ModelAPI_AttributeRefAttr.h"
#include "ModelAPI_AttributeReference.h"
#include "ModelAPI_EventReentrantMessage.h"

#include "GeomDataAPI_Point2D.h"

#include "GeomAPI_Lin2d.h"
#include "GeomAPI_Dir2d.h"

#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ISelectionActivate.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_ViewerPrs.h>
#include <ModuleBase_WidgetSelector.h>
#include <ModuleBase_PageWidget.h>
#include <ModuleBase_PageBase.h>
#include <ModuleBase_WidgetFactory.h>
#include <ModuleBase_OperationDescription.h>
#include "ModuleBase_ToolBox.h"
#include "ModuleBase_ISelection.h"

#include <SketchPlugin_Feature.h>
#include <SketchPlugin_Line.h>
#include <SketchPlugin_MacroArc.h>
#include <SketchPlugin_MacroCircle.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Trim.h>
#include <SketchPlugin_Split.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintVertical.h>

#include <XGUI_Workshop.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ErrorMgr.h>
#include <XGUI_SelectionMgr.h>

#include <QToolButton>

//#define DEBUG_RESTART

PartSet_SketcherReentrantMgr::PartSet_SketcherReentrantMgr(ModuleBase_IWorkshop* theWorkshop)
: QObject(theWorkshop),
  myWorkshop(theWorkshop),
  myRestartingMode(RM_None),
  myIsFlagsBlocked(false),
  myIsInternalEditOperation(false),
  myNoMoreWidgetsAttribute(""),
  myIsAutoConstraints(true)
{
}

PartSet_SketcherReentrantMgr::~PartSet_SketcherReentrantMgr()
{
}

bool PartSet_SketcherReentrantMgr::isInternalEditActive() const
{
  return myIsInternalEditOperation;
}

void PartSet_SketcherReentrantMgr::updateInternalEditActiveState()
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
        myIsInternalEditOperation = false;
        updateAcceptAllAction();
      }
    }
  }
}

bool PartSet_SketcherReentrantMgr::operationCommitted(ModuleBase_Operation* theOperation)
{
  bool aProcessed = false;
  if (!isActiveMgr())
    return aProcessed;

  aProcessed = myIsInternalEditOperation;
  resetFlags();

  return aProcessed;
}

void PartSet_SketcherReentrantMgr::operationStarted(ModuleBase_Operation* theOperation)
{
  if (!isActiveMgr())
    return;

  //if (myPreviousFeature.get() && myRestartingMode == RM_LastFeatureUsed) {
    //ModuleBase_OperationFeature* aCurrentOperation = dynamic_cast<ModuleBase_OperationFeature*>(
    //                                                            myWorkshop->currentOperation());
    //CompositeFeaturePtr aSketch = module()->sketchMgr()->activeSketch();
    //if (myPreviousFeature.get() && myPreviousFeature->data()->isValid()) // it is not removed
      //copyReetntrantAttributes(myPreviousFeature, aCurrentOperation->feature(), aSketch);
  //}
  resetFlags();
}

void PartSet_SketcherReentrantMgr::operationAborted(ModuleBase_Operation* theOperation)
{
  if (!isActiveMgr())
    return;

  resetFlags();
}

bool PartSet_SketcherReentrantMgr::processMouseMoved(ModuleBase_IViewWindow* theWnd,
                                                      QMouseEvent* theEvent)
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

      FeaturePtr aCurrentFeature = aFOperation->feature();
      bool isLineFeature = false, isReentrantArcFeature = false;
      std::string anAttributeOnStart;
      if (aCurrentFeature->getKind() == SketchPlugin_Line::ID()) {
        anAttributeOnStart = SketchPlugin_Line::START_ID();
        isLineFeature = anActiveWidget->attributeID() == anAttributeOnStart;
      }
      else if (isTangentArc(aFOperation, module()->sketchMgr()->activeSketch())) {
        isReentrantArcFeature = true;
      }
      bool aCanBeActivatedByMove = isLineFeature || isReentrantArcFeature;
      if (aCanBeActivatedByMove) {
        /// before restarting of operation we need to clear selection, as it may take part in
        /// new feature creation, e.g. tangent arc. But it is not necessary as it was processed
        /// by mouse release when the operation was restarted.
        workshop()->selector()->clearSelection();

        myPreviousFeature = aFOperation->feature();
        restartOperation();
        myPreviousFeature = FeaturePtr();

        anActiveWidget = module()->activeWidget();
        aProcessed = true;
        if (anActiveWidget && anActiveWidget->attributeID() == anAttributeOnStart) {
          // it was not deactivated by preselection processing
          aPanel->activateNextWidget(anActiveWidget);
        }
      } else {
        // processing mouse move in active widget of restarted operation
        ModuleBase_ModelWidget* anActiveWidget = module()->activeWidget();
        PartSet_MouseProcessor* aProcessor = dynamic_cast<PartSet_MouseProcessor*>(anActiveWidget);
        if (aProcessor)
          aProcessor->mouseMoved(theWnd, theEvent);
      }
    }
  }
  return aProcessed;
}

bool PartSet_SketcherReentrantMgr::processMousePressed(ModuleBase_IViewWindow* /* theWnd*/,
                                                        QMouseEvent* /* theEvent*/)
{
  return isActiveMgr() && myIsInternalEditOperation;
}

bool PartSet_SketcherReentrantMgr::processMouseReleased(ModuleBase_IViewWindow* theWindow,
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
      ModuleBase_Tools::blockUpdateViewer(true);

      ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                           (myWorkshop->currentOperation());
      myPreviousFeature = aFOperation->feature();

      /// selection should be obtained from workshop before ask if the operation can be started as
      /// the canStartOperation method performs commit/abort of previous operation.
      /// Sometimes commit/abort
      /// may cause selection clear(Sketch operation) as a result
      /// it will be lost and is not used for preselection.
      ModuleBase_ISelection* aSelection = myWorkshop->selection();
      QList<ModuleBase_ViewerPrsPtr> aPreSelected =
        aSelection->getSelected(ModuleBase_ISelection::AllControls);

      myClickedSketchPoint = PartSet_Tools::getPnt2d(theEvent, theWindow,
                                                     module()->sketchMgr()->activeSketch());
      FeaturePtr anExternalCreatedFeature;
      if (!aPreSelected.empty()) {
        ModuleBase_ViewerPrsPtr aValue = aPreSelected.first();
        module()->getGeomSelection(aValue, mySelectedObject, mySelectedAttribute);

        PartSet_WidgetPoint2D* aPointWidget = dynamic_cast<PartSet_WidgetPoint2D*>(anActiveWidget);
        if (aPointWidget) {
          GeomShapePtr aShape;
          aPointWidget->getGeomSelection_(aValue, mySelectedObject, aShape);
          ObjectPtr anExternalObject =
            aPointWidget->getExternalObjectMgr()->getExternalObjectValidated();
          // if external object is during reentrant operation and is used as a parameter of feature
          // it should be removed after the operation is restarted. (Circle feature, Projection)
          if (anExternalObject.get())
            anExternalCreatedFeature = ModelAPI_Feature::feature(anExternalObject);
        }
      }

      restartOperation();
      // remove created external feature
      if (anExternalCreatedFeature.get()) {
        QObjectPtrList anObjects;
        anObjects.append(anExternalCreatedFeature);
        workshop()->deleteFeatures(anObjects);
      }

      myClickedSketchPoint = std::shared_ptr<GeomAPI_Pnt2d>();
      mySelectedObject = ObjectPtr();
      mySelectedAttribute = AttributePtr();

      myPreviousFeature = FeaturePtr();
      aProcessed = true;

      // fill the first widget by the mouse event point
      // if the active widget is not the first, it means that the restarted operation is filled by
      // the current preselection.
      PartSet_MouseProcessor* aMouseProcessor = dynamic_cast<PartSet_MouseProcessor*>(
                                                                       module()->activeWidget());
      //PartSet_WidgetPoint2D* aPoint2DWdg =
      //  dynamic_cast<PartSet_WidgetPoint2D*>(module()->activeWidget());
      PartSet_MouseProcessor* aFirstWidget = dynamic_cast<PartSet_MouseProcessor*>(
                                                        aPanel->findFirstAcceptingValueWidget());
      //if (aPoint2DWdg && aPoint2DWdg == aFirstWidget) {
      if (aMouseProcessor && aMouseProcessor == aFirstWidget) {
        std::shared_ptr<ModuleBase_ViewerPrs> aSelectedPrs;
        if (!aPreSelected.empty())
          aSelectedPrs = aPreSelected.front();
        if (aSelectedPrs.get() && aSelectedPrs->object().get()
            && !aSelectedPrs->object()->data()->isValid()) {
          // the selected object was removed diring restart, e.g. presentable macro feature
          // there are created objects to replace the object depending on created feature kind
          aSelectedPrs = std::shared_ptr<ModuleBase_ViewerPrs>();
        }
        aMouseProcessor->setPreSelection(aSelectedPrs, theWindow, theEvent);
        //aPoint2DWdg->mouseReleased(theWindow, theEvent);
        //if (!aPreSelected.empty())
        //  aPoint2DWdg->setPreSelection(ModuleBase_ViewerPrsPtr());
      }
      // unblock viewer update
      ModuleBase_Tools::blockUpdateViewer(false);
    }
  }
  return aProcessed;
}

//******************************************************
void PartSet_SketcherReentrantMgr::setReentrantPreSelection(
                                       const std::shared_ptr<Events_Message>& theMessage)
{
  ReentrantMessagePtr aReentrantMessage =
                      std::dynamic_pointer_cast<ModelAPI_EventReentrantMessage>(theMessage);
  if (!aReentrantMessage.get())
    return;

  // if feature has already filled the selected object, we should not overwrite it
  if (!aReentrantMessage->selectedObject().get())
    aReentrantMessage->setSelectedObject(mySelectedObject);

  aReentrantMessage->setSelectedAttribute(mySelectedAttribute);
  aReentrantMessage->setClickedPoint(myClickedSketchPoint);
}

//void PartSet_SketcherReentrantMgr::onWidgetActivated()
//{
//  if (!isActiveMgr())
//    return;
//  if (!myIsInternalEditOperation)
//    return;
//
//  PartSet_Module* aModule = module();
//  ModuleBase_ModelWidget* aFirstWidget = aModule->activeWidget();
//  ModuleBase_IPropertyPanel* aPanel = aModule->currentOperation()->propertyPanel();
//  if (aFirstWidget != aPanel->activeWidget()) {
//    ModuleBase_WidgetSelector* aWSelector = dynamic_cast<ModuleBase_WidgetSelector*>
//      (aFirstWidget);
//    if (aWSelector) {
//      myWorkshop->selectionActivate()->updateSelectionModesAndFilters(aWSelector);
//    }
//  }
//}

void PartSet_SketcherReentrantMgr::onNoMoreWidgets(const std::string& thePreviousAttributeID)
{
#ifdef DEBUG_RESTART
  std::cout << "PartSet_SketcherReentrantMgr::onNoMoreWidgets" << std::endl;
#endif

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
  if (module()->sketchMgr()->isDragModeCreation()) {
    if (aFOperation && myIsAutoConstraints)
      addConstraints(aFOperation->feature());
    return;
  }
  if (!myWorkshop->module()->getFeatureError(aFOperation->feature()).isEmpty())
    return;

  if (aFOperation && module()->sketchMgr()->isNestedSketchOperation(aFOperation)) {
    if (aFOperation->isNeedToBeAborted()) {
      aFOperation->abort();
    }
    else {
      bool isStarted = false;
      if (!module()->sketchMgr()->sketchSolverError()) {
        if (myRestartingMode != RM_Forbided) {
          myRestartingMode = RM_LastFeatureUsed;
          isStarted = startInternalEdit(thePreviousAttributeID);
        }
      }
      if (!isStarted) {
        if (myIsAutoConstraints)
          addConstraints(aFOperation->feature());
        aFOperation->commit();
      }
    }
  }
}

bool PartSet_SketcherReentrantMgr::processEnter(const std::string& thePreviousAttributeID)
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
    // It seems that the call is obsolete for Enter key processing
    // It prevent finalysing of the current operation by Enter key
    //isDone = startInternalEdit(thePreviousAttributeID);
  }

  return isDone;
}

void PartSet_SketcherReentrantMgr::onVertexSelected()
{
  if (!isActiveMgr())
    return;

  ModuleBase_Operation* anOperation = myWorkshop->currentOperation();
  std::string anOperationId = anOperation->id().toStdString();
  if (anOperationId == SketchPlugin_Line::ID() ||
      isTangentArc(anOperation, module()->sketchMgr()->activeSketch())) {
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

void PartSet_SketcherReentrantMgr::onAfterValuesChangedInPropertyPanel()
{

  if (isInternalEditActive()) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                       (myWorkshop->currentOperation());
    ModuleBase_ModelWidget* aWidget = (ModuleBase_ModelWidget*)sender();
    if (!aWidget->isModifiedInEdit().empty())
      restartOperation();
  }
}

void PartSet_SketcherReentrantMgr::onBeforeStopped()
{
  if (!isActiveMgr() || !myIsInternalEditOperation)
    return;

  beforeStopInternalEdit();
}

bool PartSet_SketcherReentrantMgr::canBeCommittedByPreselection()
{
  return !isActiveMgr() || myRestartingMode == RM_None;
}

bool PartSet_SketcherReentrantMgr::isActiveMgr() const
{
  ModuleBase_Operation* aCurrentOperation = myWorkshop->currentOperation();

  bool anActive = PartSet_SketcherMgr::isSketchOperation(aCurrentOperation);
  if (!anActive) {
    anActive = module()->sketchMgr()->isNestedSketchOperation(aCurrentOperation);
    if (anActive) { // the manager is not active when the current operation is a usual Edit
      ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                       (myWorkshop->currentOperation());
      if (aFOperation->isEditOperation())
        anActive = myIsInternalEditOperation;
    }
  }
  return anActive;
}

bool PartSet_SketcherReentrantMgr::startInternalEdit(const std::string& thePreviousAttributeID)
{
#ifdef DEBUG_RESTART
  std::cout << "PartSet_SketcherReentrantMgr::startInternalEdit" << std::endl;
#endif

  bool isDone = false;
  /// this is workaround for ModuleBase_WidgetEditor, used in SALOME mode. Sometimes key enter
  /// event comes two times, so we should not start another internal edit operation
  /// the Apply button becomes disabled becase the second additional internal feature is created
  if (myIsInternalEditOperation)
    return true;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                     (myWorkshop->currentOperation());

  if (aFOperation && module()->sketchMgr()->isNestedSketchOperation(aFOperation)) {
    /// improvement to deselect automatically all eventual selected objects, when
    // returning to the neutral point of the Sketcher or start internal edit
    workshop()->selector()->clearSelection();

    if (myIsAutoConstraints)
      addConstraints(aFOperation->feature());

    aFOperation->setEditOperation(true/*, false*/);
    createInternalFeature();

    myIsInternalEditOperation = true;
    updateAcceptAllAction();

    isDone = true;
    connect(aFOperation, SIGNAL(beforeCommitted()), this, SLOT(onBeforeStopped()));
    connect(aFOperation, SIGNAL(beforeAborted()), this, SLOT(onBeforeStopped()));

    // activate selection filters of the first widget in the viewer
    //onWidgetActivated();

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
            aPreviousAttributeWidget->emitFocusInWidget();
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
            // if there is no the next widget to be automatically activated,
            // the Ok button in property
            // panel should accept the focus(example is parallel constraint on sketch lines)
            QToolButton* anOkBtn =
              dynamic_cast<XGUI_PropertyPanel*>(aPanel)->findButton(PROP_PANEL_OK);
            if (anOkBtn)
              ModuleBase_Tools::setFocus(anOkBtn, "XGUI_PropertyPanel::activateNextWidget");
          }
        }
      }
    }
  }
  if (isDone)
    module()->sketchMgr()->clearClickedFlags();

  return isDone;
}

void PartSet_SketcherReentrantMgr::beforeStopInternalEdit()
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                      (myWorkshop->currentOperation());
  if (aFOperation) {
    disconnect(aFOperation, SIGNAL(beforeCommitted()), this, SLOT(onBeforeStopped()));
    disconnect(aFOperation, SIGNAL(beforeAborted()), this, SLOT(onBeforeStopped()));
  }

  deleteInternalFeature();
}

void PartSet_SketcherReentrantMgr::restartOperation()
{
#ifdef DEBUG_RESTART
  std::cout << "PartSet_SketcherReentrantMgr::restartOperation" << std::endl;
#endif

  if (myIsInternalEditOperation) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>(
                                                                  myWorkshop->currentOperation());
    if (aFOperation) {
      ModuleBase_ISelection* aSelection = myWorkshop->selection();
      QList<ModuleBase_ViewerPrsPtr> aPreSelected =
        aSelection->getSelected(ModuleBase_ISelection::AllControls);

      if (myInternalFeature.get())
        copyReetntrantAttributes(myInternalFeature, aFOperation->feature(),
                                 module()->sketchMgr()->activeSketch());

      myNoMoreWidgetsAttribute = "";
      myIsFlagsBlocked = true;
      /// launch has 'false' parameter to do not start new operation if the previous operation
      /// is not committed. It is important for Line Sketch feature as it uses the previous
      /// created feature parameter(to build coincidence), but by abort the previous is removed
      module()->launchOperation(aFOperation->id(), true);
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

void PartSet_SketcherReentrantMgr::createInternalFeature()
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                     (myWorkshop->currentOperation());

  if (aFOperation && module()->sketchMgr()->isNestedSketchOperation(aFOperation)) {
    FeaturePtr anOperationFeature = aFOperation->feature();

    CompositeFeaturePtr aSketch = module()->sketchMgr()->activeSketch();
    myInternalFeature = aSketch->addFeature(anOperationFeature->getKind());

#ifdef DEBUG_RESTART
    std::cout << "PartSet_SketcherReentrantMgr::createInternalFeature: "
              << myInternalFeature->data()->name() << std::endl;
#endif

    bool isFeatureChanged = copyReetntrantAttributes(anOperationFeature, myInternalFeature,
                                                     aSketch, false);
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
      setInternalActiveWidget(aFirstWidget);
  }
}

void PartSet_SketcherReentrantMgr::deleteInternalFeature()
{
#ifdef DEBUG_RESTART
  std::cout << "PartSet_SketcherReentrantMgr::deleteInternalFeature: "
            << myInternalFeature->data()->name() << std::endl;
#endif
  setInternalActiveWidget(0);
  delete myInternalWidget;
  myInternalWidget = 0;

  QObjectPtrList anObjects;
  anObjects.append(myInternalFeature);
  workshop()->deleteFeatures(anObjects);
  myInternalFeature = FeaturePtr();
}

void PartSet_SketcherReentrantMgr::resetFlags()
{
  if (!myIsFlagsBlocked) {
    myIsInternalEditOperation = false;
    updateAcceptAllAction();
    myRestartingMode = RM_None;
    myReentrantMessage = std::shared_ptr<Events_Message>();
  }
}

bool PartSet_SketcherReentrantMgr::copyReetntrantAttributes(const FeaturePtr& theSourceFeature,
                                                             const FeaturePtr& theNewFeature,
                                                             const CompositeFeaturePtr& theSketch,
                                                             const bool /*isTemporary*/)
{
  bool aChanged = false;
  if (!theSourceFeature.get() || !theSourceFeature->data().get() ||
      !theSourceFeature->data()->isValid())
    return aChanged;

#ifdef DEBUG_RESTART
  std::cout << "PartSet_SketcherReentrantMgr::copyReetntrantAttributes from '"
            << theSourceFeature->data()->name() << "' to '" << theNewFeature->data()->name()
            << "'" << std::endl;
#endif

  std::string aFeatureKind = theSourceFeature->getKind();
  /*if (aFeatureKind == SketchPlugin_Line::ID()) {
    // Initialize new line with first point equal to end of previous
    std::shared_ptr<ModelAPI_Data> aSFData = theSourceFeature->data();
    std::shared_ptr<GeomDataAPI_Point2D> aSPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                 aSFData->attribute(SketchPlugin_Line::END_ID()));
    std::shared_ptr<ModelAPI_Data> aNFData = theNewFeature->data();
    std::shared_ptr<GeomDataAPI_Point2D> aNPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                 aNFData->attribute(SketchPlugin_Line::START_ID()));
    aNPoint->setValue(aSPoint->x(), aSPoint->y());
    PartSet_Tools::createConstraint(theSketch, aSPoint, aNPoint);

    aNPoint = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                                                 aSFData->attribute(SketchPlugin_Line::END_ID()));
    aNPoint->setValue(aSPoint->x(), aSPoint->y());
  }
  else*/ if (aFeatureKind == SketchPlugin_MacroCircle::ID()) {
    // set circle type
    /*std::string aTypeAttributeId = SketchPlugin_MacroCircle::CIRCLE_TYPE();
    AttributeStringPtr aSourceFeatureTypeAttr = theSourceFeature->data()->string(aTypeAttributeId);
    AttributeStringPtr aNewFeatureTypeAttr = theNewFeature->data()->string(aTypeAttributeId);
    if (aNewFeatureTypeAttr->value() != aTypeAttributeId) // do nothing if there is no changes
      aNewFeatureTypeAttr->setValue(aSourceFeatureTypeAttr->value());
    //ModuleBase_Tools::flushUpdated(theNewFeature);*/
    //aChanged = true;
  }
  else if (aFeatureKind == SketchPlugin_MacroArc::ID()) {
    // set arc type
    /*std::string aTypeAttributeId = SketchPlugin_MacroArc::ARC_TYPE();
    AttributeStringPtr aSourceFeatureTypeAttr = theSourceFeature->data()->string(aTypeAttributeId);
    AttributeStringPtr aNewFeatureTypeAttr = theNewFeature->data()->string(aTypeAttributeId);
    if (aNewFeatureTypeAttr->value() != aTypeAttributeId) // do nothing if there is no changes
      aNewFeatureTypeAttr->setValue(aSourceFeatureTypeAttr->value());*/
    //// if the arc is tangent, set coincidence to end point of the previous arc
    //std::string anArcType = aSourceFeatureTypeAttr->value();
    //if (anArcType == SketchPlugin_Arc::ARC_TYPE_TANGENT()) {
    //  // get the last point of the previuos arc feature(geom point 2d)
    //  std::shared_ptr<ModelAPI_Data> aSData = theSourceFeature->data();
    //  std::shared_ptr<GeomDataAPI_Point2D> aSPointAttr =
    //                                  std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    //                                  aSData->attribute(SketchPlugin_Arc::END_ID()));
    //  // get point attribute on the current feature
    //  AttributeRefAttrPtr aTangentPointAttr = theNewFeature->data()->refattr(
    //                                                SketchPlugin_Arc::TANGENT_POINT_ID());
    //  aTangentPointAttr->setAttr(aSPointAttr);

    //  std::shared_ptr<GeomDataAPI_Point2D> aNPointAttr =
    //                                std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
    //                                theNewFeature->data()->attribute(SketchPlugin_Arc::END_ID()));
    //  aNPointAttr->setValue(aSPointAttr->x(), aSPointAttr->y());

    //}
    //ModuleBase_Tools::flushUpdated(theNewFeature);
    //aChanged = true;
  }
  else if (aFeatureKind == SketchPlugin_Trim::ID() ||
           aFeatureKind == SketchPlugin_Split::ID()) {
    std::string aPreviewObjectAttribute = aFeatureKind == SketchPlugin_Trim::ID() ?
                SketchPlugin_Trim::PREVIEW_OBJECT(): SketchPlugin_Split::PREVIEW_OBJECT();
    std::string aPreviewPointAttribute = aFeatureKind == SketchPlugin_Trim::ID() ?
                SketchPlugin_Trim::PREVIEW_POINT(): SketchPlugin_Split::PREVIEW_POINT();
    std::shared_ptr<ModelAPI_AttributeReference> aRefPreviewAttr =
                      std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                      theSourceFeature->data()->attribute(aPreviewObjectAttribute));
    std::shared_ptr<ModelAPI_AttributeReference> aNRefPreviewAttr =
                        std::dynamic_pointer_cast<ModelAPI_AttributeReference>(
                        theNewFeature->data()->attribute(aPreviewObjectAttribute));
    aNRefPreviewAttr->setValue(aRefPreviewAttr->value());
    std::shared_ptr<GeomDataAPI_Point2D> aPointPreviewAttr =
                      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                      theSourceFeature->data()->attribute(aPreviewPointAttribute));
    std::shared_ptr<GeomDataAPI_Point2D> aNPointPreviewAttr =
                      std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
                      theNewFeature->data()->attribute(aPreviewPointAttribute));
    aNPointPreviewAttr->setValue(aPointPreviewAttr->x(), aPointPreviewAttr->y());
  }
  return aChanged;
}

bool PartSet_SketcherReentrantMgr::isTangentArc(ModuleBase_Operation* theOperation,
                                                 const CompositeFeaturePtr& /*theSketch*/) const
{
  bool aTangentArc = false;
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                        (theOperation);
  if (aFOperation && module()->sketchMgr()->isNestedSketchOperation(aFOperation)) {
    FeaturePtr aFeature = aFOperation->feature();
    if (aFeature.get() && aFeature->getKind() == SketchPlugin_MacroArc::ID()) {
      AttributeStringPtr aTypeAttr = aFeature->data()->string(SketchPlugin_MacroArc::ARC_TYPE());
      std::string anArcType = aTypeAttr.get() ? aTypeAttr->value() : "";
      aTangentArc = anArcType == SketchPlugin_MacroArc::ARC_TYPE_BY_TANGENT_EDGE();
    }
  }
  return aTangentArc;
}

void PartSet_SketcherReentrantMgr::updateAcceptAllAction()
{
  CompositeFeaturePtr aSketch = module()->sketchMgr()->activeSketch();
  if (aSketch.get())
    workshop()->errorMgr()->updateAcceptAllAction(aSketch);
}

XGUI_Workshop* PartSet_SketcherReentrantMgr::workshop() const
{
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myWorkshop);
  return aConnector->workshop();
}

PartSet_Module* PartSet_SketcherReentrantMgr::module() const
{
  return dynamic_cast<PartSet_Module*>(myWorkshop->module());
}

void PartSet_SketcherReentrantMgr::setInternalActiveWidget(ModuleBase_ModelWidget* theWidget)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
    (myWorkshop->currentOperation());
  if (aFOperation)
  {
    XGUI_PropertyPanel* aPropertyPanel = dynamic_cast<XGUI_PropertyPanel*>
      (aFOperation->propertyPanel());
    if (aPropertyPanel)
      aPropertyPanel->setInternalActiveWidget(theWidget);
  }
}

void PartSet_SketcherReentrantMgr::onAutoConstraints(bool isOn)
{
  myIsAutoConstraints = isOn;
}

void PartSet_SketcherReentrantMgr::addConstraints(const FeaturePtr& theFeature)
{
  if (theFeature->getKind() != SketchPlugin_Line::ID())
    return;

  static GeomDir2dPtr myHorDir(new GeomAPI_Dir2d(1, 0));
  static GeomDir2dPtr myVertDir(new GeomAPI_Dir2d(0, 1));

  std::shared_ptr<ModelAPI_Data> aData = theFeature->data();
  std::shared_ptr<GeomDataAPI_Point2D> aPoint1 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::START_ID()));
  std::shared_ptr<GeomDataAPI_Point2D> aPoint2 = std::dynamic_pointer_cast<GeomDataAPI_Point2D>(
      aData->attribute(SketchPlugin_Line::END_ID()));
  if (aPoint1.get() && aPoint2.get()) {
    GeomLine2dPtr aLine(new GeomAPI_Lin2d(aPoint1->pnt(), aPoint2->pnt()));
    GeomDir2dPtr aDir = aLine->direction();
    double aHorAngle = fabs(myHorDir->angle(aDir));
    double aVertAngle = fabs(myVertDir->angle(aDir));
    if (aHorAngle > M_PI/2.)
      aHorAngle = M_PI - aHorAngle;
    if (aVertAngle > M_PI/2.)
      aVertAngle = M_PI - aVertAngle;

    double aTolerance = Config_PropManager::real(SKETCH_TAB_NAME, "angular_tolerance");
    CompositeFeaturePtr aSketch = module()->sketchMgr()->activeSketch();
    FeaturePtr aFeature;
    if (aHorAngle < aTolerance)
      // Add horizontal constraint
      aFeature = aSketch->addFeature(SketchPlugin_ConstraintHorizontal::ID());
    else if (aVertAngle < aTolerance)
      // Add vertical constraint
      aFeature = aSketch->addFeature(SketchPlugin_ConstraintVertical::ID());

    if (aFeature.get()) {
      aFeature->refattr(SketchPlugin_Constraint::ENTITY_A())->setObject(
          theFeature->firstResult());
    }
  }
}
