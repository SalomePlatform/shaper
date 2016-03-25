// Copyright (C) 2014-20xx CEA/DEN, EDF R&D

// File:        PartSet_SketcherMgr.cpp
// Created:     19 Dec 2014
// Author:      Vitaly SMETANNIKOV

#include "PartSet_SketcherMgr.h"
#include "PartSet_SketcherReetntrantMgr.h"
#include "PartSet_Module.h"
#include "PartSet_WidgetPoint2d.h"
#include "PartSet_WidgetPoint2dDistance.h"
#include "PartSet_Tools.h"
#include "PartSet_WidgetSketchLabel.h"
#include "PartSet_WidgetEditor.h"
#include "PartSet_ResultSketchPrs.h"

#include <XGUI_ModuleConnector.h>
#include <XGUI_Displayer.h>
#include <XGUI_Workshop.h>
#include <XGUI_ContextMenuMgr.h>
#include <XGUI_Selection.h>
#include <XGUI_SelectionMgr.h>
#include <XGUI_ModuleConnector.h>
#include <XGUI_PropertyPanel.h>
#include <XGUI_ViewerProxy.h>
#include <XGUI_OperationMgr.h>
#include <XGUI_Tools.h>

#include <ModuleBase_IPropertyPanel.h>
#include <ModuleBase_ISelection.h>
#include <ModuleBase_IViewer.h>
#include <ModuleBase_IWorkshop.h>
#include <ModuleBase_IViewWindow.h>
#include <ModuleBase_ModelWidget.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_OperationFeature.h>
#include <ModuleBase_Operation.h>
#include <ModuleBase_WidgetEditor.h>

#include <GeomDataAPI_Point2D.h>

#include <Events_Loop.h>

#include <SketchPlugin_Line.h>
#include <SketchPlugin_Sketch.h>
#include <SketchPlugin_Point.h>
#include <SketchPlugin_Arc.h>
#include <SketchPlugin_Circle.h>
#include <SketchPlugin_ConstraintLength.h>
#include <SketchPlugin_ConstraintDistance.h>
#include <SketchPlugin_ConstraintParallel.h>
#include <SketchPlugin_ConstraintPerpendicular.h>
#include <SketchPlugin_ConstraintRadius.h>
#include <SketchPlugin_ConstraintRigid.h>
#include <SketchPlugin_ConstraintHorizontal.h>
#include <SketchPlugin_ConstraintVertical.h>
#include <SketchPlugin_ConstraintEqual.h>
#include <SketchPlugin_ConstraintTangent.h>
#include <SketchPlugin_ConstraintCoincidence.h>
#include <SketchPlugin_ConstraintFillet.h>
#include <SketchPlugin_ConstraintMirror.h>
#include <SketchPlugin_ConstraintAngle.h>
#include <SketchPlugin_ConstraintCollinear.h>
#include <SketchPlugin_ConstraintMiddle.h>
#include <SketchPlugin_MultiRotation.h>
#include <SketchPlugin_MultiTranslation.h>
#include <SketchPlugin_IntersectionPoint.h>

#include <SketcherPrs_Tools.h>

#include <SelectMgr_IndexedMapOfOwner.hxx>
#include <StdSelect_BRepOwner.hxx>

//#include <AIS_DimensionSelectionMode.hxx>
#include <AIS_Shape.hxx>
#include <AIS_Dimension.hxx>

#include <ModelAPI_Events.h>
#include <ModelAPI_Session.h>
#include <ModelAPI_AttributeString.h>

#include <QMouseEvent>
#include <QApplication>
#include <QCursor>
#include <QMessageBox>

//#define DEBUG_DO_NOT_BY_ENTER

//#define DEBUG_CURSOR

/// Returns list of unique objects by sum of objects from List1 and List2
/*QList<ModuleBase_ViewerPrs> getSumList(const QList<ModuleBase_ViewerPrs>& theList1,
                                       const QList<ModuleBase_ViewerPrs>& theList2)
{
  QList<ModuleBase_ViewerPrs> aRes;
  foreach (ModuleBase_ViewerPrs aPrs, theList1) {
    if (!aRes.contains(aPrs))
      aRes.append(aPrs);
  }
  foreach (ModuleBase_ViewerPrs aPrs, theList2) {
    if (!aRes.contains(aPrs))
      aRes.append(aPrs);
  }
  return aRes;
}*/

// Fills the list of features the list of selected presentations.
// \param theList a list of selected presentations
// \param theSketch a sketch to project a vertex shape of a presentation to the plane
// and find the corresponded attribute
// \param theFeatureList  an output list of features
void fillFeatureList(const QList<ModuleBase_ViewerPrs>& theList,
                     const FeaturePtr theSketch,
                     QList<FeaturePtr>& theFeatureList)
{
  QList<ModuleBase_ViewerPrs> aRes;

  QList<ModuleBase_ViewerPrs>::const_iterator anIt = theList.begin(),
                                              aLast = theList.end();
  for (; anIt != aLast; anIt++)
  {
    ModuleBase_ViewerPrs aPrs = *anIt;
    FeaturePtr aFeature = ModelAPI_Feature::feature(aPrs.object());
    if (aFeature.get()  && !theFeatureList.contains(aFeature))
      theFeatureList.append(aFeature);
  }
}

/// Fills attribute and result lists by the selected owner. In case if the attribute is found,
/// by the owner shape, it is put to the list. Otherwise if type of owner shape is edge, put the function
/// result as is to the list of results.
/// \param theOwner a viewer selected owner
/// \param theFeature a feature, where the attribute is searched
/// \param theSketch a current sketch
/// \param theSelectedAttribute an output list of attributes
/// \param theSelectedResults an output list of edge results
void getAttributesOrResults(const Handle(SelectMgr_EntityOwner)& theOwner,
                            const FeaturePtr& theFeature, const FeaturePtr& theSketch,
                            const ResultPtr& theResult,
                            std::set<AttributePtr>& aSelectedAttributes,
                            std::set<ResultPtr>& aSelectedResults)
{
  Handle(StdSelect_BRepOwner) aBRepOwner = Handle(StdSelect_BRepOwner)::DownCast(theOwner);
  if (aBRepOwner.IsNull())
    return;
  Handle(AIS_InteractiveObject) anIO = Handle(AIS_InteractiveObject)::DownCast(
                                                                    aBRepOwner->Selectable());
  if (aBRepOwner->HasShape()) {
    const TopoDS_Shape& aShape = aBRepOwner->Shape();
    TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
    if (aShapeType == TopAbs_VERTEX) {
      AttributePtr aPntAttr = PartSet_Tools::findAttributeBy2dPoint(theFeature,
                                                                    aShape, theSketch);
      if (aPntAttr.get() != NULL)
        aSelectedAttributes.insert(aPntAttr);
    }
    else if (aShapeType == TopAbs_EDGE &&
             aSelectedResults.find(theResult) == aSelectedResults.end()) {
      aSelectedResults.insert(theResult);
    }
  }
}

PartSet_SketcherMgr::PartSet_SketcherMgr(PartSet_Module* theModule)
  : QObject(theModule), myModule(theModule), myIsDragging(false), myDragDone(false),
    myIsMouseOverWindow(false),
    myIsMouseOverViewProcessed(true), myPreviousUpdateViewerEnabled(true),
    myIsPopupMenuActive(false)
{
  ModuleBase_IWorkshop* anIWorkshop = myModule->workshop();
  ModuleBase_IViewer* aViewer = anIWorkshop->viewer();

  myPreviousDrawModeEnabled = true;//aViewer->isSelectionEnabled();

  connect(aViewer, SIGNAL(mousePress(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMousePressed(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseRelease(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseReleased(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseMove(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseMoved(ModuleBase_IViewWindow*, QMouseEvent*)));

  connect(aViewer, SIGNAL(mouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)),
          this, SLOT(onMouseDoubleClick(ModuleBase_IViewWindow*, QMouseEvent*)));

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(anIWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  connect(aWorkshop, SIGNAL(applicationStarted()), this, SLOT(onApplicationStarted()));

  myIsConstraintsShown[PartSet_Tools::Geometrical] = true;
  myIsConstraintsShown[PartSet_Tools::Dimensional] = true;
  myIsConstraintsShown[PartSet_Tools::Expressions] = false;
}

PartSet_SketcherMgr::~PartSet_SketcherMgr()
{
  if (!myPlaneFilter.IsNull())
    myPlaneFilter.Nullify();
}

void PartSet_SketcherMgr::onEnterViewPort()
{
  // 1. if the mouse over window, update the next flag. Do not perform update visibility of
  // created feature because it should be done in onMouseMove(). Some widgets watch
  // the mouse move and use the cursor position to update own values. If the presentaion is
  // redisplayed before this update, the feature presentation jumps from reset value to current.
  myIsMouseOverWindow = true;

  #ifdef DEBUG_DO_NOT_BY_ENTER
  return;
  #endif

  if (canChangeCursor(getCurrentOperation())) {
    QCursor* aCurrentCursor = QApplication::overrideCursor();
    if (!aCurrentCursor || aCurrentCursor->shape() != Qt::CrossCursor) {
      QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
#ifdef DEBUG_CURSOR
      qDebug("onEnterViewPort() : Qt::CrossCursor");
#endif
    }
  }

  if (!isNestedCreateOperation(getCurrentOperation()))
    return;

  operationMgr()->onValidateOperation();

  // we need change displayed state of the current operation feature
  // if the feature is presentable, e.g. distance construction. It has no results, so workshop does
  // not accept a signal about the result created. Nothing is shown until mouse is moved out/in view
  // port. If the isDisplayed flag is true, the presentable feature is displayed as soon as the
  // presentation becomes valid and redisplay happens
  //ModuleBase_Operation* aOperation = getCurrentOperation();
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                           (getCurrentOperation());
  if (aFOperation) {
    FeaturePtr aFeature = aFOperation->feature();
    if (aFeature.get() && aFeature->data()->isValid()) {
      visualizeFeature(aFeature, aFOperation->isEditOperation(), canDisplayObject(aFeature), false);
    }
  }
}

void PartSet_SketcherMgr::onLeaveViewPort()
{
  myIsMouseOverViewProcessed = false;
  myIsMouseOverWindow = false;
  // it is important to validate operation here only if sketch entity create operation is active
  // because at this operation we reacts to the mouse leave/enter view port
  //operationMgr()->onValidateOperation();

  #ifdef DEBUG_DO_NOT_BY_ENTER
  return;
  #endif

  if (canChangeCursor(getCurrentOperation())) {
    QApplication::restoreOverrideCursor();
#ifdef DEBUG_CURSOR
    qDebug("onLeaveViewPort() : None");
#endif
  }

  if (!isNestedCreateOperation(getCurrentOperation()))
    return;

  // the method should be performed if the popup menu is called,
  // the reset of the current widget should not happen
  if (myIsPopupMenuActive)
    return;

  operationMgr()->onValidateOperation();

  // 2. if the mouse IS NOT over window, reset the active widget value and hide the presentation
  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
  // disable the viewer update in order to avoid visualization of redisplayed feature in viewer
  // obtained after reset value
  bool isEnableUpdateViewer = aDisplayer->enableUpdateViewer(false);
  ModuleBase_ModelWidget* anActiveWidget = getActiveWidget();
  if (anActiveWidget)
    anActiveWidget->reset();

  // hides the presentation of the current operation feature
  // the feature is to be erased here, but it is correct to call canDisplayObject because
  // there can be additional check (e.g. editor widget in distance constraint)
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                           (getCurrentOperation());
  if (aFOperation) {
    FeaturePtr aFeature = aFOperation->feature();
    visualizeFeature(aFeature, aFOperation->isEditOperation(), canDisplayObject(aFeature));
  }
  // we should update viewer after the presentation are hidden in the viewer
  // otherwise the reset presentation(line) appears in the viewer(by quick move from viewer to PP)
  aDisplayer->enableUpdateViewer(isEnableUpdateViewer);
}

void PartSet_SketcherMgr::onBeforeValuesChangedInPropertyPanel()
{
  if (!isNestedEditOperation(getCurrentOperation()) ||
      myModule->sketchReentranceMgr()->isInternalEditActive())
    return;
  // it is necessary to save current selection in order to restore it after the values are modifed
  storeSelection();

  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
  myPreviousUpdateViewerEnabled = aDisplayer->enableUpdateViewer(false);
}

void PartSet_SketcherMgr::onAfterValuesChangedInPropertyPanel()
{
  if (!isNestedEditOperation(getCurrentOperation()) ||
      myModule->sketchReentranceMgr()->isInternalEditActive()) {
    myModule->sketchReentranceMgr()->updateInternalEditActiveState();
    return;
  }
  // it is necessary to restore current selection in order to restore it after the values are modified
  restoreSelection();
  myCurrentSelection.clear();

  // 3. the flag to disable the update viewer should be set in order to avoid blinking in the 
  // viewer happens by deselect/select the modified objects. The flag should be restored after
  // the selection processing. The update viewer should be also called.
  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
  aDisplayer->enableUpdateViewer(myPreviousUpdateViewerEnabled);
  aDisplayer->updateViewer();


}

void PartSet_SketcherMgr::onMousePressed(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myModule->sketchReentranceMgr()->processMousePressed(theWnd, theEvent))
    return;

  //get2dPoint(theWnd, theEvent, myClickedPoint);

  if (!(theEvent->buttons() & Qt::LeftButton))
    return;

  // Clear dragging mode
  myIsDragging = false;

  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  ModuleBase_IViewer* aViewer = aWorkshop->viewer();
  if (!aViewer->canDragByMouse())
    return;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                               (getCurrentOperation());
  if (!aFOperation)
    return;

  if (aFOperation->isEditOperation()) {
    // If the current widget is a selector, do nothing, it processes the mouse press
    ModuleBase_ModelWidget* anActiveWidget = getActiveWidget();
    if(anActiveWidget && anActiveWidget->isViewerSelector()) {
      return;
    }
  }

  // Use only for sketch operations
  if (myCurrentSketch) {
    if (!PartSet_Tools::sketchPlane(myCurrentSketch))
      return;

    bool isSketcher = isSketchOperation(aFOperation);
    bool isSketchOpe = isNestedSketchOperation(aFOperation);

    // Avoid non-sketch operations
    if ((!isSketchOpe) && (!isSketcher))
      return;

    bool isEditing = aFOperation->isEditOperation();

    // Ignore creation sketch operation
    if ((!isSketcher) && (!isEditing))
      return;

    Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();
    if (!aContext.IsNull()) {
      // MoveTo in order to highlight current object
      aContext->MoveTo(theEvent->x(), theEvent->y(), theWnd->v3dView());
    }
    // Remember highlighted objects for editing
    ModuleBase_ISelection* aSelect = aWorkshop->selection();

    bool aHasShift = (theEvent->modifiers() & Qt::ShiftModifier);
    storeSelection(!aHasShift);

    if (myCurrentSelection.empty()) {
      if (isSketchOpe && (!isSketcher))
        // commit previous operation
        if (!aFOperation->commit())
          aFOperation->abort();
      return;
    }
    // Init flyout point for radius rotation
    FeaturePtr aFeature = myCurrentSelection.begin().key();

    if (isSketcher) {
      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurrentPoint);
      myDragDone = false;

      myPreviousDrawModeEnabled = aViewer->enableDrawMode(false);
      launchEditing();
      if (aFeature.get() != NULL) {
        std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
                  std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
      if (aSPFeature.get() && aSPFeature->getKind() == SketchPlugin_ConstraintRadius::ID()) {
          DataPtr aData = aSPFeature->data();
          AttributePtr aAttr = aData->attribute(SketchPlugin_Constraint::FLYOUT_VALUE_PNT());
          std::shared_ptr<GeomDataAPI_Point2D> aFPAttr = 
            std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aAttr);
          aFPAttr->setValue(myCurrentPoint.myCurX, myCurrentPoint.myCurY);
        }
      }
    } else if (isSketchOpe && isEditing) {
      // If selected another object commit current result
      aFOperation->commit();

      myIsDragging = true;
      get2dPoint(theWnd, theEvent, myCurrentPoint);
      myDragDone = false;

      myPreviousDrawModeEnabled = aViewer->enableDrawMode(false);

      // this is temporary commented in order to avoid the following wrong case:
      // Distance constraint is under edition, double click on the digit -> nothing happens
      // because QApplication::processEvents() calls onMouseDoubleClick, which try to show editor
      // but as the prev edit is commited an new one is not started, editor is not shown.
      // This is necessary in order to finalize previous operation
      //QApplication::processEvents();
      launchEditing();
    }
  }
}

void PartSet_SketcherMgr::onMouseReleased(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myModule->sketchReentranceMgr()->processMouseReleased(theWnd, theEvent))
    return;

  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  ModuleBase_IViewer* aViewer = aWorkshop->viewer();
  if (!aViewer->canDragByMouse())
    return;
  ModuleBase_Operation* aOp = getCurrentOperation();
  if (aOp) {
    if (isNestedSketchOperation(aOp)) {
      //get2dPoint(theWnd, theEvent, myClickedPoint);

      // Only for sketcher operations
      if (myIsDragging) {
        if (myDragDone) {
          //aOp->commit();
          myCurrentSelection.clear();
          /*Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();
          if (!aContext.IsNull()) {
          // Reselect edited object
          aContext->MoveTo(theEvent->x(), theEvent->y(), theWnd->v3dView());
          if (theEvent->modifiers() & Qt::ShiftModifier)
            aContext->ShiftSelect();
          else
            aContext->Select();
          */
        }
      }
    }
  }

  aWorkshop->viewer()->enableDrawMode(myPreviousDrawModeEnabled);
  myIsDragging = false;
}

void PartSet_SketcherMgr::onMouseMoved(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  if (myModule->sketchReentranceMgr()->processMouseMoved(theWnd, theEvent))
    return;

  if (isNestedCreateOperation(getCurrentOperation()) && !myIsMouseOverViewProcessed) {
    myIsMouseOverViewProcessed = true;
    // 1. perform the widget mouse move functionality and display the presentation
    // the mouse move should be processed in the widget, if it can in order to visualize correct
    // presentation. These widgets correct the feature attribute according to the mouse position
    ModuleBase_ModelWidget* anActiveWidget = getActiveWidget();
    PartSet_WidgetPoint2D* aPoint2DWdg = dynamic_cast<PartSet_WidgetPoint2D*>(anActiveWidget);
    if (aPoint2DWdg) {
      aPoint2DWdg->onMouseMove(theWnd, theEvent);
    }
    PartSet_WidgetPoint2dDistance* aDistanceWdg = dynamic_cast<PartSet_WidgetPoint2dDistance*>
                                                                (anActiveWidget);
    if (aDistanceWdg) {
      aDistanceWdg->onMouseMove(theWnd, theEvent);
    }
    // the feature is to be erased here, but it is correct to call canDisplayObject because
    // there can be additional check (e.g. editor widget in distance constraint)
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                             (getCurrentOperation());
    if (aFOperation) {
      FeaturePtr aFeature = aFOperation->feature();
      visualizeFeature(aFeature, aFOperation->isEditOperation(), canDisplayObject(aFeature));
    }
  }

  //myClickedPoint.clear();

  if (myIsDragging) {
    // 1. the current selection is saved in the mouse press method in order to restore it after moving
    // 2. the enable selection in the viewer should be temporary switched off in order to ignore
    // mouse press signal in the viewer(it call Select for AIS context and the dragged objects are
    // deselected). This flag should be restored in the slot, processed the mouse release signal.

    ModuleBase_Operation* aCurrentOperation = getCurrentOperation();
    if (!aCurrentOperation)
      return;
    if (isSketchOperation(aCurrentOperation))
      return; // No edit operation activated

    Handle(V3d_View) aView = theWnd->v3dView();
    gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
    double aX, aY;
    PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, aX, aY);
    double dX =  aX - myCurrentPoint.myCurX;
    double dY =  aY - myCurrentPoint.myCurY;

    ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
    XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
    // 3. the flag to disable the update viewer should be set in order to avoid blinking in the 
    // viewer happens by deselect/select the modified objects. The flag should be restored after
    // the selection processing. The update viewer should be also called.
    bool isEnableUpdateViewer = aDisplayer->enableUpdateViewer(false);

    static Events_ID aMoveEvent = Events_Loop::eventByName(EVENT_OBJECT_MOVED);
    //static Events_ID aUpdateEvent = Events_Loop::eventByName(EVENT_OBJECT_UPDATED);
    FeatureToSelectionMap::const_iterator anIt = myCurrentSelection.begin(),
                                          aLast = myCurrentSelection.end();
    // 4. the features and attributes modification(move)
    bool isModified = false;
    for (; anIt != aLast; anIt++) {
      FeaturePtr aFeature = anIt.key();

      std::set<AttributePtr> anAttributes = anIt.value().first;
      // Process selection by attribute: the priority to the attribute
      if (!anAttributes.empty()) {
        std::set<AttributePtr>::const_iterator anAttIt = anAttributes.begin(),
                                               anAttLast = anAttributes.end();
        for (; anAttIt != anAttLast; anAttIt++) {
          AttributePtr anAttr = *anAttIt;
          if (anAttr.get() == NULL)
            continue;
          std::string aAttrId = anAttr->id();
          DataPtr aData = aFeature->data();
          if (aData->isValid()) {
            std::shared_ptr<GeomDataAPI_Point2D> aPoint = 
              std::dynamic_pointer_cast<GeomDataAPI_Point2D>(aData->attribute(aAttrId));
            if (aPoint.get() != NULL) {
              bool isImmutable = aPoint->setImmutable(true);
              aPoint->move(dX, dY);
              isModified = true;
              ModelAPI_EventCreator::get()->sendUpdated(aFeature, aMoveEvent);
              aPoint->setImmutable(isImmutable);
            }
          }
        }
      } else {
        // Process selection by feature
        std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
          std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
        if (aSketchFeature) {
          aSketchFeature->move(dX, dY);
          isModified = true;
          ModelAPI_EventCreator::get()->sendUpdated(aSketchFeature, aMoveEvent);
        }
      }
    }
    // the modified state of the current operation should be updated if there are features, which
    // were changed here
    if (isModified) {
      aCurrentOperation->onValuesChanged();
    }
    Events_Loop::loop()->flush(aMoveEvent); // up all move events - to be processed in the solver
    //Events_Loop::loop()->flush(aUpdateEvent); // up update events - to redisplay presentations

    // 5. it is necessary to save current selection in order to restore it after the features moving
    restoreSelection();
    // 6. restore the update viewer flag and call this update
    aDisplayer->enableUpdateViewer(isEnableUpdateViewer);
    aDisplayer->updateViewer();

    myDragDone = true;
    myCurrentPoint.setValue(aX, aY);
  }
}

void PartSet_SketcherMgr::onMouseDoubleClick(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                               (getCurrentOperation());
  if (aFOperation && aFOperation->isEditOperation()) {
    std::string aId = aFOperation->id().toStdString();
    if (isDistanceOperation(aFOperation))
    {
      // Activate dimension value editing on double click
      ModuleBase_IPropertyPanel* aPanel = aFOperation->propertyPanel();
      QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
      // Find corresponded widget to activate value editing
      foreach (ModuleBase_ModelWidget* aWgt, aWidgets) {
        if (aWgt->attributeID() == "ConstraintValue") {
          PartSet_WidgetEditor* anEditor = dynamic_cast<PartSet_WidgetEditor*>(aWgt);
          if (anEditor)
            anEditor->showPopupEditor();
          return;
        }
      }
    }
  }
}

void PartSet_SketcherMgr::onApplicationStarted()
{
  ModuleBase_IWorkshop* anIWorkshop = myModule->workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(anIWorkshop);
  XGUI_Workshop* aWorkshop = aConnector->workshop();
  PartSet_SketcherReetntrantMgr* aReentranceMgr = myModule->sketchReentranceMgr();

  XGUI_PropertyPanel* aPropertyPanel = aWorkshop->propertyPanel();
  if (aPropertyPanel) {
    //connect(aPropertyPanel, SIGNAL(beforeWidgetActivated(ModuleBase_ModelWidget*)),
    //        this, SLOT(onBeforeWidgetActivated(ModuleBase_ModelWidget*)));

    connect(aPropertyPanel, SIGNAL(noMoreWidgets(const std::string&)),
            aReentranceMgr, SLOT(onNoMoreWidgets(const std::string&)));
    connect(aPropertyPanel, SIGNAL(widgetActivated(ModuleBase_ModelWidget*)),
            aReentranceMgr, SLOT(onWidgetActivated()));
  }

  XGUI_ViewerProxy* aViewerProxy = aWorkshop->viewer();
  connect(aViewerProxy, SIGNAL(enterViewPort()), this, SLOT(onEnterViewPort()));
  connect(aViewerProxy, SIGNAL(leaveViewPort()), this, SLOT(onLeaveViewPort()));

  XGUI_ContextMenuMgr* aContextMenuMgr = aWorkshop->contextMenuMgr();
  connect(aContextMenuMgr, SIGNAL(beforeContextMenu()), this, SLOT(onBeforeContextMenu()));
  connect(aContextMenuMgr, SIGNAL(afterContextMenu()), this, SLOT(onAfterContextMenu()));
}

//void PartSet_SketcherMgr::onBeforeWidgetActivated(ModuleBase_ModelWidget* theWidget)
//{
  //if (!myClickedPoint.myIsInitialized)
  //  return;

  //ModuleBase_Operation* aOperation = getCurrentOperation();
  // the distance constraint feature should not use the clickedd point
  // this is workaround in order to don't throw down the flyout point value,
  // set by execute() method of these type of features
  //if (isDistanceOperation(aOperation))
  //  return;

  //PartSet_WidgetPoint2D* aPnt2dWgt = dynamic_cast<PartSet_WidgetPoint2D*>(theWidget);
  //if (aPnt2dWgt) {
  //  aPnt2dWgt->setPoint(myClickedPoint.myCurX, myClickedPoint.myCurY);
  //}
//}

void PartSet_SketcherMgr::onBeforeContextMenu()
{
  myIsPopupMenuActive = true;
}

void PartSet_SketcherMgr::onAfterContextMenu()
{
  myIsPopupMenuActive = false;
}

void PartSet_SketcherMgr::get2dPoint(ModuleBase_IViewWindow* theWnd, QMouseEvent* theEvent, 
                                     Point& thePoint)
{
  Handle(V3d_View) aView = theWnd->v3dView();
  gp_Pnt aPoint = PartSet_Tools::convertClickToPoint(theEvent->pos(), aView);
  double aX, anY;
  PartSet_Tools::convertTo2D(aPoint, myCurrentSketch, aView, aX, anY);
  thePoint.setValue(aX, anY);
}

void PartSet_SketcherMgr::launchEditing()
{
  if (!myCurrentSelection.empty()) {
    FeaturePtr aFeature = myCurrentSelection.begin().key();
    std::shared_ptr<SketchPlugin_Feature> aSPFeature = 
              std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
    if (aSPFeature) {
      myModule->editFeature(aSPFeature);
    }
  }
}

bool PartSet_SketcherMgr::sketchSolverError()
{
  bool anError = false;
  CompositeFeaturePtr aSketch = activeSketch();
  if (aSketch.get()) {
    AttributeStringPtr aAttributeString = aSketch->string(SketchPlugin_Sketch::SOLVER_ERROR());
    anError = !aAttributeString->value().empty();
  }
  return anError;
}

QString PartSet_SketcherMgr::getFeatureError(const FeaturePtr& theFeature)
{
  QString anError = "";
  if (!theFeature.get() || !theFeature->data()->isValid())
    return anError;

  CompositeFeaturePtr aSketch = activeSketch();
  if (aSketch.get() && aSketch == theFeature) {
    AttributeStringPtr aAttributeString = aSketch->string(SketchPlugin_Sketch::SOLVER_ERROR());
    anError = aAttributeString->value().c_str();
  }
  return anError;
}

void PartSet_SketcherMgr::clearClickedFlags()
{
  //myClickedPoint.clear();
  myCurrentPoint.clear();
}

const QStringList& PartSet_SketcherMgr::sketchOperationIdList()
{
  static QStringList aIds;
  if (aIds.size() == 0) {
    aIds << SketchPlugin_Line::ID().c_str();
    aIds << SketchPlugin_Point::ID().c_str();
    aIds << SketchPlugin_Arc::ID().c_str();
    aIds << SketchPlugin_Circle::ID().c_str();
    aIds << SketchPlugin_ConstraintFillet::ID().c_str();
    aIds << SketchPlugin_IntersectionPoint::ID().c_str();
    // TODO
    // SketchRectangle is a python feature, so its ID is passed just as a string
    aIds << "SketchRectangle";
    aIds.append(constraintsIdList());
  }
  return aIds;
}

const QStringList& PartSet_SketcherMgr::constraintsIdList()
{
  static QStringList aIds;
  if (aIds.size() == 0) {
    aIds << SketchPlugin_ConstraintLength::ID().c_str();
    aIds << SketchPlugin_ConstraintDistance::ID().c_str();
    aIds << SketchPlugin_ConstraintRigid::ID().c_str();
    aIds << SketchPlugin_ConstraintRadius::ID().c_str();
    aIds << SketchPlugin_ConstraintPerpendicular::ID().c_str();
    aIds << SketchPlugin_ConstraintParallel::ID().c_str();
    aIds << SketchPlugin_ConstraintHorizontal::ID().c_str();
    aIds << SketchPlugin_ConstraintVertical::ID().c_str();
    aIds << SketchPlugin_ConstraintEqual::ID().c_str();
    aIds << SketchPlugin_ConstraintTangent::ID().c_str();
    aIds << SketchPlugin_ConstraintCoincidence::ID().c_str();
    aIds << SketchPlugin_ConstraintMirror::ID().c_str();
    aIds << SketchPlugin_ConstraintAngle::ID().c_str();
    aIds << SketchPlugin_MultiRotation::ID().c_str();
    aIds << SketchPlugin_MultiTranslation::ID().c_str();
    aIds << SketchPlugin_ConstraintCollinear::ID().c_str();
    aIds << SketchPlugin_ConstraintMiddle::ID().c_str();
  }
  return aIds;
}

void PartSet_SketcherMgr::sketchSelectionModes(QIntList& theModes)
{
  theModes.clear();

  theModes.append(SketcherPrs_Tools::Sel_Dimension_Text);
  theModes.append(SketcherPrs_Tools::Sel_Dimension_Line);
  theModes.append(SketcherPrs_Tools::Sel_Constraint);
  theModes.append(TopAbs_VERTEX);
  theModes.append(TopAbs_EDGE);
}

Handle(AIS_InteractiveObject) PartSet_SketcherMgr::createPresentation(const ResultPtr& theResult)
{
  Handle(AIS_InteractiveObject) aPrs;

  FeaturePtr aFeature = ModelAPI_Feature::feature(theResult);
  if (aFeature.get() && aFeature->getKind() == SketchPlugin_Sketch::ID()) {
    aPrs = new PartSet_ResultSketchPrs(theResult);
  }
  return aPrs;
}

bool PartSet_SketcherMgr::isSketchOperation(ModuleBase_Operation* theOperation)
{
  return theOperation && theOperation->id().toStdString() == SketchPlugin_Sketch::ID();
}

bool PartSet_SketcherMgr::isNestedSketchOperation(ModuleBase_Operation* theOperation)
{
  return theOperation &&
         PartSet_SketcherMgr::sketchOperationIdList().contains(theOperation->id());
}

bool PartSet_SketcherMgr::isNestedCreateOperation(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                               (theOperation);
  return aFOperation && !aFOperation->isEditOperation() && isNestedSketchOperation(aFOperation);
}

bool PartSet_SketcherMgr::isNestedEditOperation(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                               (theOperation);
  return aFOperation && aFOperation->isEditOperation() && isNestedSketchOperation(aFOperation);
}

bool PartSet_SketcherMgr::isEntity(const std::string& theId)
{
  return (theId == SketchPlugin_Line::ID()) ||
         (theId == SketchPlugin_Point::ID()) ||
         (theId == SketchPlugin_Arc::ID()) ||
         (theId == SketchPlugin_Circle::ID());
}

bool PartSet_SketcherMgr::isDistanceOperation(ModuleBase_Operation* theOperation)
{
  std::string anId = theOperation ? theOperation->id().toStdString() : "";

  return isDistanceKind(anId);
}

bool PartSet_SketcherMgr::isDistanceKind(std::string& theKind)
{
  return (theKind == SketchPlugin_ConstraintLength::ID()) ||
         (theKind == SketchPlugin_ConstraintDistance::ID()) ||
         (theKind == SketchPlugin_ConstraintRadius::ID()) ||
         (theKind == SketchPlugin_ConstraintAngle::ID());
}

void PartSet_SketcherMgr::startSketch(ModuleBase_Operation* theOperation)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                               (getCurrentOperation());
  if (!aFOperation)
    return;

  myModule->onViewTransformed();

  // Display all sketcher sub-Objects
  myCurrentSketch = std::dynamic_pointer_cast<ModelAPI_CompositeFeature>(aFOperation->feature());
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());
  aConnector->workshop()->displayer()->activateTrihedron(true);

  // Hide sketcher result
  std::list<ResultPtr> aResults = myCurrentSketch->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    (*aIt)->setDisplayed(false);
  }
  myCurrentSketch->setDisplayed(false);

  // Display sketcher objects
  for (int i = 0; i < myCurrentSketch->numberOfSubs(); i++) {
    FeaturePtr aFeature = myCurrentSketch->subFeature(i);
    std::list<ResultPtr> aResults = aFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      (*aIt)->setDisplayed(true);
    }
    aFeature->setDisplayed(true);
  }

  if(myCirclePointFilter.IsNull()) {
    myCirclePointFilter = new PartSet_CirclePointFilter(myModule->workshop());
  }

  myModule->workshop()->viewer()->addSelectionFilter(myCirclePointFilter);

  if (myPlaneFilter.IsNull()) 
    myPlaneFilter = new ModuleBase_ShapeInPlaneFilter();

  myModule->workshop()->viewer()->addSelectionFilter(myPlaneFilter);
  bool aHasPlane = false;
  std::shared_ptr<GeomAPI_Pln> aPln;
  if (aFOperation->isEditOperation()) {
    // If it is editing of sketch then it means that plane is already defined
    aPln = PartSet_Tools::sketchPlane(myCurrentSketch);
    if (aPln.get())
      aHasPlane = true;
  }
  myPlaneFilter->setPlane(aPln);

  Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
  // all sketch objects should be activated in the sketch selection modes by edit operation start
  // in case of creation operation, there is an active widget, which activates own selection mode
  if (aFOperation->isEditOperation() && aHasPlane)
    aConnector->activateModuleSelectionModes();
}

void PartSet_SketcherMgr::stopSketch(ModuleBase_Operation* theOperation)
{
  myIsMouseOverWindow = false;
  myIsConstraintsShown[PartSet_Tools::Geometrical] = true;
  myIsConstraintsShown[PartSet_Tools::Dimensional] = true;
  myIsConstraintsShown[PartSet_Tools::Expressions] = false;

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(myModule->workshop());

  DataPtr aData = myCurrentSketch->data();
  if (!aData->isValid()) {
    XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();
    // The sketch was aborted
    myCurrentSketch = CompositeFeaturePtr();
    // TODO: move this outside of if-else
    myModule->workshop()->viewer()->removeSelectionFilter(myCirclePointFilter);
    myModule->workshop()->viewer()->removeSelectionFilter(myPlaneFilter);

    // Erase all sketcher objects
    QStringList aSketchIds = sketchOperationIdList();
    QObjectPtrList aObjects = aDisplayer->displayedObjects();
    foreach (ObjectPtr aObj, aObjects) {
      DataPtr aObjData = aObj->data();
      if (!aObjData->isValid())
        aObj->setDisplayed(false);
    }
  }
  else {
    // Hide all sketcher sub-Objects
    for (int i = 0; i < myCurrentSketch->numberOfSubs(); i++) {
      FeaturePtr aFeature = myCurrentSketch->subFeature(i);
      std::list<ResultPtr> aResults = aFeature->results();
      std::list<ResultPtr>::const_iterator aIt;
      for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
        (*aIt)->setDisplayed(false);
      }
      aFeature->setDisplayed(false);
    }
    // Display sketcher result
    std::list<ResultPtr> aResults = myCurrentSketch->results();
    std::list<ResultPtr>::const_iterator aIt;
    Events_Loop* aLoop = Events_Loop::loop();
    static Events_ID aDispEvent = aLoop->eventByName(EVENT_OBJECT_TO_REDISPLAY);

    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                           (theOperation);
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
      if (!aFOperation->isDisplayedOnStart(*aIt)) {
        (*aIt)->setDisplayed(true);
        // this display event is needed because sketch already may have "displayed" state,
        // but not displayed while it is still active (issue 613, abort of existing sketch)
        ModelAPI_EventCreator::get()->sendUpdated(*aIt, aDispEvent);
      }
    }
    if (!aFOperation->isDisplayedOnStart(myCurrentSketch))
      myCurrentSketch->setDisplayed(true);
    
    myCurrentSketch = CompositeFeaturePtr();

    myModule->workshop()->viewer()->removeSelectionFilter(myCirclePointFilter);
    myModule->workshop()->viewer()->removeSelectionFilter(myPlaneFilter);

    Events_Loop::loop()->flush(aDispEvent);
  }
  // restore the module selection modes, which were changed on startSketch
  aConnector->activateModuleSelectionModes();
  aConnector->workshop()->displayer()->activateTrihedron(false);
}

void PartSet_SketcherMgr::startNestedSketch(ModuleBase_Operation* theOperation)
{
  if (canChangeCursor(theOperation) && myIsMouseOverWindow) {
    QCursor* aCurrentCursor = QApplication::overrideCursor();
    if (!aCurrentCursor || aCurrentCursor->shape() != Qt::CrossCursor) {
      QApplication::setOverrideCursor(QCursor(Qt::CrossCursor));
#ifdef DEBUG_CURSOR
      qDebug("startNestedSketch() : Qt::CrossCursor");
#endif
    }
  }
}

void PartSet_SketcherMgr::stopNestedSketch(ModuleBase_Operation* theOperation)
{
  myIsMouseOverViewProcessed = true;
  operationMgr()->onValidateOperation();
  if (canChangeCursor(theOperation)) {
    QApplication::restoreOverrideCursor();
#ifdef DEBUG_CURSOR
    qDebug("stopNestedSketch() : None");
#endif
  }
}

void PartSet_SketcherMgr::commitNestedSketch(ModuleBase_Operation* theOperation)
{
  if (isNestedCreateOperation(theOperation)) {
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                             (theOperation);
    if (aFOperation) {
      FeaturePtr aFeature = aFOperation->feature();
      // it is necessary to check the the feature data validity because
      // some kind of features are removed by an operation commit(the macro state of a feature)
      if (aFeature.get() && aFeature->data()->isValid()) {
        visualizeFeature(aFeature, aFOperation->isEditOperation(), true);
      }
    }
  }
}

void PartSet_SketcherMgr::activatePlaneFilter(const bool& toActivate)
{
  if (toActivate)
    myModule->workshop()->viewer()->addSelectionFilter(myPlaneFilter);
  else
    myModule->workshop()->viewer()->removeSelectionFilter(myPlaneFilter);
}

bool PartSet_SketcherMgr::operationActivatedByPreselection()
{
  bool isOperationStopped = false;
  ModuleBase_Operation* anOperation = getCurrentOperation();
  if(anOperation && PartSet_SketcherMgr::isNestedSketchOperation(anOperation)) {
    // Set final definitions if they are necessary
    //propertyPanelDefined(aOperation);
    /// Commit sketcher operations automatically
    /// distance operation are able to show popup editor to modify the distance value
    /// after entering the value, the operation should be committed/aborted(by Esc key)
    bool aCanCommitOperation = true;
    ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                            (anOperation);
    if (aFOperation && PartSet_SketcherMgr::isDistanceOperation(aFOperation))
      aCanCommitOperation = setDistanceValueByPreselection(anOperation, myModule->workshop());

    if (aCanCommitOperation)
      isOperationStopped = anOperation->commit();
    else {
      anOperation->abort();
      isOperationStopped = true;
    }
  }
  return isOperationStopped;
}

bool PartSet_SketcherMgr::canUndo() const
{
  return isNestedCreateOperation(getCurrentOperation());
}

bool PartSet_SketcherMgr::canRedo() const
{
  return isNestedCreateOperation(getCurrentOperation());
}

bool PartSet_SketcherMgr::canEraseObject(const ObjectPtr& theObject) const
{
  bool aCanErase = true;
  // when the sketch operation is active, results of sketch sub-feature can not be hidden
  if (myCurrentSketch.get()) {
    ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(theObject);
    if (aResult.get()) {
      // Display sketcher objects
      for (int i = 0; i < myCurrentSketch->numberOfSubs() && aCanErase; i++) {

        FeaturePtr aFeature = myCurrentSketch->subFeature(i);
        std::list<ResultPtr> aResults = aFeature->results();
        std::list<ResultPtr>::const_iterator anIt;
        for (anIt = aResults.begin(); anIt != aResults.end() && aCanErase; ++anIt) {
          aCanErase = *anIt != aResult;
        }
      }
    }
  }
  return aCanErase;
}

bool PartSet_SketcherMgr::canDisplayObject(const ObjectPtr& theObject) const
{
  bool aCanDisplay = true;

  bool aHasActiveSketch = activeSketch().get() != NULL;
  if (aHasActiveSketch) {
    // 1. the sketch feature should not be displayed during the sketch active operation
    // it is hidden by a sketch operation start and shown by a sketch stop, just the sketch 
    // nested features can be visualized
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature.get() != NULL && aFeature == activeSketch()) {
      aCanDisplay = false;
    }
  }
  else { // there are no an active sketch
    // 2. sketch sub-features should not be visualized if the sketch operation is not active
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature.get() != NULL) {
      std::shared_ptr<SketchPlugin_Feature> aSketchFeature =
                              std::dynamic_pointer_cast<SketchPlugin_Feature>(aFeature);
      if (aSketchFeature.get()) {
        aCanDisplay = false;
      }
    }
  }

  // 3. the method should not filter the objects, which are not related to the current operation.
  // The object is filtered just if it is a current operation feature or this feature result
  bool isObjectFound = false;
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                               (getCurrentOperation());
  if (aFOperation) {
    FeaturePtr aFeature = aFOperation->feature();
    if (aFeature.get()) {
      std::list<ResultPtr> aResults = aFeature->results();
      if (theObject == aFeature)
        isObjectFound = true;
      else {
        std::list<ResultPtr>::const_iterator anIt = aResults.begin(), aLast = aResults.end();
        for (; anIt != aLast && !isObjectFound; anIt++) {
          isObjectFound = *anIt == theObject;
        }
      }
    }
  }
  if (isObjectFound) {
    // 4. For created nested feature operation do not display the created feature if
    // the mouse curstor leaves the OCC window.
    // The correction cases, which ignores this condition:
    // a. the property panel values modification
    // b. the popup menu activated
    // c. widget editor control
    #ifndef DEBUG_DO_NOT_BY_ENTER
    if (aCanDisplay && isNestedCreateOperation(getCurrentOperation())) {
      ModuleBase_ModelWidget* anActiveWidget = getActiveWidget();
      ModuleBase_WidgetEditor* anEditorWdg = anActiveWidget ? dynamic_cast<ModuleBase_WidgetEditor*>(anActiveWidget) : 0;
      // the active widget editor should not influence here. The presentation should be visible always
      // when this widget is active.
      if (!anEditorWdg && !myIsPopupMenuActive) {
        // during a nested create operation, the feature is redisplayed only if the mouse over view
        // of there was a value modified in the property panel after the mouse left the view
        aCanDisplay = canDisplayCurrentCreatedFeature();
      }
    }
    #endif
  }

  // checks the sketcher constraints visibility according to active sketch check box states
  if (aCanDisplay) {
    bool aProcessed = false;
    FeaturePtr aFeature = ModelAPI_Feature::feature(theObject);
    if (aFeature.get()) {
      bool aConstraintDisplayed = canDisplayConstraint(aFeature, PartSet_Tools::Any, aProcessed);
      if (aProcessed)
        aCanDisplay = aConstraintDisplayed;
    }
  }

  return aCanDisplay;
}

bool PartSet_SketcherMgr::canDisplayConstraint(const FeaturePtr& theFeature,
                                             const PartSet_Tools::ConstraintVisibleState& theState,
                                             bool& isProcessed) const
{
  bool aSwitchedOn = true;

  const QStringList& aConstrIds = constraintsIdList();

  std::string aKind = theFeature->getKind();
  if (aConstrIds.contains(QString(aKind.c_str()))) {
    bool isTypedConstraint = false;

    switch (theState) {
      case PartSet_Tools::Dimensional: {
        bool isDistance = isDistanceKind(aKind);
        if (isDistance) {
          isProcessed = true;
          aSwitchedOn = myIsConstraintsShown[theState];
        }
      }
      break;
      case PartSet_Tools::Geometrical: {
        bool isGeometrical = !isDistanceKind(aKind);
        if (isGeometrical) {
          isProcessed = true;
          aSwitchedOn = myIsConstraintsShown[theState];
        }
      }
      break;
      case PartSet_Tools::Any: {
        isProcessed = true;
        bool isDistance = isDistanceKind(aKind);
        if (isDistance)
          aSwitchedOn = myIsConstraintsShown[PartSet_Tools::Dimensional];
        else
          aSwitchedOn = myIsConstraintsShown[PartSet_Tools::Geometrical];
      }
      break;
    default:
      break;
    }
  }
  return aSwitchedOn;
}

void PartSet_SketcherMgr::processHiddenObject(const std::list<ObjectPtr>& theObjects)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                           (getCurrentOperation());
  if (aFOperation && myCurrentSketch.get()) {
    // find results of the current operation
    // these results should not be proposed to be deleted
    FeaturePtr anOperationFeature = aFOperation->feature();
    std::list<ResultPtr> anOperationResultList = anOperationFeature->results();
    std::set<ResultPtr> anOperationResults;
    std::list<ResultPtr>::const_iterator aRIt = anOperationResultList.begin(),
                                        aRLast = anOperationResultList.end();
    for (; aRIt != aRLast; aRIt++)
      anOperationResults.insert(*aRIt);

    std::set<FeaturePtr> anObjectsToBeDeleted;
    QStringList anObjectsToBeDeletedNames;
    std::list<ObjectPtr>::const_iterator anIt = theObjects.begin(), aLast = theObjects.end();
    for (; anIt != aLast; anIt++) {
      ObjectPtr anObject = *anIt;
      bool aCanErase = true;
      // when the sketch operation is active, results of sketch sub-feature can not be hidden
      ResultPtr aResult = std::dynamic_pointer_cast<ModelAPI_Result>(anObject);
      // the result is found between current feature results
      if (anOperationResults.find(aResult) != anOperationResults.end())
        continue;

      if (aResult.get()) {
        // Display sketcher objects
        for (int i = 0; i < myCurrentSketch->numberOfSubs() && aCanErase; i++) {
          FeaturePtr aFeature = myCurrentSketch->subFeature(i);
          std::list<ResultPtr> aResults = aFeature->results();
          std::list<ResultPtr>::const_iterator anIt;
          for (anIt = aResults.begin(); anIt != aResults.end() && aCanErase; ++anIt) {
            aCanErase = *anIt != aResult;
          }
        }
      }
      if (!aCanErase) {
        FeaturePtr aFeature = ModelAPI_Feature::feature(anObject);
        if (aFeature.get() && anObjectsToBeDeleted.find(aFeature) == anObjectsToBeDeleted.end()) {
          anObjectsToBeDeleted.insert(aFeature);
          anObjectsToBeDeletedNames.append(aFeature->name().c_str());
        }
      }
    }
    if (!anObjectsToBeDeleted.empty()) {
      QString aFeatureNames = anObjectsToBeDeletedNames.join(", ");
      QString aMessage = tr("The following features have incorrect presentation and \
will be hidden: %1. Would you like to delete them?")
                         .arg(aFeatureNames);
      int anAnswer = QMessageBox::question(qApp->activeWindow(), tr("Features hide"),
                                           aMessage, QMessageBox::Ok | QMessageBox::Cancel,
                                           QMessageBox::Cancel);
      if (anAnswer == QMessageBox::Ok) {
        QObjectPtrList anObjects;
        std::set<FeaturePtr>::const_iterator anIt = anObjectsToBeDeleted.begin(),
                                             aLast = anObjectsToBeDeleted.end();
        for (; anIt != aLast; anIt++)
          anObjects.append(*anIt);
        SessionPtr aMgr = ModelAPI_Session::get();
        DocumentPtr aDoc = aMgr->activeDocument();
        bool aIsOp = aMgr->isOperation();
        if (!aIsOp)
          aMgr->startOperation();
        workshop()->deleteFeatures(anObjects);
        //static Events_ID aDeletedEvent = Events_Loop::eventByName(EVENT_OBJECT_DELETED);
        //static Events_ID aRedispEvent = Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY);
        //Events_Loop::loop()->flush(aDeletedEvent);
        //Events_Loop::loop()->flush(aRedispEvent);

        if (!aIsOp)
          aMgr->finishOperation();
      }
    }
  }
}

bool PartSet_SketcherMgr::canDisplayCurrentCreatedFeature() const
{
  bool aCanDisplay = myIsMouseOverWindow;
  if (!aCanDisplay) {
    ModuleBase_ModelWidget* anActiveWidget = getActiveWidget();
    if (anActiveWidget)
      aCanDisplay = anActiveWidget->getValueState() == ModuleBase_ModelWidget::Stored;
  }
  return aCanDisplay;
}

bool PartSet_SketcherMgr::canChangeCursor(ModuleBase_Operation* theOperation) const
{
  return isNestedCreateOperation(theOperation) ||
         myModule->sketchReentranceMgr()->isInternalEditActive();
}

const QMap<PartSet_Tools::ConstraintVisibleState, bool>& PartSet_SketcherMgr::showConstraintStates()
{
  return myIsConstraintsShown;
}

bool PartSet_SketcherMgr::isObjectOfSketch(const ObjectPtr& theObject) const
{
  bool isFoundObject = false;

  FeaturePtr anObjectFeature = ModelAPI_Feature::feature(theObject);
  if (anObjectFeature.get()) {
    int aSize = myCurrentSketch->numberOfSubs();
    for (int i = 0; i < myCurrentSketch->numberOfSubs() && !isFoundObject; i++) {
      FeaturePtr aCurrentFeature = myCurrentSketch->subFeature(i);
      isFoundObject = myCurrentSketch->subFeature(i) == anObjectFeature;
    }
  }
  return isFoundObject;
}

void PartSet_SketcherMgr::onPlaneSelected(const std::shared_ptr<GeomAPI_Pln>& thePln)
{
  if (myPlaneFilter.IsNull()) 
   myPlaneFilter = new ModuleBase_ShapeInPlaneFilter();

  myPlaneFilter->setPlane(thePln);
}

bool PartSet_SketcherMgr::setDistanceValueByPreselection(ModuleBase_Operation* theOperation,
                                                         ModuleBase_IWorkshop* theWorkshop)
{
  bool isValueAccepted = false;

  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                              (theOperation);
  FeaturePtr aFeature = aFOperation->feature();
  // editor is shown only if all attribute references are filled by preseletion
  bool anAllRefAttrInitialized = true;

  std::list<AttributePtr> aRefAttrs = aFeature->data()->attributes(
                                              ModelAPI_AttributeRefAttr::typeId());
  std::list<AttributePtr>::const_iterator anIt = aRefAttrs.begin(), aLast = aRefAttrs.end();
  for (; anIt != aLast && anAllRefAttrInitialized; anIt++) {
    anAllRefAttrInitialized = (*anIt)->isInitialized();
  }
  if (anAllRefAttrInitialized) {
    // Activate dimension value editing on double click
    ModuleBase_IPropertyPanel* aPanel = aFOperation->propertyPanel();
    QList<ModuleBase_ModelWidget*> aWidgets = aPanel->modelWidgets();
    // Find corresponded widget to activate value editing
    foreach (ModuleBase_ModelWidget* aWgt, aWidgets) {
      if (aWgt->attributeID() == "ConstraintValue") {
        // the featue should be displayed in order to find the AIS text position,
        // the place where the editor will be shown
        aFeature->setDisplayed(true);
        /// the execute is necessary to perform in the feature compute for flyout position
        aFeature->execute();

        Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_CREATED));
        Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));

        PartSet_WidgetEditor* anEditor = dynamic_cast<PartSet_WidgetEditor*>(aWgt);
        if (anEditor) {
          int aX = 0, anY = 0;

          XGUI_Workshop* aWorkshop = XGUI_Tools::workshop(theWorkshop);
          XGUI_Displayer* aDisplayer = aWorkshop->displayer();
          AISObjectPtr anAIS = aDisplayer->getAISObject(aFeature);
          Handle(AIS_InteractiveObject) anAISIO;
          if (anAIS.get() != NULL) {
            anAISIO = anAIS->impl<Handle(AIS_InteractiveObject)>();
          }
          if (anAIS.get() != NULL) {
            Handle(AIS_InteractiveObject) anAISIO = anAIS->impl<Handle(AIS_InteractiveObject)>();

            if (!anAISIO.IsNull()) {
              Handle(AIS_Dimension) aDim = Handle(AIS_Dimension)::DownCast(anAISIO);
              if (!aDim.IsNull()) {
                gp_Pnt aPosition = aDim->GetTextPosition();

                ModuleBase_IViewer* aViewer = aWorkshop->viewer();
                Handle(V3d_View) aView = aViewer->activeView();
                int aCX, aCY;
                aView->Convert(aPosition.X(), aPosition.Y(), aPosition.Z(), aCX, aCY);

                QWidget* aViewPort = aViewer->activeViewPort();
                QPoint aGlPoint = aViewPort->mapToGlobal(QPoint(aCX, aCY));
                aX = aGlPoint.x();
                anY = aGlPoint.y();
              }
            }
            anEditor->setCursorPosition(aX, anY);
            isValueAccepted = anEditor->showPopupEditor(false);
          }
        }
      }
    }
  }
  return isValueAccepted;
}

void PartSet_SketcherMgr::getCurrentSelection(const FeaturePtr& theFeature,
                                              const FeaturePtr& theSketch,
                                              ModuleBase_IWorkshop* theWorkshop,
                                              FeatureToSelectionMap& theSelection)
{
  if (theFeature.get() == NULL)
    return;

  std::set<AttributePtr> aSelectedAttributes;
  std::set<ResultPtr> aSelectedResults;

  ModuleBase_IViewer* aViewer = theWorkshop->viewer();
  Handle(AIS_InteractiveContext) aContext = aViewer->AISContext();
  if (!aContext.IsNull()) {
    XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWorkshop);
    XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

    std::list<ResultPtr> aResults = theFeature->results();
    std::list<ResultPtr>::const_iterator aIt;
    for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt)
    {
      ResultPtr aResult = *aIt;
      AISObjectPtr aAISObj = aDisplayer->getAISObject(aResult);
      if (aAISObj.get() == NULL)
        continue;
      Handle(AIS_InteractiveObject) anAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();
      for (aContext->InitSelected(); aContext->MoreSelected(); aContext->NextSelected())
      {
        Handle(SelectMgr_EntityOwner) anOwner = aContext->SelectedOwner();
        if (anOwner->Selectable() != anAISIO)
          continue;
        getAttributesOrResults(anOwner, theFeature, theSketch, aResult,
                               aSelectedAttributes, aSelectedResults);
      }
      for (aContext->InitDetected(); aContext->MoreDetected(); aContext->NextDetected()) {
        Handle(SelectMgr_EntityOwner) anOwner = aContext->DetectedOwner();
        if (anOwner.IsNull())
          continue;
        if (anOwner->Selectable() != anAISIO)
          continue;
        getAttributesOrResults(anOwner, theFeature, theSketch, aResult,
                               aSelectedAttributes, aSelectedResults);
      }
    }
  }
  theSelection[theFeature] = std::make_pair(aSelectedAttributes, aSelectedResults);
}

void PartSet_SketcherMgr::getSelectionOwners(const FeaturePtr& theFeature,
                                             const FeaturePtr& theSketch,
                                             ModuleBase_IWorkshop* theWorkshop,
                                             const FeatureToSelectionMap& theSelection,
                                             SelectMgr_IndexedMapOfOwner& theOwnersToSelect)
{
  if (theFeature.get() == NULL)
    return;

  FeatureToSelectionMap::const_iterator anIt = theSelection.find(theFeature);
  std::set<AttributePtr> aSelectedAttributes = anIt.value().first;
  std::set<ResultPtr> aSelectedResults = anIt.value().second;

  ModuleBase_IViewer* aViewer = theWorkshop->viewer();

  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(theWorkshop);
  XGUI_Displayer* aDisplayer = aConnector->workshop()->displayer();

  // 1. found the feature's owners. Check the AIS objects of the constructions
  AISObjectPtr aAISObj = aDisplayer->getAISObject(theFeature);
  if (aAISObj.get() != NULL && aSelectedAttributes.empty() && aSelectedResults.empty()) {
    Handle(AIS_InteractiveObject) anAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();

    SelectMgr_IndexedMapOfOwner aSelectedOwners;
    aConnector->workshop()->selector()->selection()->entityOwners(anAISIO, aSelectedOwners);
    for  (Standard_Integer i = 1, n = aSelectedOwners.Extent(); i <= n; i++) {
      Handle(SelectMgr_EntityOwner) anOwner = aSelectedOwners(i);
      if (!anOwner.IsNull())
        theOwnersToSelect.Add(anOwner);
    }
  }

  // 2. found the feature results's owners
  std::list<ResultPtr> aResults = theFeature->results();
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt)
  {
    ResultPtr aResult = *aIt;
    AISObjectPtr aAISObj = aDisplayer->getAISObject(aResult);
    if (aAISObj.get() == NULL)
      continue; 
    Handle(AIS_InteractiveObject) anAISIO = aAISObj->impl<Handle(AIS_InteractiveObject)>();

    SelectMgr_IndexedMapOfOwner aSelectedOwners;  
    aConnector->workshop()->selector()->selection()->entityOwners(anAISIO, aSelectedOwners);
    for  ( Standard_Integer i = 1, n = aSelectedOwners.Extent(); i <= n; i++ ) {
      Handle(StdSelect_BRepOwner) anOwner = Handle(StdSelect_BRepOwner)::DownCast(aSelectedOwners(i));
      if ( anOwner.IsNull() || !anOwner->HasShape() )
        continue;
      const TopoDS_Shape& aShape = anOwner->Shape();
      TopAbs_ShapeEnum aShapeType = aShape.ShapeType();
      if (aShapeType == TopAbs_VERTEX) {
        AttributePtr aPntAttr = PartSet_Tools::findAttributeBy2dPoint(theFeature, aShape, theSketch);
        if (aPntAttr.get() != NULL &&
            aSelectedAttributes.find(aPntAttr) != aSelectedAttributes.end()) {
          theOwnersToSelect.Add(anOwner);
        }
      }
      else if (aShapeType == TopAbs_EDGE) {
        bool aFound = aSelectedResults.find(aResult) != aSelectedResults.end();
        if (aSelectedResults.find(aResult) != aSelectedResults.end() &&
            theOwnersToSelect.FindIndex(anOwner) <= 0)
          theOwnersToSelect.Add(anOwner);
      }
    }
  }
}

void PartSet_SketcherMgr::connectToPropertyPanel(ModuleBase_ModelWidget* theWidget, const bool isToConnect)
{
  if (isToConnect) {
    connect(theWidget, SIGNAL(beforeValuesChanged()),
            this, SLOT(onBeforeValuesChangedInPropertyPanel()));
    connect(theWidget, SIGNAL(afterValuesChanged()),
            this, SLOT(onAfterValuesChangedInPropertyPanel()));
  }
  else {
    disconnect(theWidget, SIGNAL(beforeValuesChanged()),
                this, SLOT(onBeforeValuesChangedInPropertyPanel()));
    disconnect(theWidget, SIGNAL(afterValuesChanged()),
                this, SLOT(onAfterValuesChangedInPropertyPanel()));
  }
}

void PartSet_SketcherMgr::widgetStateChanged(int thePreviousState)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                           (getCurrentOperation());
  if (aFOperation) {
    if (PartSet_SketcherMgr::isSketchOperation(aFOperation) ||
        PartSet_SketcherMgr::isNestedSketchOperation(aFOperation) &&
        thePreviousState == ModuleBase_ModelWidget::ModifiedInPP) {
      FeaturePtr aFeature = aFOperation->feature();
      visualizeFeature(aFeature, aFOperation->isEditOperation(), canDisplayObject(aFeature));
    }
  }
}

void PartSet_SketcherMgr::customizePresentation(const ObjectPtr& theObject)
{
  ModuleBase_OperationFeature* aFOperation = dynamic_cast<ModuleBase_OperationFeature*>
                                                                           (getCurrentOperation());
  if (aFOperation && (PartSet_SketcherMgr::isSketchOperation(aFOperation) ||
                      PartSet_SketcherMgr::isNestedSketchOperation(aFOperation)))
    SketcherPrs_Tools::sendExpressionShownEvent(myIsConstraintsShown[PartSet_Tools::Expressions]);
}

ModuleBase_Operation* PartSet_SketcherMgr::getCurrentOperation() const
{
  return myModule->workshop()->currentOperation();
}

//**************************************************************
ModuleBase_ModelWidget* PartSet_SketcherMgr::getActiveWidget() const
{
  ModuleBase_ModelWidget* aWidget = 0;
  ModuleBase_Operation* anOperation = getCurrentOperation();
  if (anOperation) {
    ModuleBase_IPropertyPanel* aPanel = anOperation->propertyPanel();
    if (aPanel)
      aWidget = aPanel->activeWidget();
  }
  return aWidget;
}

void PartSet_SketcherMgr::visualizeFeature(const FeaturePtr& theFeature,
                                           const bool isEditOperation,
                                           const bool isToDisplay,
                                           const bool isFlushRedisplay)
{
  #ifdef DEBUG_DO_NOT_BY_ENTER
  return;
  #endif

  if (isEditOperation || !theFeature.get())
    return;

  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);

  // 1. change visibility of the object itself, here the presentable object is processed,
  // e.g. constraints features
  //FeaturePtr aFeature = aFOperation->feature();
  std::list<ResultPtr> aResults = theFeature->results();
  if (isToDisplay)
    theFeature->setDisplayed(true);
  else
    theFeature->setDisplayed(false);

  // change visibility of the object results, e.g. non-constraint features
  std::list<ResultPtr>::const_iterator aIt;
  for (aIt = aResults.begin(); aIt != aResults.end(); ++aIt) {
    if (isToDisplay) {
      (*aIt)->setDisplayed(true);
    }
    else {
      (*aIt)->setDisplayed(false);
    }
  }
  if (isFlushRedisplay)
    Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
}

void PartSet_SketcherMgr::storeSelection(const bool theHighlightedOnly)
{
  if (!myCurrentSketch.get())
    return;

  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  ModuleBase_ISelection* aSelect = aWorkshop->selection();
  QList<ModuleBase_ViewerPrs> aHighlighted = aSelect->getHighlighted();

  QList<FeaturePtr> aFeatureList;
  if (theHighlightedOnly) {
    fillFeatureList(aHighlighted, myCurrentSketch, aFeatureList);
  }
  else {
    fillFeatureList(aHighlighted, myCurrentSketch, aFeatureList);

    QList<ModuleBase_ViewerPrs> aSelected = aSelect->getSelected(ModuleBase_ISelection::AllControls);
    fillFeatureList(aSelected, myCurrentSketch, aFeatureList);
  }

  // 1. it is necessary to save current selection in order to restore it after the features moving
  myCurrentSelection.clear();
  QList<FeaturePtr>::const_iterator anIt = aFeatureList.begin(), aLast = aFeatureList.end();
  for (; anIt != aLast; anIt++) {
    getCurrentSelection(*anIt, myCurrentSketch, aWorkshop, myCurrentSelection);
  }
  //qDebug(QString("  storeSelection: %1").arg(myCurrentSelection.size()).toStdString().c_str());
}

void PartSet_SketcherMgr::restoreSelection()
{
  if (!myCurrentSketch.get())
    return;

  //qDebug(QString("restoreSelection: %1").arg(myCurrentSelection.size()).toStdString().c_str());
  ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
  FeatureToSelectionMap::const_iterator aSIt = myCurrentSelection.begin(),
                                        aSLast = myCurrentSelection.end();
  SelectMgr_IndexedMapOfOwner anOwnersToSelect;
  for (; aSIt != aSLast; aSIt++) {
    anOwnersToSelect.Clear();
    getSelectionOwners(aSIt.key(), myCurrentSketch, aWorkshop, myCurrentSelection,
                        anOwnersToSelect);
    aConnector->workshop()->selector()->setSelectedOwners(anOwnersToSelect, false);
  }
}

void PartSet_SketcherMgr::onShowConstraintsToggle(int theType, bool theState)
{
  PartSet_Tools::ConstraintVisibleState aType = (PartSet_Tools::ConstraintVisibleState)theType;

  updateBySketchParameters(aType, theState);
}

void PartSet_SketcherMgr::updateBySketchParameters(
                                   const PartSet_Tools::ConstraintVisibleState& theType,
                                   bool theState)
{
  if (myCurrentSketch.get() == NULL)
    return;

  bool aPrevState = myIsConstraintsShown[theType];
  myIsConstraintsShown[theType] = theState;

  switch (theType) {
    case PartSet_Tools::Geometrical:
    case PartSet_Tools::Dimensional: {
      if (aPrevState != theState) {
        ModuleBase_IWorkshop* aWorkshop = myModule->workshop();
        XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(aWorkshop);
        for (int i = 0; i < myCurrentSketch->numberOfSubs(); i++) {
          FeaturePtr aSubFeature = myCurrentSketch->subFeature(i);
          bool aProcessed = false;
          bool aConstraintDisplayed = canDisplayConstraint(aSubFeature, theType, aProcessed);
          if (aProcessed)
            aSubFeature->setDisplayed(aConstraintDisplayed);
        }
        Events_Loop::loop()->flush(Events_Loop::eventByName(EVENT_OBJECT_TO_REDISPLAY));
      }
    }
    break;
    case PartSet_Tools::Expressions: {
      /// call all sketch features redisplay, the expression state will be corrected in customize
      /// of distance presentation
      Events_ID anEventId = Events_Loop::loop()->eventByName(EVENT_OBJECT_TO_REDISPLAY);
      PartSet_Tools::sendSubFeaturesEvent(myCurrentSketch, anEventId);
    }
    break;
  }
}

XGUI_Workshop* PartSet_SketcherMgr::workshop() const
{
  ModuleBase_IWorkshop* anIWorkshop = myModule->workshop();
  XGUI_ModuleConnector* aConnector = dynamic_cast<XGUI_ModuleConnector*>(anIWorkshop);
  return aConnector->workshop();
}

XGUI_OperationMgr* PartSet_SketcherMgr::operationMgr() const
{
  return workshop()->operationMgr();
}

